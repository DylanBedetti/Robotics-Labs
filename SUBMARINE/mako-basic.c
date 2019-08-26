// Mako Basic Movement Demo, M. Finn May 2018
#include "eyebot.h"

// PSD IDs.
const int PSD_DOWN = 6;

// Thruster IDs.
const int LEFT = 1;
const int FRONT = 2;
const int RIGHT = 3;
const int BACK = 4;

const int SPEED = 70;

void up(float secs)
{   LCDPrintf("UP\n");
    MOTORDrive(FRONT, SPEED);
    MOTORDrive(BACK, SPEED);
    sleep(secs);
    MOTORDrive(FRONT, 0);
    MOTORDrive(BACK, 0);
}

void down(float secs)
{   LCDPrintf("DOWN\n");
    MOTORDrive(FRONT, -SPEED);
    MOTORDrive(BACK, -SPEED);
    sleep(secs);
    MOTORDrive(FRONT, 0);
    MOTORDrive(BACK, 0);
}

void forward(float secs)
{   LCDPrintf("FORWARD\n");
    MOTORDrive(LEFT, SPEED);
    MOTORDrive(RIGHT, SPEED);
    sleep(secs);
    MOTORDrive(LEFT, 0);
    MOTORDrive(RIGHT, 0);
}

void backward(float secs)
{   LCDPrintf("BACKWARD\n");
    MOTORDrive(LEFT, -SPEED);
    MOTORDrive(RIGHT, -SPEED);
    sleep(secs);
    MOTORDrive(LEFT, 0);
    MOTORDrive(RIGHT, 0);
}

void turn_left(float secs)
{   LCDPrintf("LEFT\n");
    MOTORDrive(LEFT, -SPEED);
    MOTORDrive(RIGHT, SPEED);
    sleep(secs);
    MOTORDrive(LEFT, 0);
    MOTORDrive(RIGHT, 0);
}

void turn_right(float secs)
{   LCDPrintf("RIGHT\n");
    MOTORDrive(LEFT, SPEED);
    MOTORDrive(RIGHT, -SPEED);
    sleep(secs);
    MOTORDrive(LEFT, 0);
    MOTORDrive(RIGHT, 0);
}

int main()
{ up(3);
  forward(3);
  turn_left(3);
  turn_right(3);
  backward(3);
  down(3);
}
