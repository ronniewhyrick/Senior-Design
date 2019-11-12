/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.
  The 2.0" TFT breakout
    ----> https://www.adafruit.com/product/4311

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/
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
  Serial.begin(9600);
  Serial.print(F("PalPack Test"));
  
  // Init ST7789 320x240
  tft.init(240, 320);           

  Serial.println(F("Initialized"));
  tft.fillScreen(ST77XX_BLACK);

  // tft print function!
  //startupScreen();
  printMap();

  Serial.println("done");
  delay(1000);
}

void loop() {}

// Printout for the startup display
void startupScreen(){
  tft.setCursor(5, 110);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.setTextSize(3);
  tft.setFont(&FreeMono9pt7b);
  tft.setTextWrap(true);
  tft.println("Welcome");
  tft.setCursor(80, 157);
  tft.println("to");
  tft.setCursor(5, 200);
  tft.println("PalPack");
  delay(1000);
}

void printMap(){
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setFont(&FreeMono9pt7b);
  tft.setCursor(0, 0);
  uint16_t radius = 0;

  tft.setCursor(0,11);
  tft.println("System Status:");
  tft.setCursor(0,25);
  tft.println("Current coordinates:");

  // Draws out sonar circles
  for (int16_t i = 0; i < 12; i++) {
    //void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
      tft.drawCircle(120, 160, radius, ST77XX_GREEN);
      radius = radius + 10;
  }
  
  tft.setCursor(0, 285);
  tft.println("Trakkers available:");
  tft.setCursor(205, 285);
  tft.setTextColor(ST77XX_RED);
  tft.println(".");
  tft.setCursor(215, 285);
  tft.setTextColor(ST77XX_BLUE);
  tft.println(".");
  tft.setCursor(225, 285);
  tft.setTextColor(ST77XX_YELLOW);
  tft.println(".");
}

void updateScreen(){
  
}
