#include <stdio.h>
#include "eyebot.h"
#include <time.h>

float left, front, right;
int distance[8];
int limit = 250;
int left_prev, x, y, phi;
int total_dist = 1949;
int error = 2;
int push = 2;
int speed = 30;
int f, b;



void straight(){
    // function that will go straight and check sensors until they are below limit
    left = PSDGet(2);
    front = PSDGet(1);
    right = PSDGet(3);

    while(left > limit && front > limit && right > limit){
        VWStraight(50, 100);
        printf("\rleft: %f, front: %f, right: %f", left, front, right);
        fflush(stdout); // flushing stdout as it is buffered

        left = PSDGet(2);
        front = PSDGet(1);
        right = PSDGet(3);
    }
}

void rotate_right(){
    // function that will make the robot rotate right until a minimum distance on the left sensor is found

    while(left > 500){
        MOTORDrive(1, 10);
        MOTORDrive(2, -10);
        usleep(1000);
        left = PSDGet(2);
        printf("\rRotating, left sensor: %f", left);
        fflush(stdout);
    }
    left = PSDGet(2);
    left_prev = left;
    while(left_prev >= left){
        left_prev = left;
        left = PSDGet(2);
        MOTORDrive(1, 10);
        MOTORDrive(2, -10);
        usleep(1000);
        printf("\rRotating, left sensor: %f", left);
        fflush(stdout);
    }
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);

}

void find_corner(int l, int k){
    // A function to find the right most corner by travelling next to the wall with a bang-bang controller
    // k is 2 or 3
    left = PSDGet(2);
    front = PSDGet(1);
    right = PSDGet(3);
    printf("Moving towards corner\n");
    
    while(front > limit){
        // as the robot gets further our the lidar will hit the sides!!!
        LIDARGet(distance);
        b = distance[0];
        f = distance[2];
        if(f > b - error){
            MOTORDrive(1, speed - push);
            MOTORDrive(2, speed + push);
            printf("\rTurning left, Sensor is: %f", left);
            fflush(stdout);
        }
        else if(f < b + error){
            MOTORDrive(1, speed + push);
            MOTORDrive(2, speed - push);
            printf("\rTurning right, Sensor is: %f", left);
            fflush(stdout);
        }
        else{
            MOTORDrive(1, speed);
            MOTORDrive(2, speed);
            printf("\rTurning Straight, Sensor is: %f", left);
            fflush(stdout);
        }
        left = PSDGet(2);
    }
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);
}

void edge(int k){
    if(k == 1){
        VWTurn(-90, 20);
        VWWait();
        VWStraight(100, 50);
        VWWait();
        VWTurn(-90, 20);
        VWWait();
    }
    if(k == 2){
        VWTurn(90, 20);
        VWWait();
        VWStraight(100, 50);
        VWWait();
        VWTurn(90, 20);
        VWWait();

    }
}
void lawnmower(){
    VWSetPosition(x,y,phi); 
    VWTurn(-90, 200);
    VWWait();
    VWGetPosition(&x, &y, &phi);
    printf("\nx: %d, y: %d, phi: %d\n", x,y,phi);
    find_corner(limit,1);
    VWGetPosition(&x, &y, &phi);
    printf("\nx: %d, y: %d, phi: %d\n", x,y,phi);
    edge(1);
    find_corner(450,2);
    edge(2);
    find_corner(650,1);
    edge(1);
    find_corner(850,2);
    edge(2);
    find_corner(1050,1);
    edge(1);
    find_corner(1250,2);
    edge(2);

}

// USE VWGETPOSITION ALONG WITH PHI CONTROL INSTEAD OF BANGBANG CONTROL FOR DISTANACE
int main() {

    SIMSetRobot(0,1000,1500,1,0);
    LIDARSet(360, 0, 8);
    // SIMGetObject(1,1000,1000,0,0);
    straight();
    rotate_right();
    find_corner(limit, 1);
    lawnmower();
}