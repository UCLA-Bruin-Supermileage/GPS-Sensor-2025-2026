#include "nmea_translate.h"
#include "stdio.h"          // just for testing
#include "stdbool.h"        // for convenience, booleans
#include "string.h"         // strtok, strcmp
#include "stdlib.h"         // strtod

Location translateGNRMC(unsigned char* NMEA_sentence) {
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

int main() {
    Location myLocation;
    char inputSentence[] = "$GPGGA,143909.00,A,5107.0020216,N,11402.3294835,W,1,04,1.0,64.6,M,-26.5,M,,*13\n$GNRMC,204520.00,A,5109.0262239,S,11401.8407338,E,0.004,102.3,130522,0.0,E,D*3B\n$GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68";

    myLocation = translateGNRMC(inputSentence);
    printf("Latitude: %lf\nLongitude %lf", myLocation.latitude, myLocation.longitude);
}