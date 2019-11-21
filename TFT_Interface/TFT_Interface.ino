// This code is designed for display on 2.0" TFT 240x320 pixel screen.
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <gfxfont.h>
#include <SoftwareSerial.h>

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

// For the ST7789-based displays, we will use this call
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
SoftwareSerial Backpack(6,5);

float BLat = -096.584159;
float BLon =  39.190953;
float TLat = -096.582849;
float TLon = 39.188239;
String BLatS, BLonS;
String TLatS, TLonS;
String userLoc, trakLoc, lastLoc;
int i = 0;

void setup(void) {
  tft.init(240, 320);           
  tft.fillScreen(ST77XX_BLACK);
  Backpack.begin(9600);
  Serial.begin(9600);
  
  //startupScreen();
  printMap();
}

void loop() {
  while(i < 5){
      BLat -= 0.00002;
      BLon += 0.00001;
      TLat += 0.00002;
      TLon -= 0.00001;
      BLatS = String(BLat,5);
      BLonS = String(BLon,5);
      TLatS = String(TLat,5);
      TLonS = String(TLon,5);
      userLoc = BLatS + "N, " + BLonS + "W";
      trakLoc = TLatS + "N, " + TLonS + "W";
      Serial.print("userLoc: ");
      Serial.println(userLoc);
      Serial.print("trakLoc: ");
      Serial.println(trakLoc);
      updateCoordinates(userLoc); //Update user's coordinate position
      i++;
      delay(2500);
  }
}

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
  //tft.setFont(&FreeMonoBold9pt7b);
  tft.setCursor(155,11);
  tft.println("ON"); //replace with status variable
  //tft.setCursor(0,41);
  //tft.println("-96.584183,N,39.190964,W"); //replace with coordinates variable
  
  // Draws out sonar circles
  for (int16_t i = 0; i <= 5; i++) {
  //for (int16_t i = 0; i < 12; i++) {
    //void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
      tft.drawCircle(120, 164, radius, ST77XX_GREEN);
      radius = radius + 22;
  }
  // Prints ring distance
  tft.setFont();
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(106, 258);
  tft.println("4.0 m");

  // Print user origin position on map
  tft.setTextColor(ST77XX_WHITE);
  tft.setFont(&FreeSansBold9pt7b);
  tft.setCursor(114, 176);
  tft.setCursor(114, 170);
  tft.println("X");

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
}

String updateCoordinates(String userLoc){
  tft.setCursor(0,41);
  //tft.setFont(&FreeMonoBold9pt7b);
  tft.setTextColor(ST77XX_BLACK);
  tft.println(lastLoc); //replace with coordinates variable
  tft.setCursor(0,41);
  tft.setTextColor(ST77XX_WHITE);
  tft.println(userLoc); //replace with coordinates variable
  lastLoc = userLoc;
  //Serial.print("lastLoc: ");
  //Serial.println(lastLoc);
  return lastLoc;
}

#define abs(x) ((x)>0?(x):-(x))
//float DeltaLatF, DeltaLonF;
//double DeltaLatD, DeltaLonD;
//double ratio, degree;

void updateTrakkers(float BLat, float BLon, float TLat, float TLon){
  float DeltaLatF = abs(BLat) - abs(TLat);
  float DeltaLonF = abs(BLon) - abs(TLon);
  float distance = sqrt((DeltaLatF*DeltaLatF) + (DeltaLonF*DeltaLonF));
  
  if (TLon > BLon) { // Tracker is North of Backpack
    if (TLat > BLat) {//Tracker is North-East of Backpack
      //First Quadrant
      // x postive, y positive
      double DeltaLatD =  100000 * abs(DeltaLatF);
      double DeltaLonD =  100000 * abs(DeltaLonF);
    }
    else { //Tracker is North-West of Backpack
      //Second Quadrant
      //x negative, y postive
      DeltaLatD = -1 * 100000 * abs(DeltaLatF);
      DeltaLonD = 100000 * abs(DeltaLonF);
    }
  }
  else { //Tracker is South of Backpack
    if (TLat > BLat) { //Tracker is South-East of Backpack
      //Fourth Quadrant
      //x positive, y negative
      DeltaLatD = 100000 * abs(DeltaLatF);
      DeltaLonD = -1 * 100000 * abs(DeltaLonF);
    }
    else { //Tracker is South-West of Backpack
      //Third Quadrant 
      // x negative, y negative
      DeltaLatD = -1 * 100000 * abs(DeltaLatF);
      DeltaLonD = -1 * 100000 * abs(DeltaLonF);
    }
  }
  double ratio = (DeltaLonD)/(DeltaLatD);
  double degree = (180 * atan(ratio))/3.14159265359;

  //tft.fillCircle(158,312,3,ST77XX_RED);
  //tft.fillCircle(170,312,3,ST77XX_BLUE);
  //tft.fillCircle(182,312,3,ST77XX_YELLOW);
}

