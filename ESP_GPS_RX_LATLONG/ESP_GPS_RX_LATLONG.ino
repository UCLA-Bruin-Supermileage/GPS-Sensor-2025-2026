#include "Misc_Control.h"
#include "I2C_Custom.h"
#include "nmea_translate.h"

// canbus twai
#include "driver/twai.h"

#define CAN_BOARD_NAME 
// aritra smv enum
enum class DAQMessage {
  Longitude,
  Latitude,
  Altitude
};

#define DAQ_BOARD 7

#define DAQ_LONGITUDE 0
#define DAQ_LATITUDE 1
#define DAQ_ALTITUDE 2

// can pins
#define CAN_TX_PIN 16
#define CAN_RX_PIN 17

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

// message object for twai

twai_message_t message;

/* this will send a double */
void twai_sendDouble(twai_message_t& msg, uint16_t id, double value) {
  msg.identifier = id;
  memcpy(msg.data, &value, sizeof(value));
  twai_transmit(&msg, pdMS_TO_TICKS(1000));
}

uint16_t daq_createID(uint16_t boardName, uint16_t datatype) {
  uint16_t id = ((boardName & 0x0F) << 7) + datatype;
  return id;
}

void setup() {
  i2c.begin();
  // serial_begin(115200);
  Serial.begin(115200);
  delay(2000);
  // some config
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)CAN_TX_PIN, (gpio_num_t)CAN_RX_PIN, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
    Serial.println("Driver installed");
  } else {
    Serial.println("Failed to install driver");
    return;
  }

  if (twai_start() == ESP_OK) {
    Serial.println("Driver started");
  } else {
    Serial.println("Failed to start driver");
    return;
  }

  message.extd = 0;  // 0 for 11-bit ID, 1 for 29-bit
  message.rtr = 0;   // not a remote transmission request
  message.ss = 0;    // not single-shot (allow retries)
  message.self = 0;  // not a self-reception
}

void loop() {

  /*  --------------- CHANGE THIS SHIT LATER WITH THE RIGHT REGISTERS ---------------*/
  /*  check for nmea data if it is ready */
  // GPS_MSB = i2c.readI2CReg(SLAVE_ADDR, DATA_SIZE_MSB_ADDR);

  // GPS_LSB = i2c.readI2CReg(SLAVE_ADDR, DATA_SIZE_LSB_ADDR);

  // // get our total number of bytes
  // availableBytes = static_cast<uint16_t>(GPS_MSB) << 8 | static_cast<uint16_t>(GPS_LSB);
  // serial_print("Available Bytes: ");
  // serial_println(availableBytes);

  uint16_t daqLat_id = daq_createID(DAQ_BOARD, DAQ_LATITUDE);
  uint16_t daqLong_id = daq_createID(DAQ_BOARD, DAQ_LONGITUDE);

  Serial.println(daqLat_id);
  Serial.println(daqLong_id);

  message.data_length_code = 8;
  // once we know how much is available, read it into our data buff
  if (availableBytes > 0) {
    for (int i = 0; i < availableBytes; i++) {
      nmeaData[i] = i2c.readI2CReg(SLAVE_ADDR, READ_DATA_ADDR);
      // serial_print(nmeaData[i]);
    }

    myLocation = translateGNRMC(nmeaData);

    // send latitude
    twai_sendDouble(message, daqLat_id, myLocation.latitude);

    twai_sendDouble(message, daqLong_id, myLocation.longitude);

    // finally, let us print what we get
    serial_println(myLocation.latitude);
    serial_println(myLocation.longitude);
  }

  /* debugging with aritra */
  // send latitude
  twai_sendDouble(message, daqLat_id, 1234.56);

  twai_sendDouble(message, daqLong_id, 78.91011);
  // delay for neo m8p to fill internal buffers again wi+th data
  delay(1000);
}
