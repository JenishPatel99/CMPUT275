// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Name = Jenish Patel, Mattheas Jamesion
// Student ID: 1572027
// CMPUT 275, WINTER 2020
// Final project: Pong
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
	Demonstrating cursor movement over the map of Edmonton. You will improve over
  what we do in the next weekly exercise.
*/

#include "const_and_lib.h"
#include "pong.h"
#include "paddle.h"

// the cursor position on the display
Pong pong(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2);
Paddle rightPaddle((DISPLAY_WIDTH - PADDLE_SIZE_X/2), DISPLAY_HEIGHT/2);
Paddle leftPaddle(PADDLE_SIZE_X/2, DISPLAY_HEIGHT/2);

bool xDirection = true;
bool yDirection = true;
bool initialRightPaddle = true;
bool initialLeftPaddle = true;
bool bounceOnce = false;
bool startDirection = true; // "true": pong starts with right player , "false": pong starts with left player 
int player1_score = 0;
int player2_score = 0;

char globalByte;

int y_speed = 0;
int x_speed = 1;

Metro pongTimer = Metro(5);
Metro rightPaddleTimer = Metro(13);
Metro leftPaddleTimer = Metro(13);
Metro getInputTimer = Metro(50);

bool end = false;

// simple finite state machine
enum ConnectionState {
  MAINMENU, START, LEADERBOARDS
};

// Description : draws the main menu
void draw_main_menu(){
  // draw main menu, i.e start and leaderboard buttons and thats it

  // redraw background in case this function is called again after setup
  tft.fillScreen(TFT_BLACK);

  //draw start word and outline around it
  tft.setCursor(168, 75); // 177-155= 22
  tft.setTextSize(5);
  tft.print("START");
  tft.drawRect(161, 68, 159, 49, TFT_WHITE);

  // draw leaderboard word and outline surrounding it
  tft.setCursor(110, 205);
  tft.setTextSize(4);
  tft.print("LEADERBOARD");
  tft.drawRect(103, 198, 274, 42, TFT_WHITE);


}// end of void draw_main_menu

// a multimeter reading says there are 300 ohms of resistance across the plate,
// so initialize with this to get more accurate readings
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

/* 
  Description: Generic Arduino setup, includes: TFT display communication
               with Arduino. Declaration of pins and its pin mode. Initially
               sets screen to black

  Return: None
*/
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

  tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);
}

/*
  Description: Refreshes the variables involving speed, position and alternating
               booleans to its initial or respective states. The function is invoke
               whenever the gameplay is started.

  Return: None
*/
void initialState() {
  pong.pongX = DISPLAY_WIDTH/2;
  pong.pongY = DISPLAY_HEIGHT/2;

  rightPaddle.paddleX = DISPLAY_WIDTH - PADDLE_SIZE_X/2;
  rightPaddle.paddleY = DISPLAY_HEIGHT/2;

  leftPaddle.paddleX = PADDLE_SIZE_X/2;
  leftPaddle.paddleY = DISPLAY_HEIGHT/2;

  initialRightPaddle = true;
  initialLeftPaddle = true;

  y_speed = 0;

  xDirection = true;
  yDirection = true;

  player1_score = 0;
  player2_score = 0;

  if (startDirection) {
    startDirection = false;
    x_speed = 1;
  } else if (!startDirection) {
    startDirection = true;
    x_speed = -1;
  }
} 

/*
  Description: Gets input from the keyboard

  Return: returns input as char
*/
char getInput() {
  bool up;
  if (Serial.available() > 0) {
    globalByte = Serial.read();
    if (globalByte == 'w') {
      // move up
      return 'w';
    } else if (globalByte == 's') {
      // move down
      return 's';
    }
  } else {
    // no movement
    return 'e';
  }

  return up;
}

/*
  Description: Regularly checks if the pong is in the vertical range of the paddle.
               Also use to make sure that proper collision outputs happen. Meaning, if
               the pong is in range then game should never end.

  Return: true if pong is in range, false otherwise
*/
bool catchmentLeftZone() {
  if ((pong.pongY >= leftPaddle.paddleY - PADDLE_SIZE_Y/2 - PONG_SIZE) && (pong.pongY <= leftPaddle.paddleY + PADDLE_SIZE_Y/2 + PONG_SIZE * 2)) {
    return true;
  } else {
    return false;
  }
}

/*
  Description: Regularly checks if the pong is in the vertical range of the paddle.
               Also use to make sure that proper collision outputs happen. Meaning, if
               the pong is in range then game should never end.

  Return: true if pong is in range, false otherwise
*/
bool catchmentRightZone() {
	if ((pong.pongY >= rightPaddle.paddleY - PADDLE_SIZE_Y/2 - PONG_SIZE) && (pong.pongY <= rightPaddle.paddleY + PADDLE_SIZE_Y/2 + PONG_SIZE * 2)) {
		return true;
	} else {
		return false;
	}
}

/*
  Description: This function changes the x and y speeds of the pong depending
               on where the pong hits the paddle. The paddle is split into 5
               parts, each section has a different bounce speed. 

  Return: None
*/
void bounceSpeedRightPaddle() {
  if ((pong.pongY > rightPaddle.paddleY - PADDLE_SIZE_Y/2 - PONG_SIZE) && (pong.pongY < rightPaddle.paddleY - 10)) {
    //Serial.println("PHASE 1");
    y_speed = 2;
    x_speed = -2;
    yDirection = false;
  } else if ((pong.pongY > rightPaddle.paddleY - 10) && (pong.pongY < rightPaddle.paddleY)) {
    //Serial.println("PHASE 2");
    y_speed = 1;
    x_speed = -1;
    yDirection = false;
  } else if (pong.pongY == rightPaddle.paddleY) {
    //Serial.println("PHASE 3");
    y_speed = 0;
    x_speed = -1;
  } else if ((pong.pongY > rightPaddle.paddleY) && ((pong.pongY < rightPaddle.paddleY + 10))) {
    //Serial.println("PHASE 4");
    y_speed = 1;
    x_speed = -1;
    yDirection = true;
  } else if ((pong.pongY < rightPaddle.paddleY + PADDLE_SIZE_Y/2 + PONG_SIZE) && (pong.pongY > rightPaddle.paddleY - 10)) {
    //Serial.println("PHASE 5");
    y_speed = 2;
    x_speed = -2;
    yDirection = true;
  }
}

/*
  Description: This function changes the x and y speeds of the pong depending
               on where the pong hits the paddle. The paddle is split into 5
               parts, each section has a different bounce speed. 

  Return: None
*/
void bounceSpeedLeftPaddle() {
  if ((pong.pongY > leftPaddle.paddleY - PADDLE_SIZE_Y/2 - PONG_SIZE) && (pong.pongY < leftPaddle.paddleY - 10)) {
    //Serial.println("PHASE 1");
    y_speed = 2;
    x_speed = 2;
    yDirection = false;
  } else if ((pong.pongY > leftPaddle.paddleY - 10) && (pong.pongY < leftPaddle.paddleY)) {
    //Serial.println("PHASE 2");
    y_speed = 1;
    x_speed = 1;
    yDirection = false;
  } else if (pong.pongY == leftPaddle.paddleY) {
    //Serial.println("PHASE 3");
    y_speed = 0;
    x_speed = 1;
  } else if ((pong.pongY > leftPaddle.paddleY) && ((pong.pongY < leftPaddle.paddleY + 10))) {
    //Serial.println("PHASE 4");
    y_speed = 1;
    x_speed = 1;
    yDirection = true;
  } else if ((pong.pongY < leftPaddle.paddleY + PADDLE_SIZE_Y/2 + PONG_SIZE) && (pong.pongY > leftPaddle.paddleY - 10)) {
    //Serial.println("PHASE 5");
    y_speed = 2;
    x_speed = 2;
    yDirection = true;
  }
}

/*
  Description: Monitor overall position of the pong. Determines when the pong
               should change it's direction. Change of direction means that
               if the pong hits a surface then it will bounce with the same 
               momentum. 

  Return: None
*/
void constraintAndTracking(bool inZoneLeft, bool inZoneRight) {
  // Right side of the screen
  if ((pong.pongX <= DISPLAY_WIDTH - PADDLE_SIZE_X) && (xDirection) && (inZoneRight)) {
    pong.pongX = pong.pongX + x_speed;
    //Serial.println("HIT");
    xDirection = true;
  } else if ((pong.pongX <= DISPLAY_WIDTH) && (xDirection) && (!inZoneRight)) {
    pong.pongX = pong.pongX + x_speed;
    //Serial.println("HIT");
    xDirection = true;
  } else {
    xDirection = false;
  }

  // Left side of the screen
  if ((pong.pongX >= PADDLE_SIZE_X) && (!xDirection) && (inZoneLeft)) {
    pong.pongX = pong.pongX + x_speed;
    //pongY = pongY - MAX_SPEED;
    xDirection = false;
  } else if ((pong.pongX >= PONG_SIZE) && (!xDirection) && (!inZoneLeft)) {
    pong.pongX = pong.pongX + x_speed;
    //Serial.println("HIT");
    xDirection = false; 
  } else {
    xDirection = true;
  }

  // Bottom of the screen
  if (((pong.pongY <= DISPLAY_HEIGHT) && (yDirection))) {
    pong.pongY = pong.pongY + y_speed;
    yDirection = true;
  } else {
    yDirection = false;
  }

  // Top of the screen
  if ((pong.pongY >= 0) && (!yDirection)) {
    pong.pongY = pong.pongY - y_speed;
    yDirection = false;
  } else  {
    yDirection = true;
  }
}

/*
  Description: Key function which determines if an collision has occured.
               If collision between pong and paddle occurs then points are
               incremented else if the pong hits either left or right sides
               of the screen, then it signals to stop the gameplay, and exit
               out of the current state.

  Return: None
*/
void collisionDetection(bool inZoneLeft, bool inZoneRight) {
  if (inZoneRight == inZoneLeft) {
    if (x_speed < 0) {
      inZoneRight = false;
    } else {
      inZoneLeft = false;
    }
  }

  // Offset of 2 added so that graphics play out nicely
  if ((inZoneRight) && (pong.pongX > DISPLAY_WIDTH - PADDLE_SIZE_X - 2) && (!bounceOnce)) {
    bounceSpeedRightPaddle();
    Serial.println("HIT");
    bounceOnce = true;
    player1_score = player1_score + 1;
  } else if ((inZoneLeft) && (pong.pongX < PADDLE_SIZE_X + PONG_SIZE + 2) && (!bounceOnce)) {
    bounceSpeedLeftPaddle();
    Serial.println("HIT");
    bounceOnce = true;
    player2_score = player2_score + 1;
  } else if ((!inZoneRight) && (pong.pongX >= DISPLAY_WIDTH)) { 
    Serial.println("Game Over");
    end = true;
  } else if ((!inZoneLeft) && (pong.pongX <= PONG_SIZE)) {
    Serial.println("Game Over");  
    end = true;
  }
}

/*
  Description: Manages the graphics of the pong and acts as the driver function 
               for the pong's interactions

  Return: None
*/
void processPong() {
	// sets coordinates for previous x and y points
	int prevPongX = pong.pongX;
	int prevPongY = pong.pongY;

  //Serial.print(bounceOnce);
  if ((pong.pongX < DISPLAY_WIDTH/2 + 10) && (pong.pongX > DISPLAY_WIDTH/2 - 10)) {
    bounceOnce = false;
  }

	pong.reddrawPrevPong(prevPongX, prevPongY, TFT_BLACK);

  bool inZoneRight = catchmentRightZone();
  bool inZoneLeft = catchmentLeftZone();
  constraintAndTracking(inZoneLeft, inZoneRight);
  
  collisionDetection(inZoneLeft, inZoneRight);

  pong.reddrawPong(TFT_RED); 
  
}

/*
  Description: Manages the graphics of the paddle. Instead of an traditional redraw
               of the paddle, only a portion of the paddle (top pixels) are redrawn.

  Return: None
*/
void processLeftPaddle() {
  // draw paddle in starting position when program is first opened
  //if (initialRightPaddle) {
    //rightPaddle.reddrawPaddle(TFT_WHITE); 
    //initialRightPaddle = false;
  //}

  char globalByte = getInput();
  // redraw paddle if we have moved joystick
  if (globalByte != 'e') {
    // sets coordinates for previous x and y points
    int prevPaddleX = leftPaddle.paddleX;
    int prevPaddleY = leftPaddle.paddleY;

    // now move the cursor
    // this updates paddleY
    leftPaddle.moveVerticalKeyboard(globalByte, 5);
    if((leftPaddle.paddleY - 2) > 0 && (leftPaddle.paddleY)< (DISPLAY_HEIGHT - PADDLE_SIZE_Y/2)){
      //does paddle move up
      if(leftPaddle.paddleY - prevPaddleY < 0){
        // redraw small black patch 
        leftPaddle.reddrawPrevPaddleNEW(prevPaddleX, prevPaddleY, TFT_BLACK, true);
        // draw new white patch
        leftPaddle.reddrawPaddleNEW(prevPaddleX, prevPaddleY, TFT_WHITE, true);
      }else if(leftPaddle.paddleY - prevPaddleY > 0){ //paddle moves down
        // redraw small black patch 
        leftPaddle.reddrawPrevPaddleNEW(prevPaddleX, prevPaddleY, TFT_BLACK, false);

        // draw new white patch
        leftPaddle.reddrawPaddleNEW(prevPaddleX, prevPaddleY, TFT_WHITE, false);
      }
    }
  }
}// end of processRightPaddle()


/*
  Description: Manages the graphics of the paddle. Instead of an traditional redraw
               of the paddle, only a portion of the paddle (top pixels) are redrawn.

  Return: None
*/
void processRightPaddle() {
  // get input from joystick
	int yVal = analogRead(JOY_VERT);

  // redraw paddle if we have moved joystick
  if ((yVal > JOY_CENTER + JOY_DEADZONE) || (yVal < JOY_CENTER - JOY_DEADZONE)) {
	  // sets coordinates for previous x and y points
	  int prevPaddleX = rightPaddle.paddleX;
	  int prevPaddleY = rightPaddle.paddleY;


    // now move the cursor
    // this updates paddleY
    rightPaddle.moveVerticalJoyStick(yVal, 5);


    if((rightPaddle.paddleY - 2) > 0 && (rightPaddle.paddleY)< (DISPLAY_HEIGHT-PADDLE_SIZE_Y/2)){
    //does paddle move up
    if(rightPaddle.paddleY - prevPaddleY < 0){
      // redraw small black patch 
      rightPaddle.reddrawPrevPaddleNEW(prevPaddleX, prevPaddleY, TFT_BLACK, true);

      // draw new white patch
      rightPaddle.reddrawPaddleNEW(prevPaddleX, prevPaddleY, TFT_WHITE, true);


    }else if(rightPaddle.paddleY - prevPaddleY > 0){ //paddle moves down
      // redraw small black patch 
      rightPaddle.reddrawPrevPaddleNEW(prevPaddleX, prevPaddleY, TFT_BLACK, false);

      // draw new white patch
      rightPaddle.reddrawPaddleNEW(prevPaddleX, prevPaddleY, TFT_WHITE, false);
    }

    }

	  // draw a small patch of the Edmonton map at the old cursor position before
	  // drawing a red rectangle at the new cursor position
	  //reddrawPrevPaddle(prevPaddleX, prevPaddleY, TFT_BLACK);

	  // draw a cursor in the new connection
	  //reddrawPaddle(TFT_WHITE); 
	}
}// end of processRightPaddle()

/*
  Description: Uses the information from the joy_stick and correlates it to the
               cursor position on the TFT display.
  Arguments: None
*/
void process() {
	if(pongTimer.check()) {
		processPong();
	}

  if (initialLeftPaddle) {
    leftPaddle.redrawPaddle(TFT_WHITE); 
    delay(300);
    initialLeftPaddle = false;
  }

  if (initialRightPaddle) {
    rightPaddle.redrawPaddle(TFT_WHITE); 
    delay(300);
    initialRightPaddle = false;
  }

	if(rightPaddleTimer.check()) {
		processRightPaddle();
	}

  if(leftPaddleTimer.check()) {
    processLeftPaddle();
  }

  if(getInputTimer.check()) {
    getInput();
  }

	delay(3);
}

// Description : called from int main. Implementation of the FSM
void finiteStateMachine() {
  bool show_again = true;
    char initials_arr[4];
  ConnectionState state = MAINMENU;

  while(true) {
    if(state==MAINMENU){
        end = false;
        // draw the main menu screen with two buttons
        draw_main_menu();

        // break out of this statement if we get a valid click
        while(true){
          TSPoint touch = ts.getPoint();

          // restore pinMode to output after reading the touch
          // this is necessary to talk to tft display
          pinMode(YP, OUTPUT); 
          pinMode(XM, OUTPUT); 

          if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) {
            // we've touched screen now act
            int16_t screen_x = map(touch.y, TS_MINX, TS_MAXX, DISPLAY_WIDTH-1, 0);
            int16_t screen_y = map(touch.x, TS_MINY, TS_MAXY, DISPLAY_HEIGHT-1, 0);

            // now check if we've clicked START button
            if(screen_x>= 161 && screen_x<= 320 && screen_y>= 68 && screen_y<= 117){
              state = START;
              break; // breaks out of INSIDE infinte while loop

            // now check if we've clicked LEADERBOARD button
            }else if(screen_x>= 103 && screen_x<= 377 && screen_y>= 198 && screen_y<= 240){
              state = LEADERBOARDS;
              break; // breaks out of INSIDE infinte while loop

            }
            
          }

        }// end of inside while loop

      }else if(state == START){
        
        // now ask both players for initals and then begin game
        // serial communication happens here from keyboard to arduino
        // call process() unitl??? game is over, maybe function returns false when game is over
        // when game is over compare new highscore to old ones in EEPROM and replace if there
        // is a new

        //re draw screen black
        tft.fillScreen(TFT_BLACK);


        if(show_again){
          // give players instructions and ask to continune or not show again
          tft.setCursor(0, 0); 
          tft.setTextSize(2);
          tft.print("1. The objective of the game is to hit  the pong as many times as possible      without missing.");
          tft.setCursor(0, 57);
          tft.print("2. The joystick controls the right      paddle which is player 1, player 2 is   controlled by the 'w' and 's' keys and  is the left paddle.");
          tft.setCursor(0, 130 );
          tft.print("3. On the next screen enter the intials of each player, player 1 enters their initials first followed by player 2");

          // draw buttons
          tft.setCursor(100, 200);
          tft.setTextSize(3);
          tft.print("DON'T SHOW AGAIN"); 
          tft.drawRect(93, 193, 300, 35, TFT_WHITE);
          tft.setCursor(169, 267);
          tft.print("CONTINUE");
          tft.drawRect(162, 260, 155, 35, TFT_WHITE);


          // while loop waits for selection of either button
          while(true){
            TSPoint touch = ts.getPoint();

            // restore pinMode to output after reading the touch
            // this is necessary to talk to tft display
            pinMode(YP, OUTPUT); 
            pinMode(XM, OUTPUT); 

            if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) {
              // we've touched screen now act
              int16_t screen_x = map(touch.y, TS_MINX, TS_MAXX, DISPLAY_WIDTH-1, 0);
              int16_t screen_y = map(touch.x, TS_MINY, TS_MAXY, DISPLAY_HEIGHT-1, 0);

              // now check if we've clicked DONT SHOW AGAIN button
              if(screen_x>= 93 && screen_x<= 393 && screen_y>= 193 && screen_y<= 228){
                show_again = false;
                break; // breaks out of INSIDE infinte while loop

              // now check if we've clicked CONTINUE button
              }else if(screen_x>= 162 && screen_x<= 317 && screen_y>= 260 && screen_y<= 295){
                show_again = true;
                break; // breaks out of INSIDE infinte while loop

              }
              
            }

          }// end of inside while loop

          

          //re draw screen black before we play game to clear screen
          //tft.fillScreen(TFT_BLACK);


        }// end of if(showagain)
        


        //re draw screen black
        tft.fillScreen(TFT_BLACK);

        // prompt user to enter initals of both players
        tft.setCursor(52, 140); 
        tft.setTextSize(3);
        tft.print("PLEASE ENTER INITIALS");
        delay(100);

        // player 1 or right paddle is first initals in array
        int letter_counter = 0;

        // while loop reads in initials of both players
        while(letter_counter<=3){
          if (Serial.available() > 0) {

            char current_letter = Serial.read();
            if ((current_letter>=65 && current_letter<=90) || (current_letter>=97 && current_letter<=122)) {
              // store letter in array and grab if necessary when checking high score
              // then also store in eeprom if necessary

              // store initals of both players in array
              initials_arr[letter_counter] = current_letter;
              letter_counter++;

              Serial.println(current_letter);
            }

          }

        
        }// end of while loop


        //re draw screen black
        tft.fillScreen(TFT_BLACK);

        // start game and run it until somebody loses
        while (!end) {
          process();
        } 

        //re draw screen black
        tft.fillScreen(TFT_BLACK);

        if(player1_score>player2_score){
          // player 1 or right paddle won

          // draw initals of winning player
          tft.drawChar(25,100,initials_arr[0] ,TFT_WHITE,0,3);
          tft.drawChar(43,100,'.' ,TFT_WHITE,0,3);   
          tft.drawChar(61,100,initials_arr[1] ,TFT_WHITE,0,3);

          // draws rest of sentence and score of player
          tft.setCursor(93, 100); 
          tft.setTextSize(3);
          tft.print("WON WITH A SCORE ");
          tft.print(player1_score);

        }else if (player1_score==player2_score){
          // player 2 or left paddle won

          // draw initals of winning player
          tft.drawChar(25,100,initials_arr[2] ,TFT_WHITE,0,3);
          tft.drawChar(43,100,'.' ,TFT_WHITE,0,3);   
          tft.drawChar(61,100,initials_arr[3] ,TFT_WHITE,0,3);

          // draws rest of sentence and score of player
          tft.setCursor(93, 100); 
          tft.setTextSize(3);
          tft.print("WON WITH A SCORE ");
          tft.print(player2_score);
        }

        delay(3000);

        // blank over screen
        tft.fillScreen(TFT_BLACK);

        if(player1_score>player2_score){
          // player 1 or right paddle won
          // check if player 1 made it into the top three
          
          // there is no 3rd place saved
          if(EEPROM[6] == 255 && EEPROM[7] == 255){
            
            // if there is no 2nd place saved
            if(EEPROM[3] == 255 && EEPROM[4] == 255){

              // if there is no 1st place saved
              if(EEPROM[0] == 255 && EEPROM[1] == 255){
                
                // update first place with new initals and new score
                EEPROM[0] = initials_arr[0];
                EEPROM[1] = initials_arr[1];
                EEPROM[2] = player1_score;
              }else{
                // there is someone in first place check score and update
                if(player1_score>EEPROM[2]){
                  // prev 1st place is moved to second place
                  EEPROM[3] = EEPROM[0];
                  EEPROM[4] = EEPROM[1];
                  EEPROM[5] = EEPROM[2];

                  //update new winner to 1st place
                  EEPROM[0] = initials_arr[0];
                  EEPROM[1] = initials_arr[1];
                  EEPROM[2] = player1_score;
                }else{
                  // player 1 is now second highest score
                  EEPROM[3] = initials_arr[0];
                  EEPROM[4] = initials_arr[1];
                  EEPROM[5] = player1_score;
                }
              }// end of inside else statement
              


            }else{
              // both 1st and 2nd place have scores saved compare to first place then second
              // else there is someone in second place check score and update

              // round winner is new first place winner
              if(player1_score>EEPROM[2]){
                // prev second place is moved to third place
                EEPROM[3] = EEPROM[6];
                EEPROM[4] = EEPROM[7];
                EEPROM[5] = EEPROM[8];

                // prev 1st place is moved to second place
                EEPROM[0] = EEPROM[3];
                EEPROM[1] = EEPROM[4];
                EEPROM[2] = EEPROM[5];

                // first place is updated
                EEPROM[0] = initials_arr[0];
                EEPROM[1] = initials_arr[1];
                EEPROM[2] = player1_score;

              // round winner is new 2nd place winner
              }else if(player1_score>EEPROM[5]){
                // prev 2nd place is moved to third place
                EEPROM[6] = EEPROM[3];
                EEPROM[7] = EEPROM[4];
                EEPROM[8] = EEPROM[5];

                //update new winner to 2nd place
                EEPROM[3] = initials_arr[0];
                EEPROM[4] = initials_arr[1];
                EEPROM[5] = player1_score;
              }else{
                // player 1 is now third highest score
                EEPROM[6] = initials_arr[0];
                EEPROM[7] = initials_arr[1];
                EEPROM[8] = player1_score;
              }
            }


          }else{
            // all three places are occupied
            // round winner is new first place winner
            if(player1_score>EEPROM[2]){
              // prev second place is moved to third place
              EEPROM[3] = EEPROM[6];
              EEPROM[4] = EEPROM[7];
              EEPROM[5] = EEPROM[8];

              // prev 1st place is moved to second place
              EEPROM[0] = EEPROM[3];
              EEPROM[1] = EEPROM[4];
              EEPROM[2] = EEPROM[5];

              // first place is updated
              EEPROM[0] = initials_arr[0];
              EEPROM[1] = initials_arr[1];
              EEPROM[2] = player1_score;

            // round winner is new 2nd place winner
            }else if(player1_score>EEPROM[5]){
              // prev 2nd place is moved to third place
              EEPROM[6] = EEPROM[3];
              EEPROM[7] = EEPROM[4];
              EEPROM[8] = EEPROM[5];

              //update new winner to 2nd place
              EEPROM[3] = initials_arr[0];
              EEPROM[4] = initials_arr[1];
              EEPROM[5] = player1_score;
            }else if(player1_score>EEPROM[8]){
              // player 1 is now third highest score
              EEPROM[6] = initials_arr[0];
              EEPROM[7] = initials_arr[1];
              EEPROM[8] = player1_score;
            }
          }


        }else{
          // else player 2 won the round

          // there is no 3rd place saved
          if(EEPROM[6] == 255 && EEPROM[7] == 255){
            
            // if there is no 2nd place saved
            if(EEPROM[3] == 255 && EEPROM[4] == 255){

              // if there is no 1st place saved
              if(EEPROM[0] == 255 && EEPROM[1] == 255){
                
                // update first place with new initals and new score
                EEPROM[0] = initials_arr[2];
                EEPROM[1] = initials_arr[3];
                EEPROM[2] = player2_score;
              }else{
                // there is someone in first place check score and update
                if(player2_score>EEPROM[2]){
                  // prev 1st place is moved to second place
                  EEPROM[3] = EEPROM[0];
                  EEPROM[4] = EEPROM[1];
                  EEPROM[5] = EEPROM[2];

                  //update new winner to 1st place
                  EEPROM[0] = initials_arr[2];
                  EEPROM[1] = initials_arr[3];
                  EEPROM[2] = player2_score;
                }else{
                  // player 1 is now second highest score
                  EEPROM[3] = initials_arr[2];
                  EEPROM[4] = initials_arr[3];
                  EEPROM[5] = player2_score;
                }
              }// end of inside else statement
              


            }else{
              // both 1st and 2nd place have scores saved compare to first place then second
              // else there is someone in second place check score and update

              // round winner is new first place winner
              if(player2_score>EEPROM[2]){
                // prev second place is moved to third place
                EEPROM[3] = EEPROM[6];
                EEPROM[4] = EEPROM[7];
                EEPROM[5] = EEPROM[8];

                // prev 1st place is moved to second place
                EEPROM[0] = EEPROM[3];
                EEPROM[1] = EEPROM[4];
                EEPROM[2] = EEPROM[5];

                // first place is updated
                EEPROM[0] = initials_arr[2];
                EEPROM[1] = initials_arr[3];
                EEPROM[2] = player2_score;

              // round winner is new 2nd place winner
              }else if(player2_score>EEPROM[5]){
                // prev 2nd place is moved to third place
                EEPROM[6] = EEPROM[3];
                EEPROM[7] = EEPROM[4];
                EEPROM[8] = EEPROM[5];

                //update new winner to 2nd place
                EEPROM[3] = initials_arr[2];
                EEPROM[4] = initials_arr[3];
                EEPROM[5] = player2_score;
              }else{
                // player 1 is now third highest score
                EEPROM[6] = initials_arr[2];
                EEPROM[7] = initials_arr[3];
                EEPROM[8] = player2_score;
              }
            }
          }else{
            // all three places are occupied
            // round winner is new first place winner
            if(player2_score>EEPROM[2]){
              // prev second place is moved to third place
              EEPROM[3] = EEPROM[6];
              EEPROM[4] = EEPROM[7];
              EEPROM[5] = EEPROM[8];

              // prev 1st place is moved to second place
              EEPROM[0] = EEPROM[3];
              EEPROM[1] = EEPROM[4];
              EEPROM[2] = EEPROM[5];

              // first place is updated
              EEPROM[0] = initials_arr[2];
              EEPROM[1] = initials_arr[3];
              EEPROM[2] = player2_score;

            // round winner is new 2nd place winner
            }else if(player2_score>EEPROM[5]){
              // prev 2nd place is moved to third place
              EEPROM[6] = EEPROM[3];
              EEPROM[7] = EEPROM[4];
              EEPROM[8] = EEPROM[5];

              //update new winner to 2nd place
              EEPROM[3] = initials_arr[2];
              EEPROM[4] = initials_arr[3];
              EEPROM[5] = player2_score;
            }else if(player2_score>EEPROM[8]){
              // player 1 is now third highest score
              EEPROM[6] = initials_arr[2];
              EEPROM[7] = initials_arr[3];
              EEPROM[8] = player2_score;
            }
            
          }
        }// end of else

        // change to leaderbaords screen to see if you made a new high score
        state = LEADERBOARDS;

        // reset all stats for pong and paddles
        initialState();
      
      }else if(state == LEADERBOARDS){
      
        // redraw background black
        tft.fillScreen(TFT_BLACK);

        // draw return to main menu button and wait for click event
        tft.setCursor(8, 284);
        tft.setTextSize(4);
        tft.print("MAIN MENU");
        tft.drawRect(0, 277, 228, 42, TFT_WHITE);

        // now draw results from EEPROM on LEADERBOARD screen
        
        // if there is a first place saved print it to screen
        if(EEPROM[0] != 255 && EEPROM[1] != 255){
          // draw initals of winning player
          tft.setTextSize(3);
          tft.setCursor(160, 100);
          tft.print("1. ");
          tft.drawChar(214,100,EEPROM[0] ,TFT_WHITE,0,3);
          tft.drawChar(232,100,'.' ,TFT_WHITE,0,3);   
          tft.drawChar(250,100,EEPROM[1] ,TFT_WHITE,0,3);

          // draws rest of sentence and score of player
          tft.setCursor(286, 100); 
          tft.setTextSize(3);

          tft.print(EEPROM[2]);
        }

        if(EEPROM[3] != 255 && EEPROM[3] != 255){
          // draw initals of winning player
          tft.setCursor(160, 140);
          tft.print("2. ");
          tft.drawChar(214,140,EEPROM[3] ,TFT_WHITE,0,3);
          tft.drawChar(232,140,'.' ,TFT_WHITE,0,3);   
          tft.drawChar(250,140,EEPROM[4] ,TFT_WHITE,0,3);

          // draws rest of sentence and score of player
          tft.setCursor(286, 140); 
          tft.setTextSize(3);

          tft.print(EEPROM[5]);
        }

        if(EEPROM[6] != 255 && EEPROM[6] != 255){
          // draw initals of winning player
          tft.setCursor(160, 180);
          tft.print("3. ");
          tft.drawChar(214,180,EEPROM[6] ,TFT_WHITE,0,3);
          tft.drawChar(232,180,'.' ,TFT_WHITE,0,3);   
          tft.drawChar(250,180,EEPROM[7] ,TFT_WHITE,0,3);

          // draws rest of sentence and score of player
          tft.setCursor(286, 180); 
          tft.setTextSize(3);

          tft.print(EEPROM[8]);
        }

        // break out of this statement if we get a valid click on return to main meu
        while(true){
          TSPoint touch = ts.getPoint();

          // restore pinMode to output after reading the touch
          // this is necessary to talk to tft display
          pinMode(YP, OUTPUT); 
          pinMode(XM, OUTPUT); 

          if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) {
            // we've touched screen now act
            int16_t screen_x = map(touch.y, TS_MINX, TS_MAXX, DISPLAY_WIDTH-1, 0);
            int16_t screen_y = map(touch.x, TS_MINY, TS_MAXY, DISPLAY_HEIGHT-1, 0);

            // now check if we've clicked MAIN MENU button
            if(screen_x>= 0 && screen_x<= 228 && screen_y>= 277 && screen_y<= 320){
              state = MAINMENU;
              break; // breaks out of INSIDE infinte while loop
            }
            
          }

        }// end of inside while loop
    
      }
  }
}


// Driver function
int main() {

  setup();
  //draw_main_menu();

  finiteStateMachine();

  Serial.end();
  return 0;
}
