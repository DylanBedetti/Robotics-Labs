#include <stdio.h>
#include "eyebot.h"
#include <time.h>
#include <math.h>

void move(float x, float y){
    // function to move to the desired vector locations
    VWDrive(x, y, 20);
    VWWait();
    usleep(1000);
}


int main() {
    SIMSetRobot(0,1000,1000,1,0);
    move(200,200);
    return 0;
}