#include <Arduino.h>
#include "test.h"
#include "screen_setup.h"
#include "html.h"
#include "defines.h"
#include "esp32_flasher.h"

#include "ui/ui.h"

// Odswiezanie ekranu
void screen_task(void *pvParameters){
    for(;;){
        server.handleClient();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// Webserver
void web_task(void *pvParameters){
    for(;;){
        lv_timer_handler();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// Sprawdzenie działań
void programm_task(void *pvParameters){
    for(;;){
        // Flaga programowania
        if (p_start){
            flash_programm();
            p_start = false;
        }
        // Sprawdzenie seriala
        if (logSerial1) {
            while (Serial2.available()) {
                char c = Serial2.read();
                Serial.print(c);
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// Diagnostyka NCLa
void diagnostic_task(void *pvParameters){
    for(;;){
        if (diagnostic_sw){
            //digitalWrite(RELAY, HIGH);
            read_params();
            check_params();
            draw_params();
        } 
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void manual_task(void *pvParameters){
    for (;;){
        // Przekaznik
        if (power_switch && !power_48V){
            digitalWrite(RELAY, HIGH);
            power_48V = true;
        }
        else if (!power_switch && power_48V){
            digitalWrite(RELAY, LOW);
            power_48V = false;
        }
        // Maksymalny prad
        if (new_I){
            set_current(max_I);
            new_I = false;
        }
        // Wysterowanie kanałów
        if (new_ch1){
            set_channel_val(1, ch1_power);
            new_ch1 = false;
        }
        if (new_ch2){
            set_channel_val(2, ch2_power);
            new_ch2 = false;
        }
        if (new_ch3){
            set_channel_val(3, ch3_power);
            new_ch3 = false;
        }
        //Serial.printf("Power: %d   Max_I: %d   Ch1: %d   Ch2: %d   Ch3: %d\n", power_switch, max_I, ch1_power, ch2_power, ch3_power);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup(){
    Serial.begin(115200);
    // Inicjalizacja wyswietlacza
    init_tft();
    init_lvgl();
    Serial.println("Screen init Complete");
    // Inicjalizacja WiFi
    wifi_setup();
    spiffs_setup();
    // Inicjalizacja UART do flashowania
    espflasher.espFlasherInit();
    // Inicjalizacja I2C
    if (I2Cinit())
        Serial.println("I2C init: OK");
    else
        Serial.println("I2C init: NOK");
    // Przekaznik 48 V - wylacz
    pinMode(RELAY, OUTPUT);
    digitalWrite(RELAY, LOW);  
    // BUZZER - wylacz
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, LOW);  
    // freeRTOS
    xTaskCreate(screen_task, "screenTask", 4096, NULL, 1, NULL);
    xTaskCreate(web_task, "webTask", 4096, NULL, 2, NULL);
    xTaskCreate(programm_task, "programmTask", 4096, NULL, 3, NULL);
    xTaskCreate(diagnostic_task, "diagnosticTask", 4096, NULL, 4, NULL);
    xTaskCreate(manual_task, "manualTask", 4096, NULL, 5, NULL);
}

void loop(){

}