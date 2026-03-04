#include "I2C_custom.h"

// include whatever is necessary here
#include <Wire.h>

// must start as master
void I2C_Custom::begin() {
  Wire.begin();
}

void I2C_Custom::write(uint8_t inByte) {
  Wire.write(inByte);
}

// some way to send the start condition
void I2C_Custom::send_start(uint8_t slaveAddr) {
  Wire.beginTransmission(slaveAddr);
}

// some way to send stop condition
void I2C_Custom::send_stop() {
  Wire.endTransmission(true);
}

void I2C_Custom::send_reset() {
  Wire.endTransmission(false);
}
