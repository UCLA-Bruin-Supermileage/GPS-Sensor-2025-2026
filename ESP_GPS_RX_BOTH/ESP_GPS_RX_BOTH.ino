#include "Misc_Control.h"
#include "I2C_Custom.h"
#include "nmea_translate.h"

TaskHandle_t Task1;
TaskHandle_t Task2;


/*  ----------------------- LATLONG RX -----------------------*/

// neo m8p slave address
#define SLAVE_ADDR 0x42

// is there data ? address
#define DATA_SIZE_MSB_ADDR 0xFD
#define DATA_SIZE_LSB_ADDR 0xFE

// actual data address
#define READ_DATA_ADDR 0xFF

// where we will store our data to be decoded
char nmeaData[1024];
uint8_t GPS_MSB = 0;
uint8_t GPS_LSB = 0;
uint16_t availableBytes = 0;

// struct that will store our lat/longitude
Location myLocation;

I2C_Custom i2c;

/*  ----------------------- LATLONG RX END -----------------------*/



/*  ------------------ RECEIVER CODE INIT RTCM BEGIN ------------------*/

#include <nRF24L01.h>
#include <RF24.h>

#define MAX_DATA_TRANSFER 32
#define RTCM_BUFF_SIZE 1000
#define ADDR_SIZE 5

RF24 radio(2, 4); // CE, CSN
const uint8_t address[ADDR_SIZE] = {1, 2, 3, 4, 5};
uint8_t receiveBuff[MAX_DATA_TRANSFER];
uint8_t rtcmBuff[RTCM_BUFF_SIZE];

/*  ------------------ RECEIVER CODE INIT RTCM END ------------------*/

void setup() {
  serial_begin(115200);
  /*  for receiver */

  radio.begin();
  radio.openReadingPipe(0, address);

  // set as necessary
  radio.setPALevel(RF24_PA_MIN);

  // change data rate as needed
  radio.setDataRate(RF24_2MBPS);

  // indicate that we are receiving
  radio.startListening();
  /*  for receiver end */


  /*  for latitude and longitude reader */
  i2c.begin();

  // now create the tasks
  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
  delay_c(500);


  xTaskCreatePinnedToCore(
    Task2code,   /* Task function. */
    "Task2",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task2,      /* Task handle to keep track of created task */
    1);          /* pin task to core 1 */
  delay_c(500);
}

void Task1code(void* pvParameters) {
  while (1) {
    while (radio.available()) {
      radio.read(receiveBuff, MAX_DATA_TRANSFER);

      // push raw data out to uart (serial_write())
      for (int i = 0; i < MAX_DATA_TRANSFER; i++)
        serial_write(receiveBuff[i]);
    }
  }
}

void Task2code(void* pvParameters) {
  while (1) {
    /*  check for nmea data if it is ready */
    GPS_MSB = i2c.readI2CReg(SLAVE_ADDR, DATA_SIZE_MSB_ADDR);

    GPS_LSB = i2c.readI2CReg(SLAVE_ADDR, DATA_SIZE_LSB_ADDR);

    // get our total number of bytes
    availableBytes = static_cast<uint16_t> (GPS_MSB) << 8 | static_cast<uint16_t> (GPS_LSB);

    // once we know how much is available, read it into our data buff
    if (availableBytes > 0) {
      for (int i = 0; i < availableBytes; i++) {
        nmeaData[i] = i2c.readI2CReg(SLAVE_ADDR, READ_DATA_ADDR);
      }
    }

    myLocation = translateGNRMC(nmeaData);

    // finally, let us print what we get
    serial_println(myLocation.latitude);
    serial_println(myLocation.longitude);
    // delay for neo m8p to fill internal buffers again with data
    delay_c(2000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
