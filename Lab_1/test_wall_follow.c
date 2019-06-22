#include <stdio.h>
#include "eyebot.h"
#include <time.h>

float left, front, right, left_prev;
int speed = 30;
int push = 3;

void run_along_wall(){
    left = PSDGet(2);
    front = PSDGet(1);
    while(front > 300){
        left = PSDGet(2);
        front = PSDGet(1);
        if(left > left_prev){
            MOTORDrive(1, speed - push);
            MOTORDrive(2, speed + push);
            printf("\rTurning left, Sensor is: %f  ", left);
            fflush(stdout);
        }
        else if(left < left_prev){
            MOTORDrive(1, speed + push);
            MOTORDrive(2, speed - push);
            printf("\rTurning right, Sensor is: %f  ", left);
            fflush(stdout);
        }
        else{
            MOTORDrive(1, speed);
            MOTORDrive(2, speed);
            printf("\rgoing straight, Sensor is: %f  ", left);
            fflush(stdout);
        }




        left_prev = left;
    }
}



int main() {

    SIMSetRobot(0,500,1800,1,120);
    run_along_wall();

}