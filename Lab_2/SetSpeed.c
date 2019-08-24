#include <stdio.h>
#include "eyebot.h"
#include <time.h>
#include <math.h>


int main(){
    SIMSetRobot(0,1000,1000,1,0); // robot id, x pos, y pos, z pos, angle
    VWSetSpeed(50, 40); //linspeed and anglespeed
    VWWait();
}