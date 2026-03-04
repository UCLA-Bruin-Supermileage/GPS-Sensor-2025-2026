#include "SPI_custom.h"

// include whatever must be included here for personal mcu SDK or library
#include <SPI.h>

void SPI_Custom::init(uint32_t maxClockSpeed) {
  // initialize spi comms
  SPI.begin();

  // initialize with arguments IF NEEDED
  // SPI.beginTransaction(SPISettings(maxClockSpeed, MSBFIRST, SPI_MODE0));
}

uint8_t SPI_Custom::transferByte(uint8_t inByte) {
  return SPI.transfer(inByte);
}
