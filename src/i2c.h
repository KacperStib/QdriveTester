#include <Wire.h>

#define I2C_SLAVE_SDA_PIN 32
#define I2C_SLAVE_SCL_PIN 33

bool I2Cinit();
bool I2CwriteREG(uint8_t ADDRESS_I2C, uint8_t REGISTER_I2C);
bool I2CwriteVAL(uint8_t ADDRESS_I2C, uint8_t REGISTER_I2C, uint8_t VALUE_I2C);
void I2Cread(uint8_t ADDRESS_I2C, uint8_t *buf, uint8_t bytesToReceive);