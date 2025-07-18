#include "test.h"

uint8_t voltage[3] = {0, 0, 0};
uint16_t current[3] = {0, 0, 0};
uint8_t temperature[3] = {0, 0, 0};
uint8_t max_temperature[3] = {0, 0, 0};
uint8_t error[3] = {0, 0, 0};

uint8_t power_48V = false;
bool testing = false;
//char buf[100];

ESP32Flasher espflasher;

// Odczyt parametrow NCLi
void read_params(){
    // pierwszy rejestr parametrow
    I2CwriteREG(SLAVE_ADDR, REG_CH1_VLED);
    const uint8_t bytesToReceive = 9;
    uint8_t buf[bytesToReceive];
    // Odczyt
    I2Cread(SLAVE_ADDR, buf, bytesToReceive);
    for (int i = 0; i < 3; i++){
        voltage[i] = buf[i];
        current[i] = buf[i+3] * 10;
        temperature[i] = buf[i+6];
    }
}

// Diagnoza parametrow NCLi
void check_params(){
    for (int i = 0; i < 3; i++){
        // maksymalne temperatury
        if (max_temperature[i] < temperature[i])
            max_temperature[i] = temperature[i];
        // error 0 bit - natezenie
        if (current[i] <= 0)
            error[i] |= (1 << 0);
        else   
            error[i] &= ~(1 << 0); 
    }
}

// Wypisywanie parametrow na ekranie
void draw_params(){    
    // Kanal 1
    sprintf(buf, "%d", voltage[0]);
    lv_label_set_text(ui_V11, buf);
    sprintf(buf, "%d", current[0]);
    lv_label_set_text(ui_I11, buf);
    sprintf(buf, "%d", temperature[0]);
    lv_label_set_text(ui_T11, buf);
    sprintf(buf, "%d", max_temperature[0]);
    lv_label_set_text(ui_MT1, buf);
    sprintf(buf, "%d", error[0]);
    lv_label_set_text(ui_Er1, buf);
    // Kanal 2
    sprintf(buf, "%d", voltage[1]);
    lv_label_set_text(ui_V22, buf);
    sprintf(buf, "%d", current[1]);
    lv_label_set_text(ui_I22, buf);
    sprintf(buf, "%d", temperature[1]);
    lv_label_set_text(ui_T22, buf);
    sprintf(buf, "%d", max_temperature[1]);
    lv_label_set_text(ui_MT2, buf);
    sprintf(buf, "%d", error[1]);
    lv_label_set_text(ui_Er2, buf);
    // Kanal 3
    sprintf(buf, "%d", voltage[2]);
    lv_label_set_text(ui_V33, buf);
    sprintf(buf, "%d", current[2]);
    lv_label_set_text(ui_I33, buf);
    sprintf(buf, "%d", temperature[2]);
    lv_label_set_text(ui_T33, buf);
    sprintf(buf, "%d", max_temperature[2]);
    lv_label_set_text(ui_MT3, buf);
    sprintf(buf, "%d", error[2]);
    lv_label_set_text(ui_Er3, buf);
}

// Ustaweienie maksymalnego pradu
void set_current(uint8_t CURRENT){
    I2CwriteVAL(SLAVE_ADDR, REG_MCU_COMMAND, 3);
    I2CwriteVAL(SLAVE_ADDR, REG_CH1_CURRENT, CURRENT);
    I2CwriteVAL(SLAVE_ADDR, REG_MCU_COMMAND, 3);
    I2CwriteVAL(SLAVE_ADDR, REG_CH2_CURRENT, CURRENT);
    I2CwriteVAL(SLAVE_ADDR, REG_MCU_COMMAND, 3);
    I2CwriteVAL(SLAVE_ADDR, REG_CH3_CURRENT, CURRENT);
}

// Ustawienie wysterowania pojedynczego kanalu
void set_channel_val(uint8_t CHANNEL, uint8_t VALUE){
    I2CwriteVAL(SLAVE_ADDR, CHANNEL + 1, VALUE);
}

// Ustawienie wysterowania wszystkich kanalow
void set_channels_val(uint8_t VALUE){
    for(int i = 1; i < 4; i++)
        set_channel_val(i, VALUE);
}

// Procedura testowa
uint8_t test_procedure(){
    // Flaga
    testing = true;
    // Odpytaj ESP o wersje programu NCL
    uint8_t ver = 0;
    I2CwriteREG(SLAVE_ADDR, REG_PROG_VER);
    I2Cread(SLAVE_ADDR, &ver, 1);
    // Blad espa
    if (ver != PROG_VER)
        return ESP_ERROR;
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Wlacz przekaznik
    digitalWrite(RELAY, HIGH);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    // Czy NCL odpowiada ? odczytaj 3 rejestry error i sprawdz czy = 0
    uint8_t buf[3] = {0, 0, 0};
    I2CwriteREG(SLAVE_ADDR, REG_CH1_ERROR);
    I2Cread(SLAVE_ADDR, buf, 3);
    // Blad NCLa
    for(int i = 0; i < 3; i++){
        if (buf[i] != 0)
            return NCL_CH1_ERROR + i;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Testowe prady
    set_current(TEST_CURRENT);

    // Po kolei 1 A
    for(int i = 0; i < 3 ; i++){
        set_channel_val(i+1, 100);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        // Blad
        if (error[i] != 0)
            return NCL_CH1_I_ERROR + i;
        set_channel_val(i+1, 0);
    }

    // Po kolei 2.5 A
    for(int i = 0; i < 3 ; i++){
        set_channel_val(i+1, 250);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        // Blad
        if (error[i] != 0)
            return NCL_CH1_I_ERROR + i;
        set_channel_val(i+1, 0);
    }

    // Rozjasnienie od 1 A do 2.5 A wszytkich naraz
    set_channels_val(100);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    for(int i = 100; i < 256; i++){
        set_channels_val(i);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        // Blad
        for(int j = 0; j < 3; j++){
            if (error[j] != 0)
                return NCL_CH1_I_ERROR + j;
        }
    }

    // Odczekanie
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    for(int i = 0; i < 3; i++){
        if (error[i] != 0)
        return NCL_CH1_I_ERROR + i;
    }

    // Wyniki testów
    //digitalWrite(BUZZER , HIGH);
    // Flaga
    testing = false;
    return SUCCESS;
}

// Wgrywanie programu NCL
void flash_programm(){
    if (SPIFFS.begin(true)) {
    Serial.println("spiffs begin");
    // Polaczenie ze slavem
    int connect_status = espflasher.espConnect();
    if (connect_status != SUCCESS) {
      Serial.print("Cannot connect to target");
    }
    Serial.print("Connected to target\n");
    // Programowanie slavea
    espflasher.espFlashBinFile("/firmware.bin");
    // Sygnal dzwiekowy
    // for (int i = 0; i < 5; i ++){
    //     digitalWrite(BUZZER, HIGH);
    //     vTaskDelay(200 / portTICK_PERIOD_MS);
    //     digitalWrite(BUZZER, LOW);
    //     vTaskDelay(200 / portTICK_PERIOD_MS);
    // }
  }
}

// Wypisywanie errorw na ekranie
void err_code_to_label(uint8_t err_code){
    switch (err_code){
    case SUCCESS:
        sprintf(buf, "Ukonczono testy!");
        break;
    case ESP_ERROR:
        sprintf(buf, "Brak komunikacji z ESP32");
        break;
    case NCL_CH1_ERROR:
        sprintf(buf, "Brak komunikacji NCL (CH1)");
        break;
    case NCL_CH2_ERROR:
        sprintf(buf, "Brak komunikacji NCL (CH2)");
        break;
    case NCL_CH3_ERROR:
        sprintf(buf, "Brak komunikacji NCL (CH3)");
        break;
    case NCL_CH1_I_ERROR:
        sprintf(buf, "Blad przetwornicy NCL (CH1)");
        break;
    case NCL_CH2_I_ERROR:
        sprintf(buf, "Blad przetwornicy NCL (CH2)");
        break;
    case NCL_CH3_I_ERROR:
        sprintf(buf, "Blad przetwornicy NCL (CH3)");
        break;
    }
    lv_label_set_text(ui_Label, buf);
}