#include "Misc_Control.h"
#include "I2C_Custom.h"
#include "nmea_translate.h"

// canbus stuff
#include "driver/twai.h"

// can pins
#define CAN_TX_PIN 16
#define CAN_RX_PIN 17

// message object for twai
twai_message_t message;

void twai_sendDouble(twai_message_t& msg, uint16_t id, double value) {
  msg.identifier = id;
  memcpy(msg.data, &value, sizeof(value));
  twai_transmit(&msg, pdMS_TO_TICKS(1000));
}

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

#define NMEA_DATA_SIZE 2048
char nmeaData[NMEA_DATA_SIZE];
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

RF24 radio(2, 4);  // CE, CSN
const uint8_t address[ADDR_SIZE] = { 1, 2, 3, 4, 5 };
uint8_t receiveBuff[MAX_DATA_TRANSFER];
uint8_t rtcmBuff[RTCM_BUFF_SIZE];

/*  ------------------ RECEIVER CODE INIT RTCM END ------------------*/

void setup() {
  /*  for latitude and longitude reader */
  i2c.begin();

  // now create the tasks
  xTaskCreatePinnedToCore(
    Task1code, /* Task function. */
    "Task1",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task1,    /* Task handle to keep track of created task */
    0);        /* pin task to core 0 */
  delay_c(500);


  xTaskCreatePinnedToCore(
    Task2code, /* Task function. */
    "Task2",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task2,    /* Task handle to keep track of created task */
    1);        /* pin task to core 1 */
  delay_c(500);
}

void Task1code(void* pvParameters) {
  /*  for receiver */
  serial_begin(115200);
  radio.begin();

  radio.enableDynamicPayloads();
  
  radio.openReadingPipe(0, address);

  // set as necessary
  radio.setPALevel(RF24_PA_MIN);

  // change data rate as needed
  radio.setDataRate(RF24_2MBPS);

  // indicate that we are receiving
  radio.startListening();
  /*  for receiver end */

  while (1) {
    while (radio.available()) {
      uint8_t size = radio.getPayloadSize();

      radio.read(receiveBuff, size);

      // push raw data out to uart (serial_write())
      for (int i = 0; i < size; i++)
        serial_write(receiveBuff[i]);
    }

    // idk AI says i need this
    vTaskDelay(1);
  }
}

void Task2code(void* pvParameters) {
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

  while (1) {
    /*  check for nmea data if it is ready */
    GPS_MSB = i2c.readI2CReg(SLAVE_ADDR, DATA_SIZE_MSB_ADDR);

    GPS_LSB = i2c.readI2CReg(SLAVE_ADDR, DATA_SIZE_LSB_ADDR);

    // get our total number of bytes
    availableBytes = static_cast<uint16_t>(GPS_MSB) << 8 | static_cast<uint16_t>(GPS_LSB);

    // once we know how much is available, read it into our data buff
    if (availableBytes > 0 && availableBytes < NMEA_DATA_SIZE - 1) {
      for (int i = 0; i < availableBytes; i++) {
        nmeaData[i] = i2c.readI2CReg(SLAVE_ADDR, READ_DATA_ADDR);
      }

      nmeaData[availableBytes] = '\0';


      myLocation = translateGNRMC(nmeaData);

      message.data_length_code = 8;

      // send latitude
      twai_sendDouble(message, 0x123, myLocation.latitude);

      twai_sendDouble(message, 0x124, myLocation.longitude);

      // finally, let us print what we get
      // serial_println(myLocation.latitude);
      // serial_println(myLocation.longitude);
    }

    // delay for neo m8p to fill internal buffers again with data
    vTaskDelay(1500);
  }
}

void loop() {
}
