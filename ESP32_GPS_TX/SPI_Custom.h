#ifndef SPI_CUSTOM_H
#define SPI_CUSTOM_H
#include "stdint.h"

class SPI_Custom {
public:
  void init(uint32_t maxClockSpeed);
  uint8_t transferByte(uint8_t inByte);
};

#endif // SPI_CUSTOM_H
