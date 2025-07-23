#include "i2c.h"
#include "defines.h"
#include "esp32_flasher.h"

#include "ui/ui.h"

#define PROG_VER 3

#define SUCCESS 0
#define ESP_ERROR 1
#define NCL_CH1_ERROR 2
#define NCL_CH2_ERROR 3
#define NCL_CH3_ERROR 4
#define NCL_CH1_I_ERROR 5
#define NCL_CH2_I_ERROR 6
#define NCL_CH3_I_ERROR 7

extern uint8_t voltage[3];
extern uint16_t current[3];
extern uint8_t temperature[3];
extern uint8_t max_temperature[3];
extern uint8_t error[3];

extern uint8_t power_48V;
extern bool testing;
extern uint8_t testing_bar;
//extern char buf[100];

extern ESP32Flasher espflasher;

void read_params();
void check_params();
void draw_params();
void set_current(uint8_t CURRENT);
void set_channel_val(uint8_t CHANNEL, uint8_t VALUE);
void set_channels_val(uint8_t VALUE);
uint8_t test_procedure();
void flash_programm();
void err_code_to_label(uint8_t err_code);
