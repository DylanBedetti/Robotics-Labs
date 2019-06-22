#include <stdio.h>
#include "eyebot.h"
#include <time.h>
#include <math.h>

#define pi acos(-1.0)

int x; int y; int phi; 
float phi_error;
float phi_to_point;
int speed = 30;
float push = 0.4;
float distance = 100;


void angle_control(int x_p, int y_p){
    while(distance > 10){
        VWGetPosition(&x, &y, &phi);

        phi_to_point = atan2(y_p - y,x_p - x)*180/pi;

        printf("phi_to_point: %f \t", phi_to_point);

        phi_error = phi_to_point - phi;
        printf("phi_error: %f\t", phi_error);

        distance = sqrt((y_p - y)*(y_p - y) + (x_p - x)*(x_p - x) );
        printf("distance: %f \n", distance);


        if(phi_error > 0){
            // turn left
            MOTORDrive(1, speed - phi_error*push);
            MOTORDrive(2, speed + phi_error*push);
        }
        else if(phi_error < 0){
            // turn left
            MOTORDrive(1, speed + phi_error*push);
            MOTORDrive(2, speed - phi_error*push);
        }
        else{
            VWStraight(10, 30);
        }
    }
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);

}




int main() {
    SIMSetRobot(0,1000,1000,1,0);
    VWSetPosition(x, y, phi);
    VWGetPosition(&x, &y, &phi);

    angle_control(300, 300); // x , y
    return 0;
}