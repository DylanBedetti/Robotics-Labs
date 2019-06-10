#include "eyebot.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int leftsensor, centersensor, rightsensor;
int number_of_sweeps;
int centersensor_val1, centersensor_val2;
int alpha = 25;
int turn_rate = 100;
int desired_distance = 150;
int drive_length;
int angle = 0;
int angle_rec;
int centersensor_min = 4000;
int corner = 0;
float Kp = 0.001;
float Ki = 0.0001;
float Kd = 0.0001;
float error;
float error_old = 0;
float error_old2 = 0;
float output;
float output_old = 0;
int t = 0;

void scan();
void sweep();
void drive_to_edge();
void drive_around_box();


int main() {
    VWStraight(20, 200);
    usleep(10000);
    //scan();
    //sweep();
    //drive_to_edge();
    drive_around_box();

    return 0;
}


void drive_around_box(){ //will need to continually check if right sensor is too close or far

    while(corner < 4){
        centersensor = PSDGet(1);
        rightsensor = PSDGet(3);
        error = rightsensor - desired_distance;

        if (centersensor < 160){
            LCDClear();
            LCDPrintf("taking corner ");
            VWCurve(230, 100, 100);
            VWWait();

            centersensor = PSDGet(1);
            rightsensor = PSDGet(3);
            error = rightsensor - desired_distance;
            corner++;
        }

        if (rightsensor > 160){
            LCDClear();
            LCDPrintf("turning right %lf  error: %lf  center: %i" , output, error, centersensor);
            VWCurve(20, -20, 200);
            usleep(10000);
            //VWWait();


        }
        else if (rightsensor < 140){
            LCDClear();
            LCDPrintf("turning left %lf error: %lf  center: %i", output, error, centersensor);
            VWCurve(20, 20, 200);
            usleep(10000);
            //VWWait();
        }
        else {
            VWStraight(20, 200);
            usleep(10000);
            //VWWait();
        }

    }



}

void drive_to_edge(){
    centersensor = PSDGet(1);
    desired_distance = 100;
    drive_length = centersensor - desired_distance;

    if (drive_length > 50) {
        VWStraight(drive_length - 50 + 25,200);
        VWWait();
        VWCurve(2*50, 105, turn_rate);
        VWWait();
    }
    else {
        VWCurve(1.5 * drive_length, 90, turn_rate);
        VWWait();
    }
    /*
    while (t < 50){
        centersensor = PSDGet(1);
        rightsensor = PSDGet(3);
        error = rightsensor - desired_distance;
        output = output_old + Kp*(error - error_old)+ Ki*(error + error_old)/2 +Kd*(error-2*error_old+error_old2);
        if (output <=  1 && output >= 0){
            output = 1.1;
        }
        if (output <=  0 && output >= -1){
            output = -1.1;
        }


        if (rightsensor >= 160){
            LCDClear();
            LCDPrintf("turning right %lf  error: %lf  center: %i time: %i" , output, error, centersensor, t);
            VWDrive(3, -3, 100);
           // VWWait();


        }
        if (rightsensor < 140){
            LCDClear();
            LCDPrintf("turning left %lf error: %lf  center: %i time: %i", output, error, centersensor, t);
            VWDrive(3, 3, 100);
            //VWWait();
        }
        else{
            VWStraight(3, 100);
            //VWWait();
        }
        output_old = output;
        error_old2 = error_old;
        error_old = error;
        t++;
        */
    //}

}

void sweep(){
    while(number_of_sweeps < 5){
        VWTurn(1*alpha,turn_rate);
        VWWait();
        centersensor_val1 = PSDGet(1);
        VWTurn(-2*alpha, turn_rate);
        VWWait();
        centersensor_val2 = PSDGet(1);

        if (centersensor_val1 < centersensor_val2){
            VWTurn(2*alpha, turn_rate);
            VWWait();
        }
        number_of_sweeps++;
        alpha = alpha - 3;
        LCDClear();
        LCDPrintf("number of sweeps done: %i  alpha: %i ", number_of_sweeps, alpha);
        //VWStraight(10, 50);
        //VWWait();




    }
}


void scan(){
    centersensor = PSDGet(1);
    leftsensor = PSDGet(2);
    rightsensor = PSDGet(3);

    if (centersensor < leftsensor && rightsensor){
        //do nothing
        LCDClear();
        LCDPrintf("center sensor closest");
        return;
    }

    if (leftsensor < rightsensor){
        VWTurn(90, 100); //+ve turn is to the left
        VWWait();
        LCDClear();
        LCDPrintf("Left sensor closest");
        return;
    }

    if (rightsensor < centersensor){
        VWTurn(-90, 100);
        VWWait();
        LCDClear();
        LCDPrintf("Right sensor closest");
    }

}
