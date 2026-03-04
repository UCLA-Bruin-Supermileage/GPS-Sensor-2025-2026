#include "I2C_Custom.h"
#include "Misc_Control.h"


/*  ----------------- TRANSMITTER CODE INIT -----------------*/
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(2, 4); // CE, CSN
const uint8_t address[5] = {1, 2, 3, 4, 5};

/*  ----------------- TRANSMITTER CODE INIT END -----------------*/


#define MAX_DATA_TRANSFER 32
uint8_t sendBuff[MAX_DATA_TRANSFER];

// to count how much data we have received so far
int dataCounter = 0;

void setup() {
  serial_begin(115200);

  /*  ----------------- TRANSMITTER SETUP BEGIN -----------------*/
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);

  // change data rate as needed
  radio.setDataRate(RF24_2MBPS);

  // indicate transmission
  radio.stopListening();
}

void loop() {
  if (serial_available()) {
    sendBuff[dataCounter] = serial_read(); 
    dataCounter++;

    if (dataCounter == (MAX_DATA_TRANSFER - 1)) {
      // buffer has been populated, transmit data
      radio.write(sendBuff, MAX_DATA_TRANSFER);
      dataCounter = 0;
    }
    
  } 
}
