#include "JoyConProtocol.h"

// Temporary global state (will be removed later)
int  handle_ok = 0;
bool enable_button_test = false;
bool enable_IRVideoPhoto = false;
bool enable_IRAutoExposure = false;
bool enable_NFCScanning = false;
bool cancel_spi_dump = false;
bool check_connection_ok = false;

u8 timming_byte = 0;
u8 ir_max_frag_no = 0;
