// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Name = Jenish Patel, Mattheas Jamieson
// Student ID: 1572027, 1601689
// CMPUT 275, WINTER 2020
// Major Assignment: Restaurant Finder Part 1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "a1part2.h"

// Create one global instance of RestDist array of structs
// and one instance of restuarant 
RestDist rest_dist[NUM_RESTAURANTS];
restaurant rest1;

// global variables for use in getRestaurantFast()
bool first_call = true;
uint32_t prev_block;
restaurant restBlock[8];

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

} // end of manhattanDistance()

/*
  Description:
    Swaps two struct instances of RestDist struct beside each other in an array.

  Arguments:
    *array (RestDist): Pointer to a array of RestDist structs
    j (int): Swap struct at this index with previous index.

  Returns:
    Nothing
*/
void swap(RestDist * array, int j) {
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
void iSort(int size, RestDist * array) {
  int j, i = 1;

  while (i < size) {
    j = i;

    while (j > 0 && (array[j - 1].dist > array[j].dist)) {
      swap(array, j);
      j--;
    }
    i++;

  }

} // end of void iSort();

/*
  Description:
    Swaps two struct instances of RestDist struct in the array at the given indices

  Arguments:
    *array (RestDist): Pointer to a array of RestDist structs
    j (int): index of one struct inside the array
    i (int): index of second struct inside the array

  Returns:
    Nothing
*/
void swapQSort(RestDist * array, int j, int i) {
  // create placeholder instance to store intermediate restuarant
  RestDist intermediate;

  // swap data from array[j] into intermediate
  intermediate.index = array[j].index;
  intermediate.dist = array[j].dist;

  // reassign array[j] data to equal array[i] data
  array[j].index = array[i].index;
  array[j].dist = array[i].dist;

  // reassign array[i] to equal intermediate(prev array[j])
  array[i].index = intermediate.index;
  array[i].dist = intermediate.dist;

} // end of void swapQSort()

/*
  Description:
    Given a non-empty array and an index pi this function rearranges
    all elements in the array such that all items that are less than or
    equal to the value stored at index pi is to the left of than index 
    pi and all items that are greater than index pi are to the right of
    index pi.

  Arguments:
    *a (RestDist): Pointer to a array of RestDist structs
    pi (int): index that we are rearranging in relation to
    n (int): length of array we are re-ordering

  Returns:
    New index pi where a[pi] is stored in array
*/
int pivot(int pi, int n, RestDist * a) {
  // set lo and hi values
  int lo = 0;
  int hi = n - 2;

  // swap pivot index to last element in sub array
  swapQSort(a, pi, n - 1);

  while (hi >= lo) {
    if (a[lo].dist <= a[n - 1].dist) {
      lo++;
    } else if (a[hi].dist > a[n - 1].dist) {
      hi--;
    } else {
      swapQSort(a, lo, hi);
    }

  } // end of while loop
  swapQSort(a, lo, n - 1);

  // return location of new pivot
  return lo;

} // end of int pivot()

/*
  Description:
    This is a quick sort algorithim that is used in conjunction with the
    pivot function to recursively sort a array of structs based on their
    manhattan distance from the cursor position. 

  Arguments:
    *a (RestDist): Pointer to a array of RestDist structs
    arr_len (int): length of array we are re-ordering

  Returns:
    Nothing
*/
void qSort(int arr_len, RestDist * rest_dist) {
  if (arr_len <= 1) {
    return;
  }
  // pick pivet index
  int pi = arr_len / 2;

  // find new pivot index
  int new_pi = pivot(pi, arr_len, rest_dist);

  // Two recursive calls to sort first and last half of array
  qSort(new_pi, rest_dist);
  qSort(arr_len - new_pi - 1, rest_dist + new_pi + 1);

} // end of void quick sort()

// forward declaration for redrawing the cursor
void reddrawCursor(uint16_t colour);
void mode0();
void optionsDisplaySetup();

/*
  Description:
    Redraws the text inside the sort. This is called whenever the
    "sort" button is clicked and the text needs to be changed

  Arguments:
    None

  Returns:
    Nothing
*/
void displaySortMethod() {
  if (sortMethod == 1) {
    tft.fillRect(420, 162, 60, 158, TFT_WHITE);
    tft.drawRect(420, 162, 60, 158, TFT_GREEN);

    tft.drawChar(445, 185, 'Q', TFT_BLACK, TFT_WHITE, 2);
    tft.drawChar(445, 208, 'S', TFT_BLACK, TFT_WHITE, 2);
    tft.drawChar(445, 229, 'O', TFT_BLACK, TFT_WHITE, 2);
    tft.drawChar(445, 252, 'R', TFT_BLACK, TFT_WHITE, 2);
    tft.drawChar(445, 273, 'T', TFT_BLACK, TFT_WHITE, 2);
  } else if (sortMethod == 2) {
    tft.fillRect(420, 162, 60, 158, TFT_WHITE);
    tft.drawRect(420, 162, 60, 158, TFT_GREEN);

    tft.drawChar(445, 185, 'I', TFT_BLACK, TFT_WHITE, 2);
    tft.drawChar(445, 208, 'S', TFT_BLACK, TFT_WHITE, 2);
    tft.drawChar(445, 229, 'O', TFT_BLACK, TFT_WHITE, 2);
    tft.drawChar(445, 252, 'R', TFT_BLACK, TFT_WHITE, 2);
    tft.drawChar(445, 273, 'T', TFT_BLACK, TFT_WHITE, 2);
  } else {
    tft.fillRect(420, 162, 60, 158, TFT_WHITE);
    tft.drawRect(420, 162, 60, 158, TFT_GREEN);

    tft.drawChar(445, 192, 'B', TFT_BLACK, TFT_WHITE, 2);
    tft.drawChar(445, 217, 'O', TFT_BLACK, TFT_WHITE, 2);
    tft.drawChar(445, 242, 'T', TFT_BLACK, TFT_WHITE, 2);
    tft.drawChar(445, 267, 'H', TFT_BLACK, TFT_WHITE, 2);
  }

} // end of displaySortMethod()

/*
  Description:
    Redraws both buttons including the white background, the red and
    green border and also draws the rating inside the rating selector
    button. This function also calls displaySortMethod() to update what
    is inside the "sort" button.

  Arguments:
    None

  Returns:
    Nothing
*/
void optionsDisplaySetup() {
  tft.fillRect(420, 0, 60, 158, TFT_WHITE);
  tft.drawRect(420, 0, 60, 158, TFT_RED);

  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);

  tft.setCursor(445, 63);
  tft.print(ratingCounter);

  // Updates what is displayed inside the sort button
  displaySortMethod();

} // end of optionsDisplaySetup()

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
  optionsDisplaySetup();

  // draws the centre of the Edmonton map, leaving the rightmost 60 columns black
  yegCoordX = YEG_SIZE / 2 - (DISPLAY_WIDTH - 60) / 2;
  yegCoordY = YEG_SIZE / 2 - DISPLAY_HEIGHT / 2;
  lcd_image_draw( & yegImage, & tft, yegCoordX, yegCoordY,
    0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);

  // initial cursor position is the middle of the screen
  cursorX = (DISPLAY_WIDTH - 60) / 2;
  cursorY = DISPLAY_HEIGHT / 2;

} // end of void setup()

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

} // end of void reddrawPrevCursor()

/*
  Description: 
    Draws a filled rectangle on the screen which is the cursor

  Arguments: 
    colour (uint16_t): Color code of the cursor

  Returns:
    Nothing
*/
void reddrawCursor(uint16_t colour) {
  tft.fillRect(cursorX - CURSOR_SIZE / 2, cursorY - CURSOR_SIZE / 2,
    CURSOR_SIZE, CURSOR_SIZE, colour);

} // end of void reddrawCursor()

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

} // end of int cursorSpeedX()

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

} // end of int cursorSpeedY()

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

} // end of void drawNewMap()

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

} // end of void moveVertical()

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

} // end of void moveHorizontal()

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

} // end of bool checkXMove()

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

} // end of bool checkYMove()

/*
  Description: 
    Reads in only restaurants from SD card that meet the minumum rating 
    requriement. This function is called before any sort is used. So we
    don't sort restaurants that don't meet the rating requirement. 

  Arguments:
    None.

  Returns: 
    The amount of restuarants we read in that meet the rating requirement. 
*/
int readInRest() {
  // read in 1066 restauarnts to store simplfied info in array
  int length = 0;
  for (int i = 0; i < NUM_RESTAURANTS; ++i) {
    // load in restaurant from SD card
    getRestaurantFast(i, & rest1);

    // store index in simplfied array of restuarants 
    if (max((rest1.rating + 1) / 2, 1) >= ratingCounter) {
      getRestaurantFast(i, & rest1);
      rest_dist[length].index = i;
      // calculate and store manhattan distance for current restaurant position
      rest_dist[length].dist = manhattanDistance(rest1.lon, rest1.lat);
      length++;
    }

  }
  return length;

} // end of readInRest()

/*
  Description: 
    sorts restuarants from closest to farthest from current postion 
    depending on which sort or both were selected. Also outputs to 
    serial monitor how long the sort(s) took. 

  Arguments:
    None

  Returns: 
    Nothing 
*/
void sortMethodChoice() {
  if (sortMethod == 1) {
    // read in from SD/ determine amount of restaurants to sort and start clock
    int length = readInRest();
    int startMillis = millis();

    // call quick sort and end clock
    qSort(length, rest_dist);
    int endMillis = millis();

    // calculate and output how long sort took
    int fastTotalTime = (endMillis - startMillis);
    Serial.print("Quick sort ");
    Serial.print(length);
    Serial.print(" restaurants running time: ");
    Serial.print(fastTotalTime);
    Serial.println(" ms");

  } else if (sortMethod == 2) {
    // read in from SD/ determine amount of restaurants to sort and start clock
    int length = readInRest();
    int startMillis = millis();

    // call insertion sort and end clock
    iSort(length, rest_dist);
    int endMillis = millis();

    // calculate and output how long sort took
    int fastTotalTime = (endMillis - startMillis);
    Serial.print("Insertion sort ");
    Serial.print(length);
    Serial.print(" restaurants running time: ");
    Serial.print(fastTotalTime);
    Serial.println(" ms");

  } else {
    // read in from SD/ determine amount of restaurants to sort and start clock
    int length = readInRest();
    int startMillis = millis();

    // call quick sort and end clock
    qSort(length, rest_dist);
    int endMillis = millis();

    // calculate and output how long sort took
    int fastTotalTime = (endMillis - startMillis);
    Serial.print("Quick sort ");
    Serial.print(length);
    Serial.print(" restaurants running time: ");
    Serial.print(fastTotalTime);
    Serial.println(" ms");

    // reset variables needed for timing sort
    startMillis = 0;
    endMillis = 0;
    fastTotalTime = 0;

    // read in AGAIN from SD/ determine amount of restaurants to sort and start clock
    length = readInRest();
    startMillis = millis();

    // call insertion sort and end clock
    iSort(length, rest_dist);
    endMillis = millis();

    // calculate and output how long sort took
    fastTotalTime = (endMillis - startMillis);
    Serial.print("Insertion sort ");
    Serial.print(length);
    Serial.print(" restaurants running time: ");
    Serial.print(fastTotalTime);
    Serial.println(" ms");
  }

} // end of sortMethodChoice()

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

  scrollCounter = ((index) * 15) - (pageNumber * 300 + pageNumber * 15);
  tft.setCursor(0, scrollCounter);
  tft.setTextWrap(false);

  getRestaurantFast(rest_dist[index].index, & rest1);

  // the if highlights the current one
  if (index == highlightedString) {
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    // this will unhighlight the previous one
  } else {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

  }
  tft.print(rest1.name);
  tft.print("\n");

} // end of void displayText()

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

  // reads in restaurant into rest_dist
  readInRest();

  // chooses sorting method
  sortMethodChoice();

  for (int i = 0; i < 21; i++) {
    tft.setCursor(0, i * 15);
    // load in restaurant from SD card
    getRestaurantFast(rest_dist[i + (pageNumber * 21)].index, & rest1);
    // The following calculations were done to ensure that the first and last strings
    // were highlighted when moving down and up respectivily. These are not magic numbers.
    if (i == 0 && upMode == false) {
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
    } else if (i > 0 && upMode == false) {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    } else if (i == 20 && upMode == true) {
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
    } else if (i < 21 && upMode == true) {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    tft.print(rest1.name);
  }
  tft.print("\n");

} // end of void displayAllText()

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

  // draws inital screen and draws first restuarant highlighted
  displayAllText();

  while (modeCheck == true) {
    int yVal = analogRead(JOY_VERT);
    int buttonVal = digitalRead(JOY_SEL);

    // goes down
    if (yVal > (JOY_CENTER + JOY_DEADZONE)) {
      upMode = false;
      // increase highlighted string by one
      highlightedString = constrain(highlightedString + 1, 0, 1066);
      if (scrollCounter >= 300) {
        // increase page number set scroll counter back to top of page
        pageNumber++;
        displayAllText();
        scrollCounter = 0;
      } else {
        //unhighligt the old string
        displayText(highlightedString - 1);
        // highlight the new string
        displayText(highlightedString);
      }
    } // end of if joystick scrolled down

    // goes up
    if (yVal < (JOY_CENTER - JOY_DEADZONE)) {
      // draw the old highlighted string normally
      upMode = true;
      // decrease the highlighted string
      highlightedString = constrain(highlightedString - 1, 0, 1066);
      // if we're NOT at the top of the page
      if (scrollCounter != 0) {
        //unhighligt the old string
        displayText(highlightedString + 1);
        // highlight the new string
        displayText(highlightedString);
        // we're at the top of the page
      } else {
        if (pageNumber > 0) {
          --pageNumber;
          displayAllText();
          scrollCounter = DISPLAY_HEIGHT - 35;
        } else {
          scrollCounter = 0;
        }
      }
    }

    if ((buttonVal == 0) && (modeCheck == true)) {
      // load in restaurant from SD card
      getRestaurantFast(rest_dist[highlightedString].index, & rest1);

      modeCheck = false;
      yegCoordX = lon_to_x(rest1.lon) - (DISPLAY_WIDTH - 60) / 2;
      yegCoordY = lat_to_y(rest1.lat) - (DISPLAY_HEIGHT / 2);

      cursorX = (DISPLAY_WIDTH - 60) / 2;
      cursorY = DISPLAY_HEIGHT / 2;

      // The following control structures takes care of the boundary cases
      // forces cursor to stay in the left boundary, if restaurant in on the left side, but not on the map
      if (yegCoordX < (DISPLAY_WIDTH - 60) / 2) {
        yegCoordX = 0;
        cursorX = constrain(lon_to_x(rest1.lon), CURSOR_SIZE / 2, (DISPLAY_WIDTH - 61) - CURSOR_SIZE / 2);

        // forces cursor to stay in the right boundary, if restaurant in on the right side, but not on the map
      } else if (yegCoordX > (YEG_SIZE - (DISPLAY_WIDTH - 60) / 2)) {
        yegCoordX = constrain(YEG_SIZE - (DISPLAY_WIDTH - 60), 0, YEG_SIZE - (DISPLAY_WIDTH - 60));
        cursorX = constrain(YEG_SIZE - lon_to_x(rest1.lon), CURSOR_SIZE / 2, (DISPLAY_WIDTH - 61) - CURSOR_SIZE / 2);
      }

      // forces cursor to stay in the top boundary, if restaurant in on the top side, but not on the map
      if (yegCoordY < (DISPLAY_HEIGHT) / 2) {
        yegCoordY = 0;
        cursorY = constrain(lat_to_y(rest1.lat), CURSOR_SIZE / 2, DISPLAY_HEIGHT - CURSOR_SIZE / 2);

        // forces cursor to stay in the bottom boundary, if restaurant in on the bottom side, but not on the map 
      } else if (yegCoordY > (YEG_SIZE - DISPLAY_HEIGHT / 2)) {
        yegCoordY = constrain(yegCoordY + DISPLAY_HEIGHT, 0, YEG_SIZE - DISPLAY_HEIGHT);
        cursorY = constrain(YEG_SIZE - lat_to_y(rest1.lat), CURSOR_SIZE / 2, DISPLAY_HEIGHT - CURSOR_SIZE / 2);

      }

      highlightedString = 0;
      scrollCounter = 0;
      pageNumber = 0;
      upMode = false;
      tft.fillScreen(TFT_BLACK);
      optionsDisplaySetup();
      lcd_image_draw( & yegImage, & tft, yegCoordX, yegCoordY,
        0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);
      mode0();

    }
    delay(100);

  }

} // end of void mode1()

/*
  Description:
    Checks if you have tapped the screen inside the map or if you've tapped it inside
    one of the two buttons. Calls appropiate functions to deal with if you've clicked
    inside the buttons.

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
    int pty = map(touch.x, TS_MINY, TS_MAXY, 0, tft.height());
    // On the side pixel calculations were done to align and display
    // text more
    if (ptx > 100) {
      return true;
    } else {
      if (pty > 160) {
        // Rating selector
        ratingCounter++;
        delay(300);
        if (ratingCounter > 5) {
          ratingCounter = 1;
        }
        tft.fillRect(440, 60, 15, 30, TFT_WHITE);
        tft.setTextSize(2);
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
        tft.setCursor(445, 63);
        tft.print(ratingCounter);
      } else if (pty < 160) {
        // Sorting method
        sortMethod++;
        if (sortMethod > 3) {
          sortMethod = 1;
        }
        displaySortMethod();
        delay(200);
      }
    }

  }
  return false;

} // end of bool checkTouch

/*
  Description:
    Draws the restuarants as dots on the screen after the number of restuarants that are greater
    or equal to the rating have been selected and sorted. This is a function that is called when
    the map part of the screen has been tapped. 

  Arguments:
    None

  Returns:
    Nothing
*/
void displayRestaurants() {

  int length = readInRest();

  sortMethodChoice();

  int PEN_SIZE = 2;
  for (int i = 0; i < length; i++) {
    // load in restaurant from SD card
    getRestaurantFast(rest_dist[i].index, & rest1);
    if ((lon_to_x(rest1.lon) - (yegCoordX) <= 420) && (lat_to_y(rest1.lat) - (yegCoordY) <= 320)) {
      tft.fillCircle(lon_to_x(rest1.lon) - (yegCoordX), lat_to_y(rest1.lat) - (yegCoordY), PEN_SIZE, TFT_BLUE);
    }
  }

} // end of void displayRestaurants()

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
    pageNumber = 0;
    // Enters mode 1
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

} // end of void mode0()

int main() {
  setup();

  while (true) {
    mode0();
  }

  Serial.end();
  return 0;

}