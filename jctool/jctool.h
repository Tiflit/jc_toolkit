#include "../libjctool/JoyConProtocol.h"

#pragma once

#include <cstdint>

template <typename T> T CLAMP(const T& value, const T& low, const T& high)
{
    return value < low ? low : (value > high ? high : value);
}

#pragma pack(push, 1)

#pragma pack(pop)

extern s16 uint16_to_int16(u16 a);
extern void decode_stick_params(u16 *decoded_stick_params, u8 *encoded_stick_params);
extern void encode_stick_params(u8 *encoded_stick_params, u16 *decoded_stick_params);

extern std::string get_sn(u32 offset, const u16 read_len);
extern int get_spi_data(u32 offset, const u16 read_len, u8 *test_buf);
extern int write_spi_data(u32 offset, const u16 write_len, u8* test_buf);
extern int get_device_info(u8* test_buf);
extern int get_battery(u8* test_buf);
extern int get_temperature(u8* test_buf);
extern int dump_spi(const char *dev_name);
extern int send_rumble();
extern int play_tune(int tune_no);
extern int play_hd_rumble_file(int file_type, u16 sample_rate, int samples, int loop_start, int loop_end, int loop_wait, int loop_times);
extern int send_custom_command(u8* arg);
extern int device_connection();
extern int set_led_busy();
extern int button_test();
extern int ir_sensor(ir_image_config &ir_cfg);
extern int ir_sensor_config_live(ir_image_config &ir_cfg);
extern int nfc_tag_info();
extern int silence_input_report();
int  handle_ok;
bool enable_button_test;
bool enable_IRVideoPhoto;
bool enable_IRAutoExposure;
bool enable_NFCScanning;
bool cancel_spi_dump;
bool check_connection_ok;

u8 timming_byte;
u8 ir_max_frag_no;

namespace CppWinFormJoy {
    class images
    {
        //For annoying designer..
        //Todo.
    };
}
