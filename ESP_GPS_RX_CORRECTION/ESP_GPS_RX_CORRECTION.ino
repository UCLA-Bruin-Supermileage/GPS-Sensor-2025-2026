#include "Misc_Control.h"


/*  ------------------ RECEIVER CODE INIT RTCM BEGIN ------------------*/

#define MAX_DATA_TRANSFER 32
#define RTCM_BUFF_SIZE 1000
#define ADDR_SIZE 5

RF24 radio(2, 4); // CE, CSN
const uint8_t address[ADDR_SIZE] = {1, 2, 3, 4, 5};
uint8_t receiveBuff[MAX_DATA_TRANSFER];
uint8_t rtcmBuff[RTCM_BUFF_SIZE];


/*  ------------------ RECEIVER CODE INIT RTCM END ------------------*/

void setup() {
  // change as necessary for GPS module baud rate
  
  serial_begin(115200);
  radio.begin();
  radio.openReadingPipe(0, address);

  // set as necessary
  radio.setPaLevel(RF24_PA_MIN);

  // change data rate as needed
  radio.setDataRate(RF24_2MBPS);

  // indicate that we are receiving
  radio.startListening();
}

void loop() {
  while (radio.available()) {
    radio.read(receiveBuff, MAX_DATA_TRANSFER);

    // push raw data out to uart (serial_write())
    for (int i = 0; i < MAX_DATA_TRANSFER; i++)
      serial_write(receiveBuff[i]);
  }
  
}
