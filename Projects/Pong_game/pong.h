#ifndef _PONG_H_
#define _PONG_H_

// Pong object cpp file
#include "const_and_lib.h"

// class declaration
class Pong {
public:
	int pongX, pongY;
	bool xDir, yDir;
	void reddrawPrevPong(int prevX, int prevY, uint16_t colour);
	void reddrawPong(uint16_t colour);
	Pong(int x, int y);
};

/*
  Description: Constructor for pong object. Stores the initial x and y 
               position of the pong.

  Arguments: initial_x (int) : initial x position
             initial_y (int) : initial y position

  Returns:
    Nothing
*/
Pong::Pong(int x, int y) {
	this->pongX = x;
	this->pongY = y;
}

/*
  Description: Draws the previous location at which the pong was before it moved

  Arguments: prevX (int): previous X point 
             prevY (int): previous Y point
             colour (uint16_t): color of previous location

  Returns:
    Nothing
*/
void Pong::reddrawPrevPong(int prevX, int prevY, uint16_t colour) {
	tft.fillCircle(prevX - PONG_SIZE/2, prevY - PONG_SIZE/2,
                PONG_SIZE/2, colour);
}

/*
  Description: Draws the next location at which the pong was before it moved

  Arguments: prevX (int): previous X point 
             prevY (int): previous Y point
             colour (uint16_t): color of next location

  Returns:
    Nothing
*/
void Pong::reddrawPong(uint16_t colour) {
	tft.fillCircle(this->pongX - PONG_SIZE/2, this->pongY - PONG_SIZE/2,
                PONG_SIZE/2, colour);
}


#endif