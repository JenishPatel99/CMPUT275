// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Name = Jenish Patel
// Student ID: 1572027
// CMPUT 275, WINTER 2020
// Weekly Assignment: Display and Pointer
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SD.h>
#include <TouchScreen.h>

#define SD_CS 10

// physical dimensions of the tft display (# of pixels)
#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 320

#define TFT_RED 0xF800
#define TFT_WHITE 0xFFFF

// touch screen pins, obtained from the documentaion
#define YP A3 // must be an analog pin, use "An" notation!
#define XM A2 // must be an analog pin, use "An" notation!
#define YM 9  // can be a digital pin
#define XP 8  // can be a digital pin

// dimensions of the part allocated to the map display
#define MAP_DISP_WIDTH (DISPLAY_WIDTH - 60)
#define MAP_DISP_HEIGHT DISPLAY_HEIGHT

#define REST_START_BLOCK 4000000
#define NUM_RESTAURANTS 1066

// calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 100
#define TS_MINY 120
#define TS_MAXX 940
#define TS_MAXY 920

int PEN_SIZE = 4;
const int BOX_SIZE = 40;

// thresholds to determine if there was a touch
#define MINPRESSURE   10
#define MAXPRESSURE 1000

MCUFRIEND_kbv tft;

// a multimeter reading says there are 300 ohms of resistance across the plate,
// so initialize with this to get more accurate readings
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// different than SD
Sd2Card card;

// Global Variables
// Stores recent slow and fast run times
int slowTotalTime = 0;
int fastTotalTime = 0;

// Stores accumulated slow and fast run times
int slowCounter = 0;
int fastCounter = 0;

// Stores number of times retrieval function is called
int countSlowRetrieval = 0;
int countFastRetrieval = 0;

struct restaurant {
  int32_t lat;
  int32_t lon;
  uint8_t rating; // from 0 to 10
  char name[55];
};

// pre-declaration of functions
void initial_setup();
void recent_slow_run();
void slow_run_avg();
void recent_fast_run();
void fast_run_avg();

/*
  Description: Sets up communication with Arduino and completes SD card initialization
*/
void setup() {
  init();
  Serial.begin(9600);

  // tft display initialization
  uint16_t ID = tft.readID();
  tft.begin(ID);

  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1);

  initial_setup();

  // SD card initialization for raw reads
  Serial.print("Initializing SPI communication for raw reads...");
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println("failed! Is the card inserted properly?");
    while (true) {}
  }
  else {
    Serial.println("OK!");
  }
}

/*
  Description: Sets up user interface when first booted
*/
void initial_setup() {
  tft.drawRect(420,0,60,320,TFT_RED);
  tft.drawFastHLine(420,160,60,TFT_RED);

  tft.drawChar(445,27,'S',TFT_WHITE,TFT_BLACK,2);
  tft.drawChar(445,54,'L',TFT_WHITE,TFT_BLACK,2);
  tft.drawChar(445,81,'O',TFT_WHITE,TFT_BLACK,2);
  tft.drawChar(445,108,'W',TFT_WHITE,TFT_BLACK,2);

  tft.drawChar(445,189,'F',TFT_WHITE,TFT_BLACK,2);
  tft.drawChar(445,216,'A',TFT_WHITE,TFT_BLACK,2);
  tft.drawChar(445,243,'S',TFT_WHITE,TFT_BLACK,2);
  tft.drawChar(445,270,'T',TFT_WHITE,TFT_BLACK,2);

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  recent_slow_run();
  slow_run_avg();
  recent_fast_run();
  fast_run_avg();
}

/*
  Description: Fetches each block everytime function is called and gets one restaurant's
  data which is indicated by restIndex and loads it into a struct pointed to by restPtr
*/
void getRestaurant(int restIndex, restaurant* restPtr) {
  uint32_t blockNum = REST_START_BLOCK + restIndex/8;
  restaurant restBlock[8];

  // fetch a block (512B) of restaurants data including
  // data about the restautant indexed by "restIndex"
  while (!card.readBlock(blockNum, (uint8_t*) restBlock)) {
    Serial.println("Read block failed, trying again.");
  }

  *restPtr = restBlock[restIndex % 8];
}

/*
  Description: Fetches each block expect if the two consecutive calls request restaurants into same 
  block then the latter call does not re-read the same block function from SD card and get a
  restaurant data. Apart from the small adjustment the function acts the same as getRestaurant
*/
void getRestaurantFast(int restIndex, restaurant* restPtr) {
  uint32_t blockNum = REST_START_BLOCK + restIndex/8;
  restaurant restBlock[8];

  // Instead of reading block everytime the function is called.
  // The next block is only read when all 8 restaurants data are extracted
  // from one read of block
  if (restIndex % 8 == 0) {
    while (!card.readBlock(blockNum, (uint8_t*) restBlock)) {
    Serial.println("Read block failed, trying again.");
    }
  }

  *restPtr = restBlock[restIndex % 8];
}

/*
  Description: Calls getRestaurant function 1066 time Calculates run of time of 
  slow retrieval method
*/
void slow() {
  restaurant rest;

  // Measures run time by getting the difference of loop end time and loop start time
  // Calls slow retrieval method to get data of 1066 restaurants from SD cards
  int startMillis = millis();
  for (int i = 0; i < 1066; ++i) {
    getRestaurant(i, &rest);
  }
  int endMillis = millis();

  slowTotalTime = (endMillis - startMillis);
  slowCounter += slowTotalTime;

  recent_slow_run();
  slow_run_avg();
  recent_fast_run();
  fast_run_avg();
}

/*
  Description: Calls getRestaurant function 1066 time Calculates run of time of 
  fast retrieval method
*/
void fast() {
  restaurant rest;

  // Measures run time by getting the difference of loop end time and loop start time
  // Calls fast retrieval method to get data of 1066 restaurants from SD cards
  int startMillis = millis();
  for (int i = 0; i < 1066; ++i) {
    getRestaurantFast(i, &rest);
  }
  int endMillis = millis();

  fastTotalTime = (endMillis - startMillis);
  fastCounter += fastTotalTime;

  recent_fast_run();
  fast_run_avg();
  recent_slow_run();
  slow_run_avg();
}

/*
  Description: Displays the most recent run time of getRestaurant
*/
void recent_slow_run() {
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0,0);
  tft.print("RECENT SLOW RUN:");
  tft.setCursor(0,20);
  if (slowTotalTime == 0) {
    tft.print("Not yet Run");
  } else {
    tft.print(slowTotalTime);
    tft.print(" ms");
  }
}

/*
  Description: Calculate and displays the average run time of getRestaurant
*/
void slow_run_avg() {
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0,60);
  tft.print("SLOW RUN AVG:");
  tft.setCursor(0,80);
  if (slowTotalTime == 0) {
    tft.print("Not yet Run");
  } else {
    int x = slowCounter / countSlowRetrieval;
    tft.print(x);
    tft.print(" ms");
  }
}

/*
  Description: Displays the most recent run time of getRestaurantFast
*/
void recent_fast_run() {
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0,120);
  tft.print("RECENT FAST RUN:");
  tft.setCursor(0,140);
  if (fastTotalTime == 0) {
    tft.print("Not yet Run");
  } else {
    tft.print(fastTotalTime);
    tft.print(" ms");
  }
}

/*
  Description: Calculate and displays the average run time of getRestaurantFast
*/
void fast_run_avg() {
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0,180);
  tft.print("FAST RUN AVG:");
  tft.setCursor(0,200);
  if (fastTotalTime == 0) {
    tft.print("Not yet Run");
  } else {
    int x = fastCounter / countFastRetrieval;
    tft.print(x);
    tft.print(" ms");
  }
}

/*
  Descrption: Manages touch inputs on touchscreen and calss appropiate functions
*/
void loop() {
  TSPoint touch = ts.getPoint(); // (x, y, pressure)
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);

  tft.setRotation(1);
  if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) {
    int ptx = map(touch.y, TS_MINX, TS_MAXX, 0, tft.width());
    int pty = map(touch.x, TS_MINY, TS_MAXY, 0, tft.height());
    
    // On the side pixel calculations were done to align and display
    // text more
    if (ptx < 80) {
      if (pty > 160) {
        tft.fillRect(0,20,200,20,TFT_BLACK);
        tft.fillRect(0,80,200,20,TFT_BLACK);
        countSlowRetrieval += 1;
        slow();

      } else if (pty < 160) {
        tft.fillRect(0,140,200,20,TFT_BLACK);
        tft.fillRect(0,200,200,20,TFT_BLACK);
        countFastRetrieval += 1;
        fast();
      }
      } 
  }
}

// Driver function
int main() {
  setup();
  
  while (true) {
     loop();
     delay(5);
  }
 
  Serial.end();

  return 0;
}