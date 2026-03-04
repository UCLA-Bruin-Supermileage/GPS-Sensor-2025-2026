#ifndef MISC_CONTROL_H
#define MISC_CONTROL_H

// change to mcu specific library
#include <Arduino.h>

/* 
  --------------------------------------------
  this is for things like delay, serial print,
  stuff like that
  --------------------------------------------
*/

inline void delay_c(unsigned long msDelay) {
  delay(msDelay);
}


template <typename T>
inline void serial_print(const T& arg) {
    Serial.print(arg);
}

inline void serial_begin(uint32_t baudRate) {
  Serial.begin(baudRate);
}
template <typename T>
inline void serial_println(const T& arg) {
  Serial.println(arg);
}


#endif // MISC_CONTROL_H
