#include <stdio.h>
#include "eyebot.h"
#include <time.h>

// Lab 1
// Dylan Bedetti 
// 23/08/19


float distances[3] = {0,0,0}; // where to store front, left and right distance values
int limit = 250; // distance limit 
int speed = 30; int push = 3; // speed is base speed, push is speed difference when trying to straighten
int turn_angle = 85; int turn_speed = 50; int turn_length = 200; // turn angle, speed and length for corners

void update(){
    // updating distance sensor
    // front, left, right
    for(int i = 0; i < 3; i++){
        distances[i] = PSDGet(i+1);
    }
}

void straight_and_turn_right(){
    float prev_distance;

    // initialise distance sensors
    update();

    while(distances[0] > limit && distances[1] > limit && distances[2] > limit) {
        VWStraight(50, 100);
        printf("\rleft: %f, front: %f, right: %f\t", distances[0], distances[1], distances[2] );
        fflush(stdout); // flushing stdout as it is buffered
        update();
    }

    // robot is now close to the wall

    prev_distance = distances[1];
    while(prev_distance >= distances[1]){ // current distance should be decreasing compared to previous until at right angle with wall
        // rotate right
        MOTORDrive(1, 10);
        MOTORDrive(2, -10);
        usleep(1000);

        printf("\rRotating, left sensor: %f\t", distances[1]);
        fflush(stdout);
        // update new and previous
        prev_distance = distances[1];
        update();
    }

    // Set motors to stop
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);

    // left sensor is closest, lets get to the wall!
    prev_distance = distances[1];
    while (distances[0] > limit) {
        update();

        if (distances[1] > prev_distance){
            MOTORDrive(1, speed - push);
            MOTORDrive(2, speed + push);
            printf("\rTurning left, Sensor is: %f  \t", distances[1]);
            fflush(stdout);
        } else if (distances[1] < prev_distance){
            MOTORDrive(1, speed + push);
            MOTORDrive(2, speed - push);
            printf("\rTurning right, Sensor is: %f  \t", distances[1]);
            fflush(stdout);
        } else {
            MOTORDrive(1, speed);
            MOTORDrive(2, speed);
            printf("\rGoing Straight, Sensor is: %f  \t", distances[1]);
            fflush(stdout);
        }
    prev_distance = distances[1];
    }

    // stop
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);
}

void rotate(int i){
    printf("\n TURNING 180 \n");
    if(i == 0){
        VWTurn(-turn_angle, turn_speed);
        VWWait();
        VWStraight(turn_length, 100);
        VWWait();
        VWTurn(-turn_angle, turn_speed);
        VWWait();
    }
    if(i == 1){
        VWTurn(turn_angle, turn_speed);
        VWWait();
        VWStraight(turn_length, 100);
        VWWait();
        VWTurn(turn_angle, turn_speed);
        VWWait();

    }
    // just go straight for a bit
    VWStraight(150, 100);
    VWWait();
    update();
}

void travel_straight_mf(){
    // 2 for starting on right, 1 for starting on left
    int k;
    int i;
    float prev_distance;

    // bit messy so could probably clean up

    if (distances[1] > distances[2]){
        i = 2;
        printf("\n right is closer \n");
    }else{
        i = 1;
        printf("\n left is closer \n");
    }

    prev_distance = distances[i];

    if (i == 2){
        k = -1;
    }else {
        k = 1;
    }

    while (distances[0] > limit) {
        update();

        if (distances[i] > prev_distance){
            MOTORDrive(1, speed - k*push);
            MOTORDrive(2, speed + k*push);
            printf("\rTurning, Sensor is: %f  \t", distances[1]);
            fflush(stdout);
        } else if (distances[i] < prev_distance){
            MOTORDrive(1, speed + k*push);
            MOTORDrive(2, speed - k*push);
            printf("\rTurning, Sensor is: %f  \t", distances[1]);
            fflush(stdout);
        } else {
            MOTORDrive(1, speed);
            MOTORDrive(2, speed);
            printf("\rGoing Straight, Sensor is: %f  \t", distances[1]);
            fflush(stdout);
        }
    prev_distance = distances[i];
    }

    // stop
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);

}

void lawnmower(){
    for(int i = 0; i < 100; i++){

        if (i % 2 == 0){
            rotate(0);
            travel_straight_mf();
        }
        if (i % 2 == 1){
            rotate(1);
            travel_straight_mf();
        }

        update();
        if((distances[1] < 300 || distances[2] < 300) && (i > 3)){
            printf("\nFINISHED BOIIIII");
            return;
        }
    }
}


int main() {
    printf("\n\n STARTING OPERATION \n\n");
    // Set robots position
    SIMSetRobot(0,1000,1500,1,20);
    // find wall and turn right
    straight_and_turn_right();
    // run through lawnmower
    lawnmower();

    return 0;
}