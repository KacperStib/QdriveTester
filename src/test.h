#include "i2c.h"
#include "defines.h"
#include "esp32_flasher.h"

#include "ui/ui.h"

#define PROG_VER 4

#define SUCCESS 0
#define ESP_ERROR 1
#define ESP_OVERHEAT 2
#define NCL_CH1_ERROR 3
#define NCL_CH2_ERROR 5
#define NCL_CH3_ERROR 6
#define NCL_CH1_I_ERROR 7
#define NCL_CH2_I_ERROR 8
#define NCL_CH3_I_ERROR 9
#define NCL_CH1_T_ERROR 10
#define NCL_CH2_T_ERROR 11
#define NCL_CH3_T_ERROR 12

// Sprawdzanie przycisku STOP
#define TEST_STOPPED 255
#define CHECK_STOP() if (stop_req) return TEST_STOPPED;
#define CHECK_INTERVAL_MS 100  
#define ALL_CHANNELS 4
#define NO_ERR_CHECK 5
#define DELAY_WITH_CHECK(ms, ch) \
    do { \
        vTaskDelay(3000 / portTICK_PERIOD_MS); \
        int elapsed = 3000; \
        while (elapsed < ms) { \
            if (stop_req) return TEST_STOPPED; \
            if (ch < 3) {if(error[ch] & (1 << 0)) return NCL_CH1_I_ERROR + ch; if (error[ch] & (1 << 1)) return NCL_CH1_T_ERROR + ch;} \
            if (ch == 4) {for (int i = 0; i < 3; i++){if (error[i] & (1 << 0)) return NCL_CH1_I_ERROR + i; if (error[i] & (1 << 1)) return NCL_CH1_T_ERROR + i;}} \
            vTaskDelay(CHECK_INTERVAL_MS / portTICK_PERIOD_MS); \
            elapsed += CHECK_INTERVAL_MS; \
        } \
    } while (0)

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

uint8_t read_esp32_temp();
void read_params();
void check_params();
void draw_params();
void set_current(uint8_t CURRENT);
void set_channel_val(uint8_t CHANNEL, uint8_t VALUE);
void set_channels_val(uint8_t VALUE);
uint8_t test_procedure();
void flash_programm();
void err_code_to_label(uint8_t err_code);
