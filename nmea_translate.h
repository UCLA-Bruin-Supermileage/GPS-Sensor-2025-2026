#ifndef NMEA_TRANSLATE_H
#define NMEA_TRANSLATE_H

/*
    ---------------------------------------------------------------
    this is a simple decoder script for NMEA sentences
    it converts many and confusing NMEA formats output by
    the GPS into human readable latitude and longitude coordinates
    specifically, this translated using the GNRMC format
    and that's really all that's needed. I have made it like this
    so that we can add other functions potentially
    ---------------------------------------------------------------
*/

typedef struct {
    double latitude;
    double longitude;
}  Location;      
Location translateGNRMC(unsigned char* NMEA_sentence);
#endif // NMEA_TRANSLATE_H