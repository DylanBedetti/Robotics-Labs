#include <stdio.h>
#include "eyebot.h"
#include <time.h>

float left, front, right;
int limit = 250;
int left_prev, x, y, phi;


void straight(){
    // function that will go straight and check sensors until they are below limit
    left = PSDGet(2);
    front = PSDGet(1);
    right = PSDGet(3);

    while(left > limit && front > limit && right > limit){
        VWStraight(10, 100);
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
    if(k == 1){
        while(front > limit){
            // MOTORDrive(1, 10);
            // MOTORDrive(2, 10);
            usleep(100);

            if(left < l){
                MOTORDrive(1, 32);
                MOTORDrive(2, 28);
                usleep(100);
                printf("\rTurning Right, Sensor is: %f", left);
                fflush(stdout);

            }
            if(left > l){
                MOTORDrive(1, 28);
                MOTORDrive(2, 32);
                usleep(100);
                printf("\rTurning Left: Sensor is: %f", left);
                fflush(stdout);
            }
            left = PSDGet(2);
            front = PSDGet(1);
        }
    }
    if(k == 2){
        while(front > limit){
            // MOTORDrive(1, 10);
            // MOTORDrive(2, 10);
            usleep(100);

            if(right < l){
                MOTORDrive(2, 32);
                MOTORDrive(1, 28);
                usleep(100);
                printf("\rTurning Right, Sensor is: %f", right);
                fflush(stdout);

            }
            if(right > l){
                MOTORDrive(2, 28);
                MOTORDrive(1, 32);
                usleep(100);
                printf("\rTurning Left: Sensor is: %f", right);
                fflush(stdout);
            }
            left = PSDGet(2);
            front = PSDGet(1);
            right = PSDGet(3);
        }
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
    VWTurn(-90, 100);
    VWWait();
    VWGetPosition(&x, &y, &phi);
    printf("\nx: %d, y: %d, phi: %d\n", x,y,phi);
    find_corner(limit,1);
    VWGetPosition(&x, &y, &phi);
    printf("\nx: %d, y: %d, phi: %d\n", x,y,phi);
    edge(1);
    find_corner(350,2);
    edge(2);
    find_corner(450,1);
    edge(1);
    find_corner(550,2);
    edge(2);
    find_corner(650,1);
    edge(1);
    find_corner(750,2);
    edge(2);

}

// USE VWGETPOSITION ALONG WITH PHI CONTROL INSTEAD OF BANGBANG CONTROL FOR DISTANACE
int main() {

    SIMSetRobot(0,1000,1000,1,0);
    // SIMGetObject(1,1000,1000,0,0);
    straight();
    rotate_right();
    find_corner(limit, 1);
    lawnmower();
}