// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Name = Jenish Patel
// Student ID: 1572027
// CMPUT 275, WINTER 2020
// Weekly Assignment: Display and Joystick
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
	Demonstrating cursor movement over the map of Edmonton. You will improve over
  what we do in the next weekly exercise.
*/

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


MCUFRIEND_kbv tft;

#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 320
#define YEG_SIZE 2048

lcd_image_t yegImage = { "yeg-big.lcd", YEG_SIZE, YEG_SIZE };

#define JOY_CENTER   512
#define JOY_DEADZONE 64
#define UPLEFT_MAXTHROTTLE 1000
#define DOWNRIGHT_MAXTHROTTLE 100

#define MAX_SPEED 8
#define MIN_SPEED 2

#define CURSOR_SIZE 9

// the cursor position on the display
int cursorX, cursorY, yegMiddleX, yegMiddleY;

// forward declaration for redrawing the cursor
void reddrawCursor(uint16_t colour);

void setup() {
  init();

  Serial.begin(9600);

	pinMode(JOY_SEL, INPUT_PULLUP);

	//tft.reset();             // hardware reset
  uint16_t ID = tft.readID();    // read ID from display
  Serial.print("ID = 0x");
  Serial.println(ID, HEX);
  if (ID == 0xD3D3) ID = 0x9481; // write-only shield
  
  // must come before SD.begin() ...
  tft.begin(ID);                 // LCD gets ready to work

	Serial.print("Initializing SD card...");
	if (!SD.begin(SD_CS)) {
		Serial.println("failed! Is it inserted properly?");
		while (true) {}
	}
	Serial.println("OK!");

	tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);

  // draws the centre of the Edmonton map, leaving the rightmost 60 columns black
	yegMiddleX = YEG_SIZE/2 - (DISPLAY_WIDTH - 60)/2;
	yegMiddleY = YEG_SIZE/2 - DISPLAY_HEIGHT/2;
	lcd_image_draw(&yegImage, &tft, yegMiddleX, yegMiddleY,
                 0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);

  // initial cursor position is the middle of the screen
  cursorX = (DISPLAY_WIDTH - 60)/2;
  cursorY = DISPLAY_HEIGHT/2;

}

/*
  Description: Draws the previous location at which the cursor was before it moved
  Arguments: prevX: previous X point, prevY: previous Y point 
*/
void reddrawPrevCursor(int prevX, int prevY) {
  lcd_image_draw(&yegImage, &tft, yegMiddleX + prevX - CURSOR_SIZE/2, yegMiddleY + prevY - CURSOR_SIZE/2,
                cursorX - CURSOR_SIZE/2, cursorY - CURSOR_SIZE/2, CURSOR_SIZE, CURSOR_SIZE);
}

/*
  Description: Draws a filled rectangle on the screen
  Arguments: color code of the cursor
*/
void reddrawCursor(uint16_t colour) {
  tft.fillRect(cursorX - CURSOR_SIZE/2, cursorY - CURSOR_SIZE/2,
                CURSOR_SIZE, CURSOR_SIZE, colour);
}

/*
  Description: Depending on how much the joystick is pushed in the X-direction
               the appropriate speed is determined.
  Arguments: xVaL: Y-direction response from joystick
  Return: speed of cursor in  X-direction
*/
int cursorSpeedX(int xVal) {
  int speed;
  if (xVal > DOWNRIGHT_MAXTHROTTLE && xVal < UPLEFT_MAXTHROTTLE) {
    speed = MIN_SPEED;
  } else {
    speed = MAX_SPEED;
  }

  Serial.println(speed);
  return speed;
}

/*
  Description: Depending on how much the joystick is pushed in the Y-direction
               the appropriate speed is determined.
  Arguments: xVaL: Y-direction response from joystick
  Return: speed of cursor in Y-direction
*/
int cursorSpeedY(int yVal) {
  int speed;
  if (yVal > DOWNRIGHT_MAXTHROTTLE && yVal < UPLEFT_MAXTHROTTLE) {
    speed = MIN_SPEED;
  } else {
    speed = MAX_SPEED;
  }

  Serial.println(speed);
  return speed;
}

/*
  Description: Changes the global variable cursorY by specified speed if the joystick is moved in
               in the Y-direction. The constrain function is used to bound the cursor inside the
               map of Edmonton
  Arguments: xVaL: Y-direction response from joystick, speed: rate at which cursor moves in Y-direction
*/
void moveVertical(int yVal, int speed) {
  if (yVal < JOY_CENTER - JOY_DEADZONE) {
    cursorY = constrain(cursorY - speed, CURSOR_SIZE/2, DISPLAY_HEIGHT - CURSOR_SIZE/2); // decrease the y coordinate of the cursor
  }
  else if (yVal > JOY_CENTER + JOY_DEADZONE) {
    cursorY = constrain(cursorY + speed, CURSOR_SIZE/2, DISPLAY_HEIGHT - CURSOR_SIZE/2);
  }
}

/*
  Description: Changes the global variable cursorX by specified speed if the joystick is moved in
               in the X-direction. The constrain function is used to bound the cursor inside the
               map of Edmonton
  Arguments: xVaL: X-direction response from joystick, speed: rate at which cursor moves in X-direction
*/
void moveHorizontal(int xVal, int speed) {
  // remember the x-reading increases as we push left
  //Due to the not inclusive nature of the constraint function the display width was subtracted by 61
  if (xVal > JOY_CENTER + JOY_DEADZONE) {
    cursorX = constrain(cursorX - speed, CURSOR_SIZE/2, (DISPLAY_WIDTH - 61) - CURSOR_SIZE/2); 
  }
  else if (xVal < JOY_CENTER - JOY_DEADZONE) {
    cursorX = constrain(cursorX + speed, CURSOR_SIZE/2, (DISPLAY_WIDTH - 61) - CURSOR_SIZE/2);
  }
}

/*
  Description: Checks if there has been movement in the X-direction

  Returns: False if no movement in X-direction or True otherwise. 
*/
bool checkXMove(int xVal) {
  bool check = false;
  if (xVal > JOY_CENTER + JOY_DEADZONE) {
      check = true;
  }
  else if (xVal < JOY_CENTER - JOY_DEADZONE) {
      check = true;
  }
  return check;
}

/*
  Description: Checks if there has been movement in the Y-direction

  Returns: False if no movement in Y-direction or True otherwise. 
*/
bool checkYMove(int yVal) {
  bool check = false;
  if (yVal > JOY_CENTER + JOY_DEADZONE) {
      check = true;
  }
  else if (yVal < JOY_CENTER - JOY_DEADZONE) {
      check = true;
  }
  return check;
}

/*
  Description: Uses the information from the joy_stick and correlates it to the
               cursor position on the TFT display.
  Arguments: None
*/
void processJoystick() {
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);

  // gets horizontal and vertical speed
  int speedX = cursorSpeedX(xVal);
  int speedY = cursorSpeedY(yVal);

  // sets coordinates for previous x and y points
  int prevX = cursorX;
  int prevY = cursorY;

  // draw a small patch of the Edmonton map at the old cursor position before
  // drawing a red rectangle at the new cursor position
  reddrawPrevCursor(prevX, prevY);

  // now move the cursor
  moveVertical(yVal, speedY);
  moveHorizontal(xVal, speedX);

  // draw a cursor in the new connection
  reddrawCursor(TFT_RED); 

  // checks if the cursor has moved in both horizontal and vertical directions
  bool checkX = checkXMove(xVal);
  bool checkY = checkYMove(yVal);

  // if the cursor is stationary then program stays in a loop until cursor is
  // moved again
  while (checkX == false && checkY == false) {
    int xVal = analogRead(JOY_HORIZ);
    int yVal = analogRead(JOY_VERT);

    checkX = checkXMove(xVal);
    checkY = checkYMove(yVal);

    Serial.println(checkX);
    Serial.println(checkY);
  }

  delay(20);
}

// Driver function
int main() {
	setup();

  while (true) {
    processJoystick();
  }

	Serial.end();
	return 0;
}
