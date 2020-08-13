#ifndef _A1PART2_H_
#define _A1PART2_H_

#define SD_CS 10
#define JOY_VERT  A9 // should connect A9 to pin VRx
#define JOY_HORIZ A8 // should connect A8 to pin VRy
#define JOY_SEL   53

#include <Arduino.h>

// core graphics library (written by Adafruit)
#include <Adafruit_GFX.h>

// Hardware-specific graphics library for MCU Friend 3.5" TFT LCD shield
#include <MCUFRIEND_kbv.h>

// LCD and SD card will communicate using the Serial Peripheral Interface (SPI)
// e.g., SPI is used to display images stored on the SD card
#include <SPI.h>

// needed for reading/writing to SD card
#include <SD.h>

#include "lcd_image.h"

#include <TouchScreen.h>

MCUFRIEND_kbv tft;

#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 320
#define YEG_SIZE 2048

lcd_image_t yegImage = { "yeg-big.lcd", YEG_SIZE, YEG_SIZE };

Sd2Card card;

// touch screen pins, obtained from the documentaion
#define YP A3 // must be an analog pin, use "An" notation!
#define XM A2 // must be an analog pin, use "An" notation!
#define YM 9  // can be a digital pin
#define XP 8  // can be a digital pin


#define JOY_CENTER   512
#define JOY_DEADZONE 64
#define UPLEFT_MAXTHROTTLE 1000
#define DOWNRIGHT_MAXTHROTTLE 100

#define TFT_RED 0xF800
#define TFT_WHITE 0xFFFF
#define TFT_GREEN 0x07E0
#define TFT_BLUE    0x001F

#define MAX_SPEED 15
#define MIN_SPEED 5

#define CURSOR_SIZE 9

#define REST_START_BLOCK 4000000
#define NUM_RESTAURANTS 1066

#define MAP_WIDTH 2048
#define MAP_HEIGHT 2048
#define LAT_NORTH 5361858l // check here if we get weir x/y vals
#define LAT_SOUTH 5340953l
#define LON_WEST -11368652l
#define LON_EAST -11333496l

// the cursor position on the display
int cursorX, cursorY, yegCoordX, yegCoordY;
bool modeCheck = false;

#define NUM_LINES 21

struct restaurant {
  int32_t lat;
  int32_t lon;
  uint8_t rating; // from 0 to 10
  char name[55];
};


struct RestDist{
  uint16_t index; //index of restuarant from 0 to NUMRESTAURANTS-1
  uint16_t dist; //manhattan distance from current cursor position 
};


int PEN_SIZE = 4;
const int BOX_SIZE = 40;

// thresholds to determine if there was a touch
#define MINPRESSURE   10
#define MAXPRESSURE 1000
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 100
#define TS_MINY 120
#define TS_MAXX 940
#define TS_MAXY 920

// Global Variables to take care of option control
int ratingCounter = 1;
// 1 = QSORT, 2 = ISORT, 3 = BOTH
int sortMethod = 1;
int scrollCounter = 0;
int pageNumber = 0;
int highlightedString = 0;
bool upMode = false;

#endif