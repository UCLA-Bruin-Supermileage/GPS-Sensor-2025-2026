## Base Station

Steps to launch:
Read through the GPS base station text document in order to create a license to access base station feeds

Launch RTKNAVI from the RTK_Executables folder

Enter your information, as described in the GPS base station document

## Using GPS with EPS32

The ESP32_GPS_TX file should be flashed to one ESP32 that is intended to transmit RTCM data 

The other RX folders should be flashed to another ESP32 board, ideally on both cores. Eventually, the RX folders will be combined into one that will automatically flash the right data to both cores

The receiving ESP32 TX pin and GND pin should be connected to the GPS, and the GPS SDA and SCL pins should be connected to the receiving ESP32, in order to transmit NMEA sentences.
