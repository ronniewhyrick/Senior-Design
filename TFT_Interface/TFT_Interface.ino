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
SoftwareSerial Backpack(3,2); //RX,TX

String userLoc, lastLoc;
float angle, mag;
const double rLen = 0.5; // Radius of map in km
const uint8_t pixCount = 110; // Amount of pixels within the radius
const uint8_t xOff = 120; // x origin of map
const uint8_t yOff = 164; // y origin of map
float bLat, bLon, tLat, tLon;
int x, y, xLast, yLast;
uint8_t trakNum = 1;
int count = 0; // Number of trakkers available
int out = 0; // Number of trakkers out of bounds
int i = 0;
int loc1, loc2;
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
  while(Backpack.available()){
    String coordinates = Backpack.readString();
    updateCoordinates(coordinates); // Update user's coordinate position
    updateTrakkers(0, tLat, tLon); // Gather and print trakker's coordinate positions
//    Serial.print(F("userLoc: "));
//    Serial.println(userLoc);
//    Serial.print(F("trakLoc: "));
  }//trakNum++;
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
  tft.println(F("3.0 m")); // <---------- abstract this to variable 

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

void updateCoordinates(String coordinates){
  loc1 = coordinates.indexOf(','); // Gather position of first delimeter
  
  // Update backpack user's coordinates:
  bLat =  coordinates.substring(0,loc1).toFloat();
  bLon = splitString(coordinates);
  userLoc = String(bLat,6) + ", " + String(bLon,6); // Parse coordinates to print on TFT screen
  
  // Update trakker coordinates:
  tLat = splitString(coordinates);
  tLon = splitString(coordinates);
  //tLon = -tLon; // Invert tLon to correctly display on TFT axes
  
  tft.setCursor(0,41);
  tft.setTextColor(ST77XX_BLACK);
  tft.println(lastLoc);
  tft.setCursor(0,41);
  tft.setTextColor(ST77XX_WHITE);
  tft.println(userLoc);
  lastLoc = userLoc;
  //Serial.print(F("curLoc: "));
  //Serial.println(userLoc);
  //Serial.print("lastLoc: ");
  //Serial.println(lastLoc);
}

void updateTrakkers(int trakNum, float tLat, float tLon){
  Serial.print("Coordinates: ");
  Serial.print(tLat);
  Serial.print(", ");
  Serial.println(tLon);
  Serial.print("Trakker number: ");
  Serial.println(trakNum);
  uint16_t color = ST77XX_BLACK;
  uint8_t r = 3; // Radius of trakker point
  // Coordinates of cursor position - intialized at "Trakkers available:"
  int xPlot = 211; // x coordinate
  int yPlot = 297; // y coordinate
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

  if(tLat >= rLen || tLon >= rLen){
     // Add to out of bounds label
     xPlot += (inc*out);
     tft.fillCircle(xPlot,yPlot,r,color);
     out++;
     Serial.println("Out of bounds");
  }
  else{
     // Convert rectangular coordinates into pixel
     x = (tLat/rLen)*pixCount;
     y = (tLon/rLen)*pixCount;
     Serial.print("Converted coordinates: ");
     Serial.print(x);
     Serial.print(", ");
     Serial.println(y);
   
     xPlot = x + xOff; // Add map origin offset
     yPlot = y + yOff;
     tft.fillCircle(xPlot,yPlot,r,color); // Plot trakker
     Serial.print("Trakker printed at: ");
     Serial.print(xPlot);
     Serial.print(", ");
     Serial.println(yPlot);
     Serial.print("With color: ");
     Serial.println(color);
  }
  xLast = xPlot;
  yLast = yPlot;

//    Serial.print("Trakker ");
//    Serial.print(trakkerNum);
//    Serial.print(" loc: ");
//    Serial.println(trakLoc);
}

float splitString(String coordinates){
  float str;
  if(loc1 > 0){
      loc2 = coordinates.indexOf(',', loc1+1);
      str = coordinates.substring(loc1+1, loc2).toFloat();
      loc1 = loc2;
  }return str;
}

void printMap(){
  uint16_t radius = 0;
  // Draws out sonar circles
  for (int16_t i = 0; i <= 5; i++) {
      tft.drawCircle(xOff, yOff, radius, ST77XX_GREEN);
      radius = radius + 22;
  }
}

