// This code is designed for display on 2.0" TFT 240x320 pixel screen.
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
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

#define TFT_CS         7
#define TFT_RST        6
#define TFT_DC         5

// Interface the display using ANY TWO or THREE PINS
#define TFT_MOSI 11  // Data out
#define TFT_SCLK 13  // Clock out

// For the ST7789-based displays, we will use this call
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void setup(void) {
  tft.init(240, 320);           
  //tft.fillScreen(ST77XX_BLACK);
  
  //startupScreen();
  printMap();
}

//void loop() {}

// Printout for the startup display
void startupScreen(){
  tft.setCursor(5, 110);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.setFont(&FreeMono9pt7b);
  tft.setTextWrap(true);
  tft.println("Welcome");
  tft.setCursor(80, 157);
  tft.println("to");
  tft.setCursor(5, 200);
  tft.println("PalPack");
  delay(1000);
}

// Initialize map and prompts on display
void printMap(){
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setFont(&FreeMono9pt7b);
  tft.setCursor(0, 0);
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
  for (int16_t i = 0; i < 12; i++) {
    //void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
      tft.drawCircle(120, 170, radius, ST77XX_GREEN);
      radius = radius + 10;
  }
  
  // Print user origin position on map
  tft.setTextColor(ST77XX_WHITE);
  tft.setFont(&FreeSansBold9pt7b);
  tft.setCursor(114, 176);
  tft.println("X");

  tft.setFont();
  tft.setTextColor(ST77XX_GREEN);
//  tft.setCursor(106, 192);
//  tft.println("1.5mi");
//  tft.setCursor(106, 212);
//  tft.println("1.5mi");
//  tft.setCursor(106, 232);
//  tft.println("1.5mi");
//  tft.setCursor(106, 252);
//  tft.println("1.5mi");
  tft.setCursor(106, 272);
  tft.println("1.5mi");

  // Print available trakker prompt
  tft.setTextColor(ST77XX_GREEN);
  tft.setFont(&FreeMono9pt7b);
  tft.setCursor(0, 315);
  tft.println("Trakkers available:");
  tft.fillCircle(211,312,3,ST77XX_RED);
  tft.fillCircle(223,312,3,ST77XX_BLUE);
  tft.fillCircle(235,312,3,ST77XX_YELLOW);
}

void updateCoordinates(){
  
}

void updateTrakkers(){

}

