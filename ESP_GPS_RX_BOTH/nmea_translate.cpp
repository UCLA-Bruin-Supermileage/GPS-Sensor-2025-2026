#include "nmea_translate.h"
#include "stdio.h"          // just for testing
#include "stdbool.h"        // for convenience, booleans
#include "string.h"         // strtok, strcmp
#include "stdlib.h"         // strtod

Location translateGNRMC(char* NMEA_sentence) {
    Location location;

    char delimiters[] = ",\n";

    char* nmeaWord;
    bool foundGPRMC = false;
    int i = 0;

    double degrees = 0.0;
    double minutes = 0.0;

    bool checkLatDirection = false;
    double Latitude = 0.0;

    bool checkLongDirection = false;
    double Longitude = 0.0;

    // the government wants you to think strtok doesn't
    // work for unsigned char
    // but, as always, they lie to you
    nmeaWord = strtok(NMEA_sentence, delimiters);

    while (nmeaWord != NULL) {
        if (foundGPRMC) {
            i++;
            if (i == 3) {
            	char degreesLat[3];
				strncpy(degreesLat, nmeaWord, 2);
				degrees = strtod(degreesLat, NULL);

				char minutesLat[11];
				strcpy(minutesLat, nmeaWord + 2);
				minutes = strtod(minutesLat, NULL);

				Latitude = degrees + (minutes / 60.0);

				nmeaWord = strtok(NULL, delimiters);

				if (nmeaWord[0] == 'S') {
					Latitude *= -1;
				}

				continue;
            }  else if (i == 5) {
            	char degreesLong[4];
				strncpy(degreesLong, nmeaWord, 3);
				degrees = strtod(degreesLong, NULL);

				char minutesLong[11];
				strcpy(minutesLong, nmeaWord + 3);
				minutes = strtod(minutesLong, NULL);

				Longitude = degrees + (minutes / 60.0);

				nmeaWord = strtok(NULL, delimiters);
				if (nmeaWord[0] == 'W') {
					Longitude *= -1;
				}
				break;
            }
        }

        if (strcmp(nmeaWord, "$GNRMC") == 0) {
            foundGPRMC = true;
        }

        nmeaWord = strtok(NULL, delimiters);
    }

    location.latitude = Latitude;
    location.longitude = Longitude;
    return location;
}
