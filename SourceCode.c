/*
	Name:			TT_SourceCode
	Description:	Senior Design II Trail Trakker Source Code.
					The following code takes GPS data and determines
					relative distance from the backpack. 
	Started: 		10/10/2019
*/
#include <SoftwareSerial.h>
#include <TinyGPS.h>
/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   9600-baud serial GPS device hooked up on pins 3(rx) and 4(tx).
*/
#include <SPI.h>
#include <RH_RF95.h>

// Singleton instance of the radio driver
RH_RF95 rf95;
TinyGPS gps;
SoftwareSerial ss(3, 4);
String datastring="";
String datastring1="";
char databuf[100];
uint8_t dataoutgoing[100];
char gps_lon[20]={"\0"};  
char gps_lat[20]={"\0"}; 

void setup()
{
  Serial.begin(9600);
  ss.begin(9600);
    if (!rf95.init())
    Serial.println("init failed");
    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    ss.print("Simple TinyGPS library v. "); ss.println(TinyGPS::library_version());
    Serial.println();
}
 //Get the GPS data
  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    ss.print("LAT=");
    ss.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    ss.print(" LON=");
    ss.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    ss.print(" SAT=");
    ss.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    ss.print(" PREC=");
    ss.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
    flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6;          
    flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6; 
    // Once the GPS fixed,send the data to the server.
    datastring +=dtostrf(flat, 0, 6, gps_lat); 
    datastring1 +=dtostrf(flon, 0, 6, gps_lon);
    ss.println(strcat(strcat(gps_lon,","),gps_lat));
    strcpy(gps_lat,gps_lon);
    ss.println(gps_lat); //Print gps_lon and gps_lat
    strcpy((char *)dataoutgoing,gps_lat); 