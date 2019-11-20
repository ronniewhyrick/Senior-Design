#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <gfxfont.h>

// Font definitions:
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMono18pt7b.h>
#include <Fonts/FreeMono24pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

// Color definitions:
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

// Pin definitions: 
// See SPI Connections for more details (https://adafru.it/d5h)
//
// 3.3V - Voltage output
// GND - GND
// 13 - SCK (SPI clock)
// 11 - MOSI (SPI MOSI)
// 7* - CS (SPI Chip select)
// 6* - RST (Display Reset)
// 5* - D/C (SPI data/command select)
// *Can be changed to any pin

#define TFT_CS         8
#define TFT_RST        7
#define TFT_DC         4

// Interface the display using ANY TWO or THREE PINS
#define TFT_MOSI 11  // Data out
#define TFT_SCLK 13  // Clock out

//Absolute function implementation
#define abs(x) ((x)>0?(x):-(x))

// For the ST7789-based displays, we will use this call
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

//Establish two software serial ports
SoftwareSerial GPSModule(5, 6); // RX, TX
SoftwareSerial Trakker(9, 10); // HC-12 TX Pin, HC-12 RX Pin

int updates;
int failedUpdates;
int pos;
int stringplace = 0;
char tx_lat[20];
char tx_lon[20];
char tx_Hlat[20];
char tx_Hlon[20];
String timeUp;
String nmea[15];
String latfinal;
String lonfinal;
String Hlatfinal;
String Hlonfinal;
String labels[12] {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};
//Backpack unique variables
char rx_init[50];
bool done = false; // Listen check complete (0 = false, 1 = true)

void setup() {
  Serial.begin(9600);
  
  //Initialize software serial for GPS then Trakker
  Trakker.begin(9600);
  GPSModule.begin(9600);

  //Initialize TFT Display settings
  tft.init(240, 320);           
  tft.fillScreen(ST77XX_BLACK);
  startupScreen();
  printMap();
  
  Serial.println("Setup Complete");
}

void loop() {     
  Serial.flush();
  float T_LonInt;
  float T_LatInt;
  float B_LonInt;
  float B_LatInt; 
  float DeltaLon;
  float DeltaLat;
  double DeltaLon1;
  double DeltaLat1;
  float distance;
  double ratio;
  double degree;
  
  //Begin reading GPS module
  while ((GPSModule.available()))
  {
    //Flush GPS buffer and begin reading data
    GPSModule.flush();
    GPSModule.read();
    
    //Begin taking in nmea coordinates
    if (GPSModule.find("$GPRMC,")) {
      String tempMsg = GPSModule.readStringUntil('\n');
      for (int i = 0; i < tempMsg.length(); i++) {
        if (tempMsg.substring(i, i + 1) == ",") {
          nmea[pos] = tempMsg.substring(stringplace, i);
          stringplace = i + 1;
          pos++;
        }
        if (i == tempMsg.length() - 1) {
          nmea[pos] = tempMsg.substring(stringplace, i);
        }
      }updates++;
      
      //Convert nmea longitude and latitude to readable degrees
      nmea[2] = ConvertLat();
      nmea[4] = ConvertLng();
      Serial.print("filtered self GPS coordinates \n");
      
      //Convert string arrays to strings, then character arrays
      latfinal = nmea[2];
      Hlatfinal = nmea[3];
      lonfinal = nmea[4];
      Hlonfinal = nmea[5]; 
      latfinal.toCharArray(tx_lat,20);
      Hlatfinal.toCharArray(tx_Hlat,20);
      lonfinal.toCharArray(tx_lon,20);
      Hlonfinal.toCharArray(tx_Hlon,20);
      
      //Print final GPS values with labels
      for (int i = 2; i < 6; i++) {
        Serial.print(labels[i]);
        Serial.print(nmea[i]);
        Serial.println("");
      }
    }
    else failedUpdates++;
    
    // Reset values and jump out of GPS while loop
    stringplace = 0;
    pos = 0;
    
    Trakker.listen(); //Priority set to trakker clips
  }

  //Begin reading Trakker clips
  while (Trakker.available())
  {
    String rx_init = Trakker.readStringUntil('\n');
//    Serial.print("rx_init: ");
//    Serial.println(rx_init);
    String T_Longitude = rx_init.substring(0,rx_init.indexOf(",")); //prints longitude substring
    T_LonInt = T_Longitude.toFloat();
    int del1 = T_Longitude.length();
    String T_Latitude = rx_init.substring(rx_init.indexOf(",",del1 + 2) + 1,rx_init.length() - 2); //prints longitude substring 
    Serial.println(T_LonInt,8);                                               //might  have to be -4 at the end for \n attached
    T_LatInt = T_Latitude.toFloat();
    Serial.println(T_LatInt,8);
     
     //Example Coordinates
     B_LonInt = 39.190953;
     B_LatInt = -096.584159;
     Serial.println(abs(B_LonInt),8);
//     Serial.println(B_LonInt,8);
//     Serial.println(B_LatInt,8);
     
     //Find magnitude of distance between trakker and backpack
     DeltaLon = abs(B_LonInt) - abs(T_LonInt);
     Serial.println(DeltaLon,8);
     DeltaLat = abs(B_LatInt) - abs(T_LatInt);
     Serial.println(DeltaLat,8);
     distance = sqrt((DeltaLat*DeltaLat) + (DeltaLon*DeltaLon));
     Serial.print("Distance is: ");
     Serial.println(distance,8);
     
    //Degree finder
    if (T_LonInt > B_LonInt) // Trakker is North of Backpack
    {
      if (T_LatInt > B_LatInt) //Trakker is North-East of Backpack
      {
        //First Quadrant
        //x & y are positive
        DeltaLon1 =  100000 * abs(DeltaLon);
        DeltaLat1 =  100000 * abs(DeltaLat);
      }
      else //Tracker is North-West of Backpack
      {
        DeltaLon1 = 100000 * abs(DeltaLon);
        DeltaLat1 = -1 * 100000 * abs(DeltaLat);
      }
    }
    else //Tracker is South of Backpack
    {
      if (T_LatInt > B_LatInt) //Tracker is South-East of Backpack
      {
        //Fourth Quadrant
        //x positive, y negative
        DeltaLon1 = -1 * 100000 * abs(DeltaLon);
        DeltaLat1 = 100000 * abs(DeltaLat);
      }
      else //Tracker is South-West of Backpack
      {
        //Third Quadrant 
        // both negative
        DeltaLon1 = -1 * 100000 * abs(DeltaLon);
        DeltaLat1 = -1 * 100000 * abs(DeltaLat);
      }
    }
     Serial.println(DeltaLat1);
     Serial.println(DeltaLon1);
     ratio = (DeltaLon1)/(DeltaLat1);
     Serial.println(ratio);
     degree = (180 * atan(ratio))/3.14159265359;
     Serial.println(degree);  
  }
}//end of void loop()

//Functions specific for coordinate conversions
String ConvertLat() {
  String posneg = "";
  if (nmea[3] == "S") {
    posneg = "-";
  }
  String latfirst;
  float latsecond;
  for (int i = 0; i < nmea[2].length(); i++) {
    if (nmea[2].substring(i, i + 1) == ".") {
      latfirst = nmea[2].substring(0, i - 2);
      latsecond = nmea[2].substring(i - 2).toFloat();
    }
  }
  latsecond = latsecond / 60;
  String CalcLat = "";

  char charVal[9];
  dtostrf(latsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLat += charVal[i];
  }
  latfirst += CalcLat.substring(1);
  latfirst = posneg += latfirst;
  return latfirst;
}

String ConvertLng() {
  String posneg = "";
  if (nmea[5] == "W") {
    posneg = "-";
  }
  String lngfirst;
  float lngsecond;
  for (int i = 0; i < nmea[4].length(); i++) {
    if (nmea[4].substring(i, i + 1) == ".") {
      lngfirst = nmea[4].substring(0, i - 2);
      //Serial.println(lngfirst);
      lngsecond = nmea[4].substring(i - 2).toFloat();
      //Serial.println(lngsecond);

    }
  }
  lngsecond = lngsecond / 60;
  String CalcLng = "";
  char charVal[9];
  dtostrf(lngsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLng += charVal[i];
  }
  lngfirst += CalcLng.substring(1);
  lngfirst = posneg += lngfirst;
  return lngfirst;
}

//Function calls specific to TFT display
// Printout for the startup display
void startupScreen(){
  tft.setCursor(20, 110);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.setFont(&FreeMonoBold24pt7b);
  tft.setTextWrap(true);
  tft.println("Welcome");
  tft.setCursor(95, 157);
  tft.println("to");
  tft.setCursor(20, 200);
  tft.println("PalPack");
  delay(1000);
}

// Initialize map and prompts on display
void printMap(){
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setFont(&FreeMono9pt7b);
  tft.setCursor(0,0);
  uint16_t radius = 0;

  // Print system and coordinate prompts
  tft.setCursor(0,11);
  tft.println("System Status:");
  tft.setCursor(0,25);
  tft.println("Current coordinates:");

  // Print current system status and coordinates
  tft.setTextColor(ST77XX_WHITE);
  tft.setFont(&FreeMonoBold9pt7b);
  tft.setCursor(155,11);
  tft.println("ON"); //replace with status variable
  tft.setCursor(0,41);
  tft.println("39.1974° N, 96.5847° W"); //replace with coordinates variable
  
  // Draws out sonar circles
  for (int16_t i = 0; i <= 5; i++) {
  //for (int16_t i = 0; i < 12; i++) {
    //void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
      tft.drawCircle(120, 164, radius, ST77XX_GREEN);
      radius = radius + 22;
  }
  
  // Print user origin position on map
  tft.setTextColor(ST77XX_WHITE);
  tft.setFont(&FreeSansBold9pt7b);
  tft.setCursor(114, 170);
  tft.println("X");

  tft.setFont();
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(106, 258);
  tft.println("4.0 m");

//  tft.setCursor(158, 312);
//  tft.setTextColor(ST77XX_RED);
//  tft.setFont(&FreeSansBold9pt7b);
//  tft.println("< V > ^");

  // Print available trakker prompt
  tft.setTextColor(ST77XX_GREEN);
  tft.setFont(&FreeMono9pt7b);
  tft.setCursor(0, 300);
  tft.println("Trakkers available:");
  tft.fillCircle(211,297,3,ST77XX_RED);
  tft.fillCircle(223,297,3,ST77XX_BLUE);
  tft.fillCircle(235,297,3,ST77XX_YELLOW);
  tft.setCursor(0, 315);
  tft.println("Out of bounds:");
  //tft.fillCircle(158,312,3,ST77XX_RED);
  //tft.fillCircle(170,312,3,ST77XX_BLUE);
  //tft.fillCircle(182,312,3,ST77XX_YELLOW);
}
