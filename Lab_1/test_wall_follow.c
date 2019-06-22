#include <stdio.h>
#include "eyebot.h"
#include <time.h>

float prev;
float distances[3] = {0,0,0};
const int speed = 30; const int push = 3;

void update(){
    // front,left, right
    for(int i = 0; i < 3; i++){
        distances[i] = PSDGet(i+1);
    }
}

void run_along_wall(int k){
    // Smooth following of great wall
    // for LEFT facing: k = 1
    // for RIGHT facing: k = 2
    while(distances[0] > 300){
        update();
        if(distances[k] > prev){
            MOTORDrive(1, speed - push);
            MOTORDrive(2, speed + push);
            printf("\rTurning left, Sensor is: %f  ", distances[k]);
            fflush(stdout);
        }
        else if(distances[k] < prev){
            MOTORDrive(1, speed + push);
            MOTORDrive(2, speed - push);
            printf("\rTurning right, Sensor is: %f  ", distances[k]);
            fflush(stdout);
        }
        else{
            MOTORDrive(1, speed);
            MOTORDrive(2, speed);
            printf("\rgoing straight, Sensor is: %f  ", distances[k]);
            fflush(stdout);
        }
        prev = distances[k];
    }
}



int main() {

    SIMSetRobot(0,500,1800,1,120);
    // run_along_wall();
    update();
    run_along_wall(1);

}