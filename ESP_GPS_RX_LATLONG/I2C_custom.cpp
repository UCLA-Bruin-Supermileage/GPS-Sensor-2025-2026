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

uint8_t I2C_Custom::read() {
  Wire.read();
}

uint8_t I2C_Custom::readI2CReg(uint8_t slaveAddr, uint8_t regAddr) {
  Wire.beginTransmission(slaveAddr);
  Wire.write(regAddr);
  Wire.endTransmission(false);

  // Wire.beginTransmission(slaveAddr);
  Wire.requestFrom(slaveAddr, 1);
  
  uint8_t inByte = Wire.read();
//  this->send_start(slaveAddr);
//  this->write(regAddr);
//  this->send_stop();
//
//  this->reqFrom(regAddr, 1);
//
//  uint8_t inByte = 0;
//  while (this->available()) {
//    inByte = this->read();
//  }

  Wire.endTransmission(true);
  return inByte;
}
uint8_t I2C_Custom::available() {
  return Wire.available();
}
// some way to send the start condition
void I2C_Custom::send_start(uint8_t slaveAddr) {
  Wire.beginTransmission(slaveAddr);
}

void I2C_Custom::reqFrom(uint8_t slaveAddr, uint32_t bytesToRead) {
  Wire.requestFrom(slaveAddr, bytesToRead);
}

// some way to send stop condition
void I2C_Custom::send_stop() {
  Wire.endTransmission(true);
}

void I2C_Custom::send_reset() {
  Wire.endTransmission(false);
}
