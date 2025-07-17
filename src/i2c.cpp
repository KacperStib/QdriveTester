#include "i2c.h"

// inicjalizacja magistrali
bool I2Cinit(){
  if(Wire.begin(I2C_SLAVE_SDA_PIN, I2C_SLAVE_SCL_PIN, 400000) == true)
    return true;
  else 
    return false;
}

// zapis do rejestru
bool I2CwriteREG(uint8_t ADDRESS_I2C, uint8_t REGISTER_I2C){
  Wire.beginTransmission(ADDRESS_I2C);
  Wire.write(REGISTER_I2C);
  if (Wire.endTransmission() == 0)
    return true;
  else
    return false; 
}

// zapis wartosci do rejestru
bool I2CwriteVAL(uint8_t ADDRESS_I2C, uint8_t REGISTER_I2C, uint8_t VALUE_I2C){
  Wire.beginTransmission(ADDRESS_I2C);
  Wire.write(REGISTER_I2C);
  Wire.write(VALUE_I2C);
  if (Wire.endTransmission() == 0)
    return true;
  else
    return false; 
}

// odczytaj dane do bufora
void I2Cread(uint8_t ADDRESS_I2C, uint8_t *buf, uint8_t bytesToReceive) {
  Wire.requestFrom(ADDRESS_I2C, bytesToReceive);
  Wire.readBytes(buf, bytesToReceive);
}