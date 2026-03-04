#ifndef I2C_CUSTOM_H
#define I2C_CUSTOM_H
#include "stdint.h"

class I2C_Custom {
public:
  void begin();
  void write(uint8_t inByte);
  void send_start(uint8_t slaveAddr);
  void send_stop();
  void send_reset();
};

#endif // I2C_CUSTOM_H
