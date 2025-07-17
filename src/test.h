#include "i2c.h"
#include "defines.h"
#include "esp32_flasher.h"

#include "ui/ui.h"

extern uint8_t voltage[3];
extern uint16_t current[3];
extern uint8_t temperature[3];
extern uint8_t max_temperature[3];
extern uint8_t error[3];

extern uint8_t power_48V;

extern ESP32Flasher espflasher;

void read_params();
void check_params();
void draw_params();
void set_current(uint8_t CURRENT);
void set_channel_val(uint8_t CHANNEL, uint8_t VALUE);
void set_channels_val(uint8_t VALUE);
void test_procedure();
void flash_programm();
