
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;
TinyGPS gps;
SoftwareSerial ssGps(3, 4);
String datastring="";
String datastring1="";
char databuf[100];
uint8_t dataoutgoing[100];
char gps_lon[20]={"\0"}; 
char gps_lat[20]={"\0"};
float flat, flon;
void setup()
{
        // Initialize the USB-Serial and GPS software serial connections.
        Serial.begin(9600);
        ssGps.begin(9600);
       
  while (!Serial) ; // Wait for serial port to be available.
  {
          Serial.println("USB-Serial initialized.");
          ssGps.println("GPS initialized.");
          if (!rf95.init())
          {
            Serial.println("RF not initialized.");
          }
          else
          {
            Serial.println("RF initialized.");
            //choosing frequency band
            rf95.setModeTx();
            rf95.setFrequency (915.0);
          }
        }
}

void loop()
{
      bool newData = false;
      // For one second we parse GPS data and report some key values.
      for (unsigned long start = millis(); millis() - start < 1000;)
      {
        while (ssGps.available())
        {
          char c = ssGps.read();
          //Serial.print(c);
          if (gps.encode(c))
          newData = true;
        }
      }
     
      // Get the GPS data.
      if (newData)
      {
        float flat, flon;
        unsigned long age;
        gps.f_get_position(&flat, &flon, &age);
        if (Serial.available())
        {
          Serial.print("LAT=");
          Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
          Serial.print(" | LON=");
          Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
          Serial.print(" | SAT=");
          Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
          Serial.print(" | PREC=");
          Serial.println(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
        }

        flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6;         
        flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6;
        // Once the GPS fixed,send the data to the server.
        datastring +=dtostrf(flat, 0, 6, gps_lat);
        datastring1 +=dtostrf(flon, 0, 6, gps_lon);

        strcpy(gps_lat,gps_lon);
        strcpy((char *)dataoutgoing,gps_lat);
        // Send the data to server
        rf95.send(dataoutgoing, sizeof(dataoutgoing));
      }
      else
      {
        uint8_t output[5];
        strcpy((char *)output,"2");
        Serial.println(*output);
        rf95.send(output, sizeof(output));
        //Serial.println(*dataoutgoing);
        //rf95.send(dataoutgoing, sizeof(dataoutgoing));
        Serial.println("No valid GPS data received.");
      }
}
