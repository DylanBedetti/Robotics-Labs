#include "eyebot.h"
#include <stdio.h>

#define RIGHT 3
#define LEFT 2

int leftsensor, centersensor, rightsensor;
int number_of_sweeps = 0;
int centersensor_val1, centersensor_val2;
int alpha = 25;
int turn_rate = 50;
int desired_distance = 150;
int drive_length;
int angle = 0;
int angle_rec;
int centersensor_min = 4000;
int corner = 0;
float Kp = 0.001;
float Ki = 0.001;
float Kd = 0.001;
float error;
float error_old = 0;
float error_old2 = 0;
float output;
float output_old = 0;
int t = 0;

int right, left;


int initDrive();
void drive_around_box_ccw();
void drive_around_box_cw();


int main() {
    int init = initDrive();
    if(init == RIGHT) drive_around_box_cw();
    else if(init == LEFT) drive_around_box_ccw();
    return 0;
}

int initDrive() {
    left = PSDGet(2);
    right = PSDGet(3);
    if (left < right) {
        return RIGHT;
    }
    else if (left > right) {
        return LEFT;
    }
    return 0;
}


void drive_around_box_ccw(){ //will need to continually check if right sensor is too close or far

    while(corner < 4){
        centersensor = PSDGet(1);
        rightsensor = PSDGet(3);
        error = rightsensor - desired_distance;
        output = output_old + Kp*(error - error_old)+ Ki*(error + error_old)/2 +Kd*(error-2*error_old+error_old2);

        if (centersensor < 300){
            LCDClear();
            LCDPrintf("Taking corner...");
            VWCurve(280, 100, turn_rate);
            VWWait();

            centersensor = PSDGet(1);
            rightsensor = PSDGet(3);
            error = rightsensor - desired_distance;
            output = output_old + Kp*(error - error_old)+ Ki*(error + error_old)/2 +Kd*(error-2*error_old+error_old2);
            corner++;
        }

        if (rightsensor >= 153){
            LCDClear();
            LCDPrintf("turning right %lf  error: %lf  center: %i" , output, error, centersensor);
            VWDrive(3, -3, 100);


        }
        else if (rightsensor < 147){
            LCDClear();
            LCDPrintf("turning left %lf error: %lf  center: %i", output, error, centersensor);
            VWDrive(3, 3, 100);
        }
        else {
            VWStraight(3, 100);
        }

        output_old = output;
        error_old2 = error_old;
        error_old = error;
    }

}

void drive_around_box_cw(){ //will need to continually check if right sensor is too close or far

    while(corner < 4){
        centersensor = PSDGet(1);
        leftsensor = PSDGet(2);
        error = leftsensor - desired_distance;
        output = output_old + Kp*(error - error_old)+ Ki*(error + error_old)/2 +Kd*(error-2*error_old+error_old2);

        if (centersensor < 300){
            LCDClear();
            LCDPrintf("Taking corner...");
            VWCurve(280, -100, turn_rate);
            VWWait();

            centersensor = PSDGet(1);
            leftsensor = PSDGet(3);
            error = leftsensor - desired_distance;
            output = output_old + Kp*(error - error_old)+ Ki*(error + error_old)/2 +Kd*(error-2*error_old+error_old2);
            corner++;
        }

        if (leftsensor >= 153){
            LCDClear();
            LCDPrintf("turning left %lf  error: %lf  center: %i" , output, error, centersensor);
            VWDrive(3, 3, 100);


        }
        else if (leftsensor < 147){
            LCDClear();
            LCDPrintf("turning right %lf error: %lf  center: %i", output, error, centersensor);
            VWDrive(3, -3, 100);
        }
        else {
            VWStraight(3, 100);
        }

        output_old = output;
        error_old2 = error_old;
        error_old = error;
    }

}
