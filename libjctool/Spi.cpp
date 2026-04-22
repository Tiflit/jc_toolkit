#include "Spi.h"
#include <hidapi/hidapi.h>
#include <cstring>

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// Advance the timing byte (sequence counter).  Wraps 0x0–0xF.
static inline void advance_timing(u8& t) {
    t = (t + 1) & 0x0F;
}

// Build a minimal neutral rumble payload (no vibration).
static void fill_neutral_rumble(u8* rumble) {
    // Standard "neutral" rumble bytes used by jctool
    rumble[0] = 0x00;
    rumble[1] = 0x01;
    rumble[2] = 0x40;
    rumble[3] = 0x40;
}

// Send one subcommand and wait for the matching reply.
// Returns bytes read (>0) on success, <=0 on failure/timeout.
static int send_subcmd(hid_device* handle, u8& timing_byte,
                       u8 subcmd, const uint8_t* args, int args_len,
                       uint8_t* reply, int reply_size,
                       int timeout_ms = 100)
{
    uint8_t buf[0x40] = {};
    buf[0] = 0x00;           // report id (prepended by hidapi)
    buf[1] = 0x01;           // cmd
    buf[2] = timing_byte;
    advance_timing(timing_byte);

    fill_neutral_rumble(&buf[3]);   // rumble_l
    fill_neutral_rumble(&buf[7]);   // rumble_r

    buf[11] = subcmd;
    if (args && args_len > 0) {
        std::memcpy(&buf[12], args, args_len);
    }

    // hidapi write expects the report-id byte as the first byte
    int res = hid_write(handle, buf, sizeof(buf));
    if (res < 0) return res;

    // Read replies until we get the ack for our subcommand (0x21)
    for (int attempt = 0; attempt < 10; ++attempt) {
        std::memset(reply, 0, reply_size);
        res = hid_read_timeout(handle, reply, reply_size, timeout_ms);
        if (res <= 0) return res;
        if (reply[0] == 0x21 && reply[14] == subcmd) return res;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// SPI read
// ---------------------------------------------------------------------------
int jc_get_spi_data(hid_device* handle, u8& timing_byte,
                    uint32_t offset, uint16_t read_len, uint8_t* buffer)
{
    // Subcommand 0x10: read SPI flash
    // Args: 4-byte LE offset + 1-byte length
    uint8_t args[5];
    args[0] = static_cast<u8>( offset        & 0xFF);
    args[1] = static_cast<u8>((offset >>  8) & 0xFF);
    args[2] = static_cast<u8>((offset >> 16) & 0xFF);
    args[3] = static_cast<u8>((offset >> 24) & 0xFF);
    args[4] = static_cast<u8>(read_len & 0xFF);

    uint8_t reply[0x40] = {};
    int res = send_subcmd(handle, timing_byte, 0x10, args, 5, reply, sizeof(reply));
    if (res <= 0) return -1;

    // Reply layout: [0]=0x21, [14]=0x10 (subcmd echo),
    //               [15..18]=offset echo, [19]=length echo, [20+]=data
    std::memcpy(buffer, &reply[20], read_len);
    return 0;
}

// ---------------------------------------------------------------------------
// SPI write
// ---------------------------------------------------------------------------
int jc_write_spi_data(hid_device* handle, u8& timing_byte,
                      uint32_t offset, uint16_t write_len, const uint8_t* buffer)
{
    // Subcommand 0x11: write SPI flash
    uint8_t args[5 + 0x1D] = {};
    args[0] = static_cast<u8>( offset        & 0xFF);
    args[1] = static_cast<u8>((offset >>  8) & 0xFF);
    args[2] = static_cast<u8>((offset >> 16) & 0xFF);
    args[3] = static_cast<u8>((offset >> 24) & 0xFF);
    args[4] = static_cast<u8>(write_len & 0xFF);
    std::memcpy(&args[5], buffer, write_len);

    uint8_t reply[0x40] = {};
    int res = send_subcmd(handle, timing_byte, 0x11, args, 5 + write_len,
                          reply, sizeof(reply));
    return (res > 0) ? 0 : -1;
}

// ---------------------------------------------------------------------------
// Device info helpers
// ---------------------------------------------------------------------------
int jc_get_device_info(hid_device* handle, u8& timing_byte, uint8_t* buffer) {
    uint8_t reply[0x40] = {};
    int res = send_subcmd(handle, timing_byte, 0x02, nullptr, 0, reply, sizeof(reply));
    if (res <= 0) return -1;
    std::memcpy(buffer, &reply[15], 12);
    return 0;
}

int jc_get_battery(hid_device* handle, u8& timing_byte, uint8_t* buffer) {
    uint8_t reply[0x40] = {};
    int res = send_subcmd(handle, timing_byte, 0x50, nullptr, 0, reply, sizeof(reply));
    if (res <= 0) return -1;
    std::memcpy(buffer, &reply[15], 5);
    return 0;
}

int jc_get_temperature(hid_device* handle, u8& timing_byte, uint8_t* buffer) {
    uint8_t reply[0x40] = {};
    int res = send_subcmd(handle, timing_byte, 0x43, nullptr, 0, reply, sizeof(reply));
    if (res <= 0) return -1;
    std::memcpy(buffer, &reply[15], 4);
    return 0;
}

// ---------------------------------------------------------------------------
// 12-bit packed stick parameter codec
//
// Six 12-bit values packed into 9 bytes, nibble-aligned LE:
//
//   encoded[0]        = val0[ 7:0]
//   encoded[1][3:0]   = val0[11:8]
//   encoded[1][7:4]   = val1[ 3:0]
//   encoded[2]        = val1[11:4]
//   encoded[3]        = val2[ 7:0]
//   encoded[4][3:0]   = val2[11:8]
//   encoded[4][7:4]   = val3[ 3:0]
//   encoded[5]        = val3[11:4]
//   encoded[6]        = val4[ 7:0]
//   encoded[7][3:0]   = val4[11:8]
//   encoded[7][7:4]   = val5[ 3:0]
//   encoded[8]        = val5[11:4]
// ---------------------------------------------------------------------------
void jc_decode_stick_params(uint16_t* decoded, const uint8_t* encoded) {
    decoded[0] = static_cast<uint16_t>( encoded[0]         | ((encoded[1] & 0x0F) << 8));
    decoded[1] = static_cast<uint16_t>((encoded[1] >> 4)   |  (encoded[2]         << 4));
    decoded[2] = static_cast<uint16_t>( encoded[3]         | ((encoded[4] & 0x0F) << 8));
    decoded[3] = static_cast<uint16_t>((encoded[4] >> 4)   |  (encoded[5]         << 4));
    decoded[4] = static_cast<uint16_t>( encoded[6]         | ((encoded[7] & 0x0F) << 8));
    decoded[5] = static_cast<uint16_t>((encoded[7] >> 4)   |  (encoded[8]         << 4));
}

void jc_encode_stick_params(uint8_t* encoded, const uint16_t* decoded) {
    encoded[0] = static_cast<u8>( decoded[0]        & 0xFF);
    encoded[1] = static_cast<u8>((decoded[0] >> 8)  & 0x0F) |
                 static_cast<u8>((decoded[1]        & 0x0F) << 4);
    encoded[2] = static_cast<u8>((decoded[1] >> 4)  & 0xFF);
    encoded[3] = static_cast<u8>( decoded[2]        & 0xFF);
    encoded[4] = static_cast<u8>((decoded[2] >> 8)  & 0x0F) |
                 static_cast<u8>((decoded[3]        & 0x0F) << 4);
    encoded[5] = static_cast<u8>((decoded[3] >> 4)  & 0xFF);
    encoded[6] = static_cast<u8>( decoded[4]        & 0xFF);
    encoded[7] = static_cast<u8>((decoded[4] >> 8)  & 0x0F) |
                 static_cast<u8>((decoded[5]        & 0x0F) << 4);
    encoded[8] = static_cast<u8>((decoded[5] >> 4)  & 0xFF);
}
