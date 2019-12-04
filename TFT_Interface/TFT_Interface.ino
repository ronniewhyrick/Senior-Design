// This code is designed for display on 2.0" TFT 240x320 pixel screen.
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <gfxfont.h>
#include <math.h>
#include <SoftwareSerial.h>

// Font definitions:
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMono24pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
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
// 8* - CS (SPI Chip select)
// 7* - RST (Display Reset)
// 4* - D/C (SPI data/command select)
// *Can be changed to any pin

#define TFT_CS         8
#define TFT_RST        7
#define TFT_DC         4

// Interface the display using ANY TWO or THREE PINS
#define TFT_MOSI 11  // Data out
#define TFT_SCLK 13  // Clock out

// For the ST7789-based displays, we will use this call
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
SoftwareSerial Backpack(3,2);

String userLoc, trakLoc, lastLoc;
float angle, mag;
const double rLen = 0.02; // Radius of map in km
const uint8_t pixCount = 110; // Amount of pixels within the radius
const uint8_t xOff = 120; // x origin of map
const uint8_t yOff = 164; // y origin of map
float bLat = -096.58418;
float bLon =  39.19096;
float tLat = -096.58282;
float tLon = 39.18823;
unsigned long x, y, xLast, yLast;
uint8_t trakNum;
int count = 0; // Number of trakkers available
int out = 0; // Number of trakkers out of bounds
int i = 0;
int iterations = 20;

void setup(void) {
  tft.init(240, 320);           
  tft.fillScreen(ST77XX_BLACK);
  Serial.begin(9600);
  Backpack.begin(9600);
  tft.setRotation(2);
  //startupScreen();
  initMap();
}

void loop() {
  while(i < iterations){
    bLat -= 0.00002;
    bLon += 0.00001;
    tLat += 0.00002;
    tLon -= 0.00001;
    updateCoordinates(bLat, bLon); // Update user's coordinate position
    updateTrakkers(0, tLat, tLon); // Gather and print trakker's coordinate positions
    tLat -= 85;
    updateTrakkers(1, tLat, tLon);
//    Serial.print(F("userLoc: "));
//    Serial.println(userLoc);
//    Serial.print(F("trakLoc: "));
//    Serial.println(trakLoc);
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
  tft.println(F("Welcome"));
  tft.setCursor(95, 157);
  tft.println(F("to"));
  tft.setCursor(20, 200);
  tft.println(F("PalPack"));
  delay(1000);
}

// Initialize map and prompts on display
void initMap(){
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setFont(&FreeMono9pt7b);
  tft.setCursor(0,0);
  int8_t radius = 0;

  // Print system and coordinate prompts
  tft.setCursor(0,11);
  tft.println(F("System Status:"));
  tft.setCursor(0,25);
  tft.println(F("Current coordinates:"));

  // Print current system status and coordinates
  tft.setTextColor(ST77XX_WHITE);
  //tft.setFont(&FreeMonoBold9pt7b);
  tft.setCursor(155,11);
  tft.println(F("ON"));
  
  // Draws out sonar circles
  for (int8_t i = 0; i <= 5; i++) {
  //for (int16_t i = 0; i < 12; i++) {
    //void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
      tft.drawCircle(xOff, yOff, radius, ST77XX_GREEN);
      radius = radius + 22;
  }
  // Prints ring distance
  tft.setFont();
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(106, 258);
  tft.println(F("3.0 m"));

  // Print user origin position on map
  tft.setTextColor(ST77XX_WHITE);
  tft.setFont(&FreeSansBold9pt7b);
  tft.setCursor(114, 176);
  tft.setCursor(114, 170);
  tft.println(F("X"));

  // Print available trakker prompt
  tft.setTextColor(ST77XX_GREEN);
  tft.setFont(&FreeMono9pt7b);
  tft.setCursor(0, 300);
  tft.println(F("Trakkers available:"));
  tft.setCursor(0, 315);
  tft.println(F("Out of bounds:"));
}

void updateCoordinates(float bLat, float bLon){
  userLoc = String(bLon,5) + "N, " + String(bLat,5) + "W";
  tft.setCursor(0,41);
  tft.setTextColor(ST77XX_BLACK);
  tft.println(lastLoc);
  tft.setCursor(0,41);
  tft.setTextColor(ST77XX_WHITE);
  tft.println(userLoc);
  //Serial.print(F("curLoc: "));
  //Serial.println(userLoc);
  lastLoc = userLoc;
  //Serial.print("lastLoc: ");
  //Serial.println(lastLoc);
}

void updateTrakkers(int trakNum, float x, float y){
  int8_t color = ST77XX_BLACK;
  int8_t r = 3; // Radius of trakker point
  // Coordinates of cursor position - intialized at "Trakkers available:"
  int8_t xPlot = 211; // x coordinate
  int8_t yPlot = 297; // y coordinate
  int8_t inc = 12; // Cursor position increment size

  tft.fillCircle(xLast,yLast,r,color);
  printMap();

  // Sets pixel color based on trakker number
  switch (trakNum) {
    case 0:
      color = ST77XX_RED;
      break;
    case 1:
      color = ST77XX_BLUE;
      break;
    case 2:
      color = ST77XX_YELLOW;
      break;
    default:
      color = ST77XX_BLACK;
      break;
  }
  xPlot += (inc*count);
  tft.fillCircle(xPlot,yPlot,r,color); // Add to available label
  count++;
  // Set cursor at "Out of bounds:"
  xPlot = 186;
  yPlot = 312;

//  int delimLoc = trakLoc.indexOf(',');
//  if(delimLoc >= 0){
//      angle = trakLoc.substring(0,delimLoc).toFloat();
//      mag = trakLoc.substring(delimLoc + 1).toFloat();
//  }
//  // Convert polar coordinates into rectangular
//  x = mag*cos(angle);
//  y = mag *sin(angle);

  if(x >= rLen || y >= rLen){
     // Add to out of bounds label
     xPlot += (inc*out);
     tft.fillCircle(xPlot,yPlot,r,color);
     out++;
  }
  else{
     // Convert rectangular coordinates into pixel
     x = (x/rLen)*pixCount;
     y = (y/rLen)*pixCount;
     xPlot = x + xOff; // Add map origin offset
     yPlot = y + yOff;
     tft.fillCircle(xPlot,yPlot,r,color); // Plot trakker
  }
  xLast = xPlot;
  yLast = yPlot;

//    Serial.print("Trakker ");
//    Serial.print(trakkerNum);
//    Serial.print(" loc: ");
//    Serial.println(trakLoc);
}

void initVals(){
  bLat = -096.58418;
  bLon =  39.19096;
  tLat = -096.58282;
  tLon = 39.18823;
  i = 0;
  out = 0;
}

void printMap(){
  uint16_t radius = 0;
  // Draws out sonar circles
  for (int16_t i = 0; i <= 5; i++) {
      tft.drawCircle(120, 164, radius, ST77XX_GREEN);
      radius = radius + 22;
  }
}

