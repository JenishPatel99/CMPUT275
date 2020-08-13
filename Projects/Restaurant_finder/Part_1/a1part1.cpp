// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Name = Jenish Patel, Mattheas Jamieson
// Student ID: 1572027, 1601689
// CMPUT 275, WINTER 2020
// Major Assignment: Restaurant Finder Part 1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "a1part1.h"

// Create one global instance of RestDist array of structs
// and one instance of restuarant 
RestDist rest_dist[NUM_RESTAURANTS];
restaurant rest1;

// global variables for use in getRestaurantFast()
bool first_call = true;
uint32_t prev_block;
restaurant restBlock[8];


int highlightedString;


int32_t x_to_lon(int16_t x) {
  return map(x, 0, MAP_WIDTH, LON_WEST, LON_EAST);
}


int32_t y_to_lat(int16_t y) {
  return map(y, 0, MAP_HEIGHT, LAT_NORTH, LAT_SOUTH);
}


int16_t lon_to_x(int32_t lon) {
  return map(lon, LON_WEST, LON_EAST, 0, MAP_WIDTH);
}


int16_t lat_to_y(int32_t lat) {
  return map(lat, LAT_NORTH, LAT_SOUTH, 0, MAP_HEIGHT);
}


/*
  Description:
    Changes the current restuarant stored in pointer by reading the desired
    restuarant from the SD card with the desired index with optimazation that
    makes it faster than the function implemented in class.

  Arguments:
    restIndex (int): the index of the restaurant you're reading from SD card
    restPtr (pointer): pointer to an instance of the restuarant struct
    
  Returns:
    Nothing
*/
void getRestaurantFast(int restIndex, restaurant * restPtr) {
  // calculate the block containing this restaurant
  uint32_t blockNum = REST_START_BLOCK + restIndex / 8;

  // established on the first ever function call and never called again
  if (first_call == true) {
    first_call = false;
    prev_block = blockNum;
    while (!card.readBlock(blockNum, (uint8_t * ) restBlock)) {
      Serial.println("Read block failed, trying again.");
    }
    * restPtr = restBlock[restIndex % 8];

  }

  // if we are in the same block as the previous call
  if (blockNum - prev_block == 0) {
    * restPtr = restBlock[restIndex % 8];

    // if we are in a different block as the previous call
  } else {
    while (!card.readBlock(blockNum, (uint8_t * ) restBlock)) {
      Serial.println("Read block failed, trying again.");
    }
    * restPtr = restBlock[restIndex % 8];

  }
  prev_block = blockNum;

} // end of void getRestaurantFast()


/*
  Description:
    Calculates a manhattan distance from the current cursor positon given the latitude
    and longtitude of a restaurant. 

  Arguments:
    lon (uint32_t): longtitude of given restaurant you're looking at.
    lat (uint32_t): latitude of given restaurant you're looking at.

  Returns:
    manhattan distance (int): manhattan distance from current cursor position.
*/
int manhattanDistance(uint32_t lon, uint32_t lat) {
  return (abs(lon_to_x(lon) - yegCoordX - cursorX) + abs(lat_to_y(lat) - yegCoordY - cursorY));

}// end of manhattanDistance()


/*
  Description:
    Swaps two struct instances of RestDist struct beside each other in an array.

  Arguments:
    *array (RestDist): Pointer to a array of RestDist structs
    j (int): Swap struct at this index with previous index.

  Returns:
    Nothing
*/
void swap(RestDist *array, int j) {
  // create placeholder instance to store intermediate restuarant
  RestDist intermediate;

  // swap data from array[j] into intermediate
  intermediate.index = array[j].index;
  intermediate.dist = array[j].dist;

  // reassign array[j] data to equal array[j-1] data
  array[j].index = array[j - 1].index;
  array[j].dist = array[j - 1].dist;

  // reassign array[j-1] to equal intermediate( prev array[j])
  array[j - 1].index = intermediate.index;
  array[j - 1].dist = intermediate.dist;

} // end of void swap()


/*
  Description:
    Insertion sort for sorting an array of RestDist structs based on closest manhattan
    distance to farthest from current cursor position. 

  Arguments:
    size (int): Size of array to be sorted.
    *array (RestDist): Array of RestDist structs to be sorted.
  Returns:
    Nothing
*/
void isort(int size, RestDist *array) { 
  int j, i = 1;

  while (i < size) {
    j = i;

    while (j > 0 && (array[j - 1].dist > array[j].dist)) {
      swap(array, j);
      j--;
    }
    i++;

  }

} // end of void isort();


// forward declaration for redrawing the cursor
void reddrawCursor(uint16_t colour);
void mode0();


/*
  Description:
    Setup function that initializes Serial Communication and reading from SD card. 
    Also sets the intial cursor position and draws center of YEG map.

  Arguments:
    None

  Returns:
    Nothing

*/
void setup() {
  init();

  Serial.begin(9600);

  pinMode(JOY_SEL, INPUT_PULLUP);

  //tft.reset();             // hardware reset
  uint16_t ID = tft.readID(); // read ID from display
  Serial.print("ID = 0x");
  Serial.println(ID, HEX);
  if (ID == 0xD3D3) ID = 0x9481; // write-only shield

  // must come before SD.begin() ...
  tft.begin(ID); // LCD gets ready to work

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed! Is it inserted properly?");
    while (true) {}
  }
  Serial.println("OK!");

  Serial.print("Initializing SPI communication for raw reads...");
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println("failed! Is the card inserted properly?");
    while (true) {}
  } else {
    Serial.println("OK!");
  }

  tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);

  // draws the centre of the Edmonton map, leaving the rightmost 60 columns black
  yegCoordX = YEG_SIZE / 2 - (DISPLAY_WIDTH - 60) / 2;
  yegCoordY = YEG_SIZE / 2 - DISPLAY_HEIGHT / 2;
  lcd_image_draw( & yegImage, & tft, yegCoordX, yegCoordY,
    0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);

  // initial cursor position is the middle of the screen
  cursorX = (DISPLAY_WIDTH - 60) / 2;
  cursorY = DISPLAY_HEIGHT / 2;

}// end of void setup()


/*
  Description:
    Draws the previous location at which the cursor was before it moved

  Arguments: 
    prevX (int): previous X point 
    prevY (int): previous Y point 

  Returns:
    Nothing

*/
void reddrawPrevCursor(int prevX, int prevY) {
  lcd_image_draw( & yegImage, & tft, yegCoordX + prevX - CURSOR_SIZE / 2, yegCoordY + prevY - CURSOR_SIZE / 2,
    cursorX - CURSOR_SIZE / 2, cursorY - CURSOR_SIZE / 2, CURSOR_SIZE, CURSOR_SIZE);

}// end of void reddrawPrevCursor()


/*
  Description: 
    Draws a filled rectangle on the screen

  Arguments: 
    colour (uint16_t): Color code of the cursor

  Returns:
    Nothing
*/
void reddrawCursor(uint16_t colour) {
  tft.fillRect(cursorX - CURSOR_SIZE / 2, cursorY - CURSOR_SIZE / 2,
    CURSOR_SIZE, CURSOR_SIZE, colour);
}// end of void reddrawCursor()


/*
  Description: 
    Depending on how much the joystick is pushed in the X-direction
    the appropriate speed is determined.

  Arguments: 
    xVaL (int): X-direction response from joystick
  Returns: 
    speed of cursor in  X-direction
*/
int cursorSpeedX(int xVal) {
  int speed;
  if (xVal > DOWNRIGHT_MAXTHROTTLE && xVal < UPLEFT_MAXTHROTTLE) {
    speed = MIN_SPEED;
  } else {
    speed = MAX_SPEED;
  }

  return speed;

}// end of int cursorSpeedX()


/*
  Description: 
    Depending on how much the joystick is pushed in the Y-direction
    the appropriate speed is determined.

  Arguments: 
    yVaL (int): Y-direction response from joystick

  Return: 
    speed of cursor in Y-direction
*/
int cursorSpeedY(int yVal) {
  int speed;
  if (yVal > DOWNRIGHT_MAXTHROTTLE && yVal < UPLEFT_MAXTHROTTLE) {
    speed = MIN_SPEED;
  } else {
    speed = MAX_SPEED;
  }

  return speed;

}// end of int cursorSpeedY()


/*
  Description:
    Redraws a patch of the YEG map on the tft display with new yegCoordX and 
    yegCoordY. Centers the cursor back in the middle of the screen.

  Arguments:
    None

  Returns:
    Nothing
*/
void drawNewMap() {
  lcd_image_draw( & yegImage, & tft, yegCoordX, yegCoordY,
    0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);
  cursorX = (DISPLAY_WIDTH - 60) / 2;
  cursorY = DISPLAY_HEIGHT / 2;

}// end of void drawNewMap()


/*
  Description: 
    Changes the global variable cursorY by specified speed if the joystick is moved in
    in the Y-direction. The constrain function is used to bound the cursor inside the
    map of Edmonton

  Arguments: 
    xVaL (int): Y-direction response from joystick
    speed (int): rate at which cursor moves in Y-direction

  Returns:
    Nothing
*/
void moveVertical(int yVal, int speed) {
  if (yVal > JOY_CENTER + JOY_DEADZONE) {
    
    // goes down
    if ((cursorY >= (DISPLAY_HEIGHT - CURSOR_SIZE / 2)) && (yegCoordY < YEG_SIZE - DISPLAY_HEIGHT)) {
      // checks if yeg y coord is less than last portion of map
      yegCoordY = constrain(yegCoordY + DISPLAY_HEIGHT, 0, YEG_SIZE - DISPLAY_HEIGHT);
      drawNewMap();

    } else {
      cursorY = constrain(cursorY + speed, CURSOR_SIZE / 2, DISPLAY_HEIGHT - CURSOR_SIZE / 2);

    }

  }
  if (yVal < JOY_CENTER - JOY_DEADZONE) {
    
    // goes up
    if ((cursorY <= CURSOR_SIZE / 2) && (yegCoordY >= 50)) {
      yegCoordY = constrain(yegCoordY - DISPLAY_HEIGHT, 0, YEG_SIZE - DISPLAY_HEIGHT);
      drawNewMap();

    } else {
      cursorY = constrain(cursorY - speed, CURSOR_SIZE / 2, DISPLAY_HEIGHT - CURSOR_SIZE / 2);

    }

  }
  delay(2);

}// end of void moveVertical()


/*
  Description: 
    Changes the global variable cursorX by specified speed if the joystick is moved in
    in the X-direction. The constrain function is used to bound the cursor inside the
    map of Edmonton.

  Arguments: 
    xVaL (int): X-direction response from joystick
    speed (int): rate at which cursor moves in X-direction

  Returns:
    Nothing
*/
void moveHorizontal(int xVal, int speed) {
  // remember the x-reading increases as we push left
  // Due to the not inclusive nature of the constraint function the display width was subtracted by 61
  if (xVal > JOY_CENTER + JOY_DEADZONE) {
    
    // goes left
    if ((cursorX <= CURSOR_SIZE / 2) && (yegCoordX >= 50)) {
      
      // checks if yeg y coord is less than last portion of map
      yegCoordX = constrain(yegCoordX - (DISPLAY_WIDTH - 60), 0, YEG_SIZE);
      drawNewMap();

    } else {
      cursorX = constrain(cursorX - speed, CURSOR_SIZE / 2, (DISPLAY_WIDTH - 61) - CURSOR_SIZE / 2);

    }

  }

  if (xVal < JOY_CENTER - JOY_DEADZONE) {
    
    // goes right
    if ((cursorX >= (DISPLAY_WIDTH - 60 - CURSOR_SIZE)) && (yegCoordX <= YEG_SIZE - DISPLAY_WIDTH - 60)) {
      
      // checks if yeg y coord is less than last portion of map
      yegCoordX = constrain(yegCoordX + (DISPLAY_WIDTH - 60), 0, YEG_SIZE - (DISPLAY_WIDTH - 60));
      drawNewMap();

    } else {
      cursorX = constrain(cursorX + speed, CURSOR_SIZE / 2, (DISPLAY_WIDTH - 61) - CURSOR_SIZE / 2);

    }

  }
  delay(2);

}// end of void moveHorizontal()


/*
  Description: 
    Checks if there has been movement in the X-direction

  Arguments:
    xVaL (int): X-direction response from joystick

  Returns: 
    False if no movement in X-direction or True otherwise. 
*/
bool checkXMove(int xVal) {
  bool check = false;
  if (xVal > JOY_CENTER + JOY_DEADZONE) {
    check = true;

  } else if (xVal < JOY_CENTER - JOY_DEADZONE) {
    check = true;

  }
  return check;

}// end of bool checkXMove()


/*
  Description: 
    Checks if there has been movement in the Y-direction

  Arguments:
    yVaL (int): Y-direction response from joystick

  Returns: 
    False if no movement in Y-direction or True otherwise. 
*/
bool checkYMove(int yVal) {
  bool check = false;
  if (yVal > JOY_CENTER + JOY_DEADZONE) {
    check = true;

  } else if (yVal < JOY_CENTER - JOY_DEADZONE) {
    check = true;

  }
  return check;

}// end of bool checkYMove()


/*
  Description:
    This gives the illusion of a scrolling list, changing the text color and the background
    color depending on which restuarant you're hovering over.

  Arguments:
    index (int): default argument value of where to start checking for scrolling.

  Returns:
    Nothing
*/
void displayText(int index = 0) {
  // 15 is the vertical span of a size-2 character
  // (including the one pixel of padding below)

  tft.setCursor(0, 15 * index);
  tft.setTextWrap(false);

  // loads in current restuarant from SD card and stores in rest1
  getRestaurantFast(rest_dist[index].index, & rest1);

  if (index == highlightedString) {
    tft.setTextColor(TFT_BLACK, TFT_WHITE);

  } else {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

  }
  tft.print(rest1.name);
  tft.print("\n");

}// end of void displayText()


/*
  Description:
    Called when initially changing from map to list view. It determines the 21 closest
    restuarants by manhattan distance and then lists them on the screen in ascending order
    from top to bottom. Also "highlights" the first restuarant to give the illusion its 
    being hovered over.

  Arguments:
    None

  Returns:
    Nothing
*/
void displayAllText() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextWrap(false); 

  // read in 1066 restauarnts to store simplfied info in array
  for (int i = 0; i < NUM_RESTAURANTS; ++i) {
    // load in restaurant from SD card
    getRestaurantFast(i, & rest1);

    // store index in simplfied array of restuarants 
    rest_dist[i].index = i;
    // calculate and store manhattan distance for current restaurant position
    rest_dist[i].dist = manhattanDistance(rest1.lon, rest1.lat);
  }

  // sorts restuarants from closest to farthest from current postion
  isort(NUM_RESTAURANTS, rest_dist);

  for (int i = 0; i < 21; i++) {
    tft.setCursor(0, i * 15);
    // load in restaurant from SD card
    getRestaurantFast(rest_dist[i].index, & rest1);

    if (i == 0) {
      tft.setTextColor(TFT_BLACK, TFT_WHITE);

    } else {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);

    }
    tft.print(rest1.name);
    tft.print("\n");

  }
  tft.print("\n");

  // initially "highlights" the top restaurant in the list
  tft.setCursor(0, 0);
  getRestaurantFast(rest_dist[0].index, & rest1);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);

}// end of void displayAllText()


/*
  Description:
    When you are in map mode (mode 0) and click the joystick this function runs that displays
    the closest 21 restaurants in a list. And in a while loop it checks for if you scroll up 
    or down the list and if you click the joystick again it will switch back to map view, centering
    you over the restuarant if it is within bounds, taking care of boundary cases by constraining
    the position of the cursor position within the YEG map.

  Arguments:
    None

  Returns:
    Nothing
*/
void mode1() {
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  highlightedString = 0;
  displayAllText();

  while (modeCheck == true) {
    int yVal = analogRead(JOY_VERT);
    int buttonVal = digitalRead(JOY_SEL);

    // goes down
    if (yVal > (JOY_CENTER + JOY_DEADZONE)) {
      // draw the old highlighted string normally
      highlightedString = constrain(highlightedString + 1, 0, 20);
      displayText(highlightedString - 1);

      // highlight the new string
      displayText(highlightedString);

    }

    // goes up
    if (yVal < (JOY_CENTER - JOY_DEADZONE)) {
      // draw the old highlighted string normally
      highlightedString = constrain(highlightedString - 1, 0, 20);
      displayText(highlightedString + 1);

      // highlight the new string
      displayText(highlightedString);

    }

    if ((buttonVal == 0) && (modeCheck == true)) {
      for (int i = 0; i < NUM_RESTAURANTS; ++i) {
        // load in restaurant from SD card
        getRestaurantFast(i, & rest1);
        
        // store index in simplfied array of restuarants 
        rest_dist[i].index = i;
        // calculate and store manhattan distance for current restaurant position
        rest_dist[i].dist = manhattanDistance(rest1.lon, rest1.lat);

      }

      isort(NUM_RESTAURANTS, rest_dist);

      for (int i = 0; i < 21; i++) {
        // load in restaurant from SD card
        getRestaurantFast(rest_dist[i].index, & rest1);

        if (i == highlightedString) {
          modeCheck = false;
          yegCoordX = lon_to_x(rest1.lon) - (DISPLAY_WIDTH - 60) / 2;
          yegCoordY = lat_to_y(rest1.lat) - (DISPLAY_HEIGHT / 2);

          cursorX = (DISPLAY_WIDTH - 60) / 2;
          cursorY = DISPLAY_HEIGHT / 2;

          if (yegCoordX < (DISPLAY_WIDTH - 60) / 2) {
            yegCoordX = 0;
            cursorX = constrain(lon_to_x(rest1.lon), CURSOR_SIZE/2, (DISPLAY_WIDTH - 61) - CURSOR_SIZE/2);

          } else if (yegCoordX > (YEG_SIZE - (DISPLAY_WIDTH - 60) / 2)) {
            yegCoordX = constrain(YEG_SIZE - (DISPLAY_WIDTH - 60), 0, YEG_SIZE - (DISPLAY_WIDTH - 60));
            cursorX = constrain(YEG_SIZE - lon_to_x(rest1.lon), CURSOR_SIZE/2, (DISPLAY_WIDTH - 61) - CURSOR_SIZE/2);

          }

          if (yegCoordY < (DISPLAY_HEIGHT) / 2) {
            yegCoordY = 0;
            cursorY = constrain(lat_to_y(rest1.lat), CURSOR_SIZE/2, DISPLAY_HEIGHT - CURSOR_SIZE/2);

          } else if (yegCoordY > (YEG_SIZE - DISPLAY_HEIGHT / 2)) {
            yegCoordY = constrain(yegCoordY + DISPLAY_HEIGHT, 0, YEG_SIZE - DISPLAY_HEIGHT);
            cursorY = constrain(YEG_SIZE - lat_to_y(rest1.lat), CURSOR_SIZE/2, DISPLAY_HEIGHT - CURSOR_SIZE/2);

          }

          tft.fillScreen(TFT_BLACK);
          lcd_image_draw( & yegImage, & tft, yegCoordX, yegCoordY,
            0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);
          mode0();
          break;

        }

      }

    }
    delay(20);

  }

}// end of void mode1


/*
  Description:
    Checks if you have tapped the screen inside the map and not in the black column on the right
    side of screen. 

  Arguments:
    none

  Returns:
    True of False.
*/
bool checkTouch() {
  TSPoint touch = ts.getPoint(); // (x, y, pressure)
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);

  if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) {
    int ptx = map(touch.y, TS_MINX, TS_MAXX, 0, tft.width());
    // On the side pixel calculations were done to align and display
    // text more
    if (ptx > 60) {
      return true;

    } else {
      return false;

    }

  }
  return false;

}// end of bool checkTouch


void displayRestaurants() {
  // read in 1066 restauarnts to store simplfied info in array
  for (int i = 0; i < NUM_RESTAURANTS; ++i) {
    // load in restaurant from SD card
    getRestaurantFast(i, & rest1);

    // store index in simplfied array of restuarants 
    rest_dist[i].index = i;
    // calculate and store manhattan distance for current restaurant position
    rest_dist[i].dist = manhattanDistance(rest1.lon, rest1.lat);

  }

  // sorts restuarants from closest to farthest from current postion
  isort(NUM_RESTAURANTS, rest_dist);

  int PEN_SIZE = 2;
  for (int i = 0; i < 21; i++) {
    // load in restaurant from SD card
    getRestaurantFast(rest_dist[i].index, & rest1);

    // Draw dot on map where restaurant is located
    tft.fillCircle(lon_to_x(rest1.lon) - (yegCoordX), lat_to_y(rest1.lat) - (yegCoordY), PEN_SIZE, TFT_BLACK);

  }

}// end of void displayRestaurants()


/*
  Description:
    Uses the information from the joy_stick and correlates it to the
    cursor position on the TFT display.

  Arguments: 
    None
  
  Returns:
    Nothing
*/
void mode0() {
  modeCheck = false;
  highlightedString = 0;
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);
  int buttonVal = digitalRead(JOY_SEL);

  // gets horizontal and vertical speed
  int speedX = cursorSpeedX(xVal);
  int speedY = cursorSpeedY(yVal);

  // sets coordinates for previous x and y points
  int prevX = cursorX;
  int prevY = cursorY;

  // checks if screen is being touched
  if (checkTouch()) {
    displayRestaurants();
  }

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

  // checks while you're in map view if you have pressed the joystick and want to enter
  // list view
  if ((buttonVal == 0) && (modeCheck == false)) {
    modeCheck = true;
    mode1();
  }

  // if the cursor is stationary then program stays in a loop until cursor is
  // moved again
  while (checkX == false && checkY == false) {
    // continually in while loop checks if screen is touched
    if (checkTouch()) {
      displayRestaurants();

    }

    int xVal = analogRead(JOY_HORIZ);
    int yVal = analogRead(JOY_VERT);

    // checks if the cursor has moved in both horizontal and vertical directions
    checkX = checkXMove(xVal);
    checkY = checkYMove(yVal);

    int buttonVal = digitalRead(JOY_SEL);

    // checks continually in loop if button is pressed
    if ((buttonVal == 0) && (modeCheck == false)) {
      modeCheck = true;
      mode1();

    }

  }
  delay(20);

}// end of void mode0()


int main() {
  setup();

  while (true) {
    mode0();
  }

  Serial.end();
  return 0;
  
}