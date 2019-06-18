#include <stdio.h>
#include "eyebot.h"
#include <time.h>

int d = 280;
int error = 2;
int push = 2;
int speed = 30;
int distance[8];
int back;
int front;

void lidar(){
    while(1){
        VWStraight(50, 100);
        LIDARGet(distance);
        for(int i = 0; i < 8; i++) {
            printf("%d\t", distance[i]);
        }
        printf("\n");

        // back = distance[0];
        // front = distance[2];

        // if(front > back - error){
        //     MOTORDrive(1, speed - push);
        //     MOTORDrive(2, speed + push);
        // }
        // else if(front < back + error){
        //     MOTORDrive(1, speed + push);
        //     MOTORDrive(2, speed - push);
        // }
        // else{
        //     MOTORDrive(1, speed);
        //     MOTORDrive(2, speed);
        // }


    }
}


int main() {

    SIMSetRobot(0,500,1800,1,90);
    // SIMSetRobot(0,1000,1800,1,90); // Start along edge
    // VWStraight(50, 100);
    // VWWait();
    LIDARSet(360, 0, 8);
    lidar();
}