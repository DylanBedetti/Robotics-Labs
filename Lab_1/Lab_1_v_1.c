#include <stdio.h>
#include "eyebot.h"
#include <time.h>

int x, y, phi;

int limit = 250; int turn_speed = 50;
int turn_length = 100; int turn_angle = 87;
float prev;
float distances[3] = {0,0,0};
const int speed = 30; 
const int push = 3;
int j;

// need to use train setting?
// need to find end point
// Could use an error for run along wall function to get it running smoother


void update(){
    // front,left, right
    for(int i = 0; i < 3; i++){
        distances[i] = PSDGet(i+1);
    }
}

void straight(){
    // function that will go straight and check sensors until they are below limit
    update();

    while(distances[0] > limit && distances[1] > limit && distances[2] > limit){
        VWStraight(50, 100);
        printf("\rleft: %f, front: %f, right: %f", distances[0], distances[1], distances[2] );
        fflush(stdout); // flushing stdout as it is buffered

        update();
    }
}

void rotate_right(){
    // function that will make the robot rotate right until a minimum distance on the left sensor is found

    while(distances[1] > 500){
        MOTORDrive(1, 10);
        MOTORDrive(2, -10);
        usleep(1000);

        update();
        printf("\rRotating, left sensor: %f", distances[1]);
        fflush(stdout);
    }

    update();
    prev = distances[1];
    while(prev >= distances[1]){
        prev = distances[1];
        update();

        MOTORDrive(1, 10);
        MOTORDrive(2, -10);
        usleep(1000);
        printf("\rRotating, left sensor: %f", distances[1]);
        fflush(stdout);
    }
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);
}

void run_along_wall(int k){
    // Smooth following of great wall
    // for LEFT facing: k = 1
    // for RIGHT facing: k = 2

    // j is used to reverse the polarity of the turn when using the right sensor instead of the left
    if(k == 2){
        j = -1;
    }
    else{
        j = 1;
    }

    while(distances[0] > limit){
        update();

        if(distances[k] > prev){
            MOTORDrive(1, speed - j*push);
            MOTORDrive(2, speed + j*push);
            printf("\rTurning left, Sensor is: %f  ", distances[k]);
            fflush(stdout);
        }
        else if(distances[k] < prev){
            MOTORDrive(1, speed + j*push);
            MOTORDrive(2, speed - j*push);
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
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);
}

void edge(int k){
    // 180 degree turn at edges of great wall
    // for LEFT turn: k = 1
    // for RIGHT turn: k = 2
    if(k == 1){
        VWTurn(-turn_angle, turn_speed);
        VWWait();
        VWStraight(turn_length, 100);
        VWWait();
        VWTurn(-turn_angle, turn_speed);
        VWWait();
    }
    if(k == 2){
        VWTurn(turn_angle, turn_speed);
        VWWait();
        VWStraight(turn_length, 100);
        VWWait();
        VWTurn(turn_angle, turn_speed);
        VWWait();

    }
}
void lawnmower(){
    for(int i = 0; i < 100; i++){
        printf("\n RUN %d \n",i);
        if(i % 2 == 0){
            printf("\nWE BE TURNING RIGHT\n");
            edge(1);
            update();
            if((distances[1] < 300 || distances[2] < 300) && (i > 3)){
                printf("\nFINISHED BOIIIII");
                return;
            }

            run_along_wall(2);
        }
        if(i % 2 == 1){
            printf("\nWE BE TURNING LEFT\n");
            edge(2);
            update();
            if((distances[1]< 300 || distances[2] < 300) && (i > 3)){
                printf("\nFINISHED BOIIIII");
                return;
            }
            run_along_wall(1);
        }
    }

}

// USE VWGETPOSITION ALONG WITH PHI CONTROL INSTEAD OF BANGBANG CONTROL FOR DISTANACE
int main() {
    // Set robots position
    SIMSetRobot(0,1000,1500,1,0); //vary last element to change degrees of starting point
    // Run straight to closest wall
    straight();
    // rotate to be parallel to wall
    rotate_right();
    // run straight along wall 
    run_along_wall(1);
    // start lawnmower pattern
    lawnmower();

    return 0;
}