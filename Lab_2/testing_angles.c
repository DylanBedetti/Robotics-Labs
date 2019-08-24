#include <stdio.h>
#include "eyebot.h"
#include <time.h>
#include <math.h>

#define pi acos(-1.0)
float angle;
int x; int y; int phi = 0;

int main(){
    SIMSetRobot(0,1000,1000,1,0);
    // angle = atan2(500,100)*180/pi;
    // printf("angle:%f\n", angle);

    // angle = atan2(500,-100)*180/pi;
    // printf("angle:%f\n", angle);

    // angle = atan2(-500,-100)*180/pi;
    // printf("angle:%f\n", angle);

    // angle = atan2(-500,100)*180/pi;
    // printf("angle:%f\n", angle);
    VWSetPosition(x, y, phi);
    while (true){
        VWGetPosition(&x, &y, &phi);
        MOTORDrive(1, 5);
        MOTORDrive(2, -5);
        printf("phi:%d\n", phi);
        }
}