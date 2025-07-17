#include "Arduino.h"
#include "TFT_eSPI.h"
#include "lvgl.h"
#include "ui/ui.h"

extern TFT_eSPI tft;

void my_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p );
void my_touchpad_read( lv_indev_drv_t * indev_drv, lv_indev_data_t * data );
void init_tft();
void init_lvgl();