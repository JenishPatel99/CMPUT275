#ifndef _PADDLE_H_
#define _PADDLE_H_

#include "const_and_lib.h"

// class declaration
class Paddle {

public:
	int paddleX, paddleY;
	Paddle(int initial_x, int initial_y);
	void reddrawPrevPaddleNEW(int prevX, int prevY, uint16_t colour, bool move_up);
	void reddrawPaddleNEW(int prevX, int prevY, uint16_t colour, bool move_up);
	void moveVerticalJoyStick(int yVal, int speed);
  void moveVerticalKeyboard(char byte, int speed);
  void redrawPaddle(uint16_t colour);
};

/*
  Description: Constructor for paddle object. Stores the initial x and y 
               position of the paddle.

  Arguments: initial_x (int) : initial x position
             initial_y (int) : initial y position

  Returns:
    Nothing
*/
Paddle::Paddle(int initial_x, int initial_y) {
	this->paddleX = initial_x;
	this->paddleY = initial_y;
}

/*
  Description: Draws the previous location at which the paddle was before it moved

  Arguments: prevX (int): previous X point 
             prevY (int): previous Y point
             colour (uint16_t): color of previous location
             move_up (bool): tells if paddle moving up and down

  Returns:
    Nothing
*/
void Paddle::reddrawPrevPaddleNEW(int prevX, int prevY, uint16_t colour, bool move_up) {
  if(move_up){
    tft.fillRect(prevX - PADDLE_SIZE_X/2, prevY + PADDLE_SIZE_Y/2,
                PADDLE_SIZE_X, abs(prevY-this->paddleY), colour);
  }else if(!move_up){
    tft.fillRect(prevX - PADDLE_SIZE_X/2, prevY - PADDLE_SIZE_Y/2,
                PADDLE_SIZE_X, abs(prevY-this->paddleY), colour);
  }
}

/*
  Description: Draws the next location at which the paddle was before it moved

  Arguments: prevX (int): previous X point 
             prevY (int): previous Y point
             colour (uint16_t): color of next location
             move_up (bool): tells if paddle moving up and down

  Returns:
    Nothing
*/
void Paddle::reddrawPaddleNEW(int prevX, int prevY, uint16_t colour, bool move_up) {
  if(move_up){
    tft.fillRect(prevX - PADDLE_SIZE_X/2, this->paddleY - PADDLE_SIZE_Y/2,
                PADDLE_SIZE_X, abs(prevY-this->paddleY), colour);
  }else if(!move_up){
    tft.fillRect(prevX - PADDLE_SIZE_X/2, this->paddleY + PADDLE_SIZE_Y/2,
                PADDLE_SIZE_X, abs(prevY-this->paddleY), colour);
  }
}

/*
  Description: Needed only once to draw the initial position of the paddles

  Arguments: colour (uint16_t): color of new location

  Returns:
    Nothing
*/
void Paddle::redrawPaddle(uint16_t colour) {
  tft.fillRect(this->paddleX - PADDLE_SIZE_X/2, this->paddleY - PADDLE_SIZE_Y/2,
                PADDLE_SIZE_X, PADDLE_SIZE_Y, colour);
}


/*
  Description: Moves the paddle up or down using the joystick

  Arguments: yVal (int) : joystick input
             speed (int) : current speed of the paddle

  Returns:
    Nothing
*/
void Paddle::moveVerticalJoyStick(int yVal, int speed) {
  
  if (yVal < JOY_CENTER - JOY_DEADZONE) {
    this->paddleY = constrain(this->paddleY - speed, PADDLE_SIZE_Y/2, DISPLAY_HEIGHT - PADDLE_SIZE_Y/2); // decrease the y coordinate of the cursor
  }
  else if (yVal > JOY_CENTER + JOY_DEADZONE) {
    this->paddleY = constrain(this->paddleY + speed, PADDLE_SIZE_Y/2, DISPLAY_HEIGHT - PADDLE_SIZE_Y/2);
  }
}

/*
  Description: Moves the paddle up or down using the keys "w" and "s" respectively

  Arguments: byte (char) : input from serial
             speed (int) : current speed of the paddle

  Returns:
    Nothing
*/
void Paddle::moveVerticalKeyboard(char byte, int speed) {
  if (byte == 'w') {
    this->paddleY = constrain(this->paddleY - speed, PADDLE_SIZE_Y/2, DISPLAY_HEIGHT - PADDLE_SIZE_Y/2); // decrease the y coordinate of the cursor
  }
  else if (byte == 's') {
    this->paddleY = constrain(this->paddleY + speed, PADDLE_SIZE_Y/2, DISPLAY_HEIGHT - PADDLE_SIZE_Y/2);
  }
}

#endif