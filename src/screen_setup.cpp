#include "screen_setup.h"

// wymiary ekranu
static const uint16_t screenWidth  = 480;
static const uint16_t screenHeight = 320;

// bufory
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

void my_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p ) {
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( (uint16_t *)color_p, w * h, true ); 
    tft.endWrite();

    lv_disp_flush_ready( disp_drv );
}

void my_touchpad_read( lv_indev_drv_t * indev_drv, lv_indev_data_t * data ) {
    uint16_t touchX, touchY;
    bool touched = tft.getTouch( &touchX, &touchY, 600 );

    if ( !touched ) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

void init_tft(){
  tft.begin();
  // orientacja ekranu
  tft.setRotation(3);
  // dane kalibracyjne dla wyswietlacza 480x320
  uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
  tft.setTouch( calData );
  // Podswietlenie Ekranu
  pinMode(17, OUTPUT);
  digitalWrite(17, HIGH);

}

void init_lvgl(){

    lv_init();
    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    // inicjalizacja wyswietlacza
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    // inicjalizacja dotyku
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );
    
    // inicjalizacja UI 
    ui_init();
}