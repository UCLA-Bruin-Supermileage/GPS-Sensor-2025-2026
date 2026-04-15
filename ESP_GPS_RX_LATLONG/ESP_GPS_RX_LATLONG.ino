#include "Misc_Control.h"
#include "I2C_Custom.h"
#include "nmea_translate.h"

// neo m8p slave address
#define SLAVE_ADDR 0x42

// is there data ? address
#define DATA_SIZE_MSB_ADDR 0xFD
#define DATA_SIZE_LSB_ADDR 0xFE

// actual data address
#define READ_DATA_ADDR 0xFF

// where we will store our data to be decoded
char nmeaData[2048];
uint8_t GPS_MSB = 0;
uint8_t GPS_LSB = 0;
uint16_t availableBytes = 0;

// struct that will store our lat/longitude
Location myLocation;

I2C_Custom i2c;
void setup() {
  i2c.begin();
  serial_begin(115200);
}

void loop() {

  /*  --------------- CHANGE THIS SHIT LATER WITH THE RIGHT REGISTERS ---------------*/
  /*  check for nmea data if it is ready */
  GPS_MSB = i2c.readI2CReg(SLAVE_ADDR, DATA_SIZE_MSB_ADDR);

  GPS_LSB = i2c.readI2CReg(SLAVE_ADDR, DATA_SIZE_LSB_ADDR);

  // get our total number of bytes
  availableBytes = static_cast<uint16_t> (GPS_MSB) << 8 | static_cast<uint16_t> (GPS_LSB);
  serial_print("Available Bytes: ");
  serial_println(availableBytes);
  
  // once we know how much is available, read it into our data buff
  if (availableBytes >= 0) {
    for (int i = 0; i < availableBytes; i++) {
      nmeaData[i] = i2c.readI2CReg(SLAVE_ADDR, READ_DATA_ADDR);
      // serial_print(nmeaData[i]);
    }

    myLocation = translateGNRMC(nmeaData);

    // finally, let us print what we get
    serial_println(myLocation.latitude);
    serial_println(myLocation.longitude);
  }
  
  // delay for neo m8p to fill internal buffers again with data
  delay(200);
}
