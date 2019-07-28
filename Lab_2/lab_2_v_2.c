#include <stdio.h>
#include "eyebot.h"
#include <time.h>
#include <math.h>

// https://www.cubic.org/docs/hermite.htm

/*
           +x
           |
           |
-y ---------------- +y
           |
           |
           -x
 */

// TO DO
// get this pls_move function working
// I should try angle control for a variety of inputs, see if it works for all

#define pi acos(-1.0)

int x; int y; int phi = 0;
float x_prev = 0; float y_prev = 0;
float sum = 0;
int steps = 10;

float phi_error;
float phi_to_point;
int speed = 0;
float push = 0.05;
float distance = 100;

float hermite[4][4] = {
    {2, -2, 1, 1},
    {-3, 3, -2, -1},
    {0, 0, 1, 0},
    {1, 0, 0, 0}
};
float pos[4][2] = {
    {0, 0}, // P1 -> Start Point (x,y)
    {300, 100}, // P2 -> End Point (x,y)
    {10, 0}, // T1 -> Start Tangent (x,y)
    {10, 0} // T2 -> End Tangent (x,y)
} ;
float t = 0;
float s[4][1] = {
    {0}, // pow(t, 3)
    {0}, // pow(t, 2)
    {0}, // t
    {0}  // 1
};
float output[4][1] = {
    {0}, 
    {0}, 
    {0}, 
    {0}  
};
float vector[2][1] = {
    {0},
    {0}
};


void move(float x, float y){
    // function to move to the desired vector locations
    VWDrive(x, y, speed);
    VWWait();
}

void pls_move(int x_p, int y_p){
    while(distance > 3){
        VWGetPosition(&x, &y, &phi);

        phi_to_point = atan2(y_p - y,x_p - x)*180/pi;
        phi_error = phi_to_point - phi;
        distance = sqrt((y_p - y)*(y_p - y) + (x_p - x)*(x_p - x) );

        printf("phi_point: %f \t", phi_to_point);
        printf("phi_error: %f\t", phi_error);
        printf("phi: %d \t", phi);
        printf("dist: %f \t", distance);
        printf("x: %d, y: %d\t", x, y);


        if(phi_error > 3){
            // turn left
            MOTORDrive(1, speed - phi_error*push);
            MOTORDrive(2, speed + phi_error*push);
            usleep(1000);
            printf("Left \n");
        }
        else if(phi_error < -3){
            // turn right
            MOTORDrive(1, speed + phi_error*push);
            MOTORDrive(2, speed - phi_error*push);
            usleep(1000);
            printf("Right \n");
        }
        else{
            MOTORDrive(1, speed + 3);
            MOTORDrive(2, speed + 3);
            usleep(1000);
            printf("straight \n");
        }
    }
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);
    distance = 100;
}

void follow_curve(int x_p, int y_p){
    VWGetPosition(&x, &y, &phi);
    phi_to_point = atan2(y_p - y,x_p - x)*180/pi;

    VWTurn(phi_to_point- phi, 30);
    VWWait();

    distance = sqrt((y_p - y)*(y_p - y) + (x_p - x)*(x_p - x) );
    VWStraight(distance, 50);
    VWWait();

    printf("\nx: %d, y: %d, phi: %d, phi to point: %f, distance: %f\n", x, y, phi, phi_to_point, distance);
}

void spline(){
    for(t=0;t< steps; t++){
        printf("element %f\n", t);
        
        // determining the s-vector (interpolation-point)
        for(int k = 0; k < 4; k++){
            s[k][1] = pow(t/steps, 3-k);
            // printf("%f\n", pow(t, 3-k));
        }

        // multiplying the s-vector with the hermite vector
        // results in the output vector
        for(int r = 0; r < 4; r++){
            sum = 0;
            for(int c = 0; c < 4; c++){
                sum += s[c][1]*hermite[c][r]; // r and c actually swapped around
            }
        output[r][1] = sum; 
        }

        // multiplying the output of above by the start and end 
        // parameters for the hermite curve
        for(int k = 0; k < 2; k++){
            sum = 0;
            for(int d = 0; d < 4; d ++){
                sum += output[d][1] * pos[d][k]; 
            }
            vector[k][1] = sum;
        }

        // printing out the discrete vectors 
        for(int i = 0; i < 2; i++){
            printf("vector %i:  %f\n",i, vector[i][1]);
        }

        // move(vector[0][1] - x_prev, vector[1][1]- y_prev);
        x_prev = vector[0][1];
        y_prev = vector[1][1];

        // pls_move(vector[0][1], vector[1][1]);
        // VWDrive(x_prev, y_prev, 50);
        // VWWait();

        follow_curve(x_prev, y_prev);
    }
}



int main() {
    SIMSetRobot(0,1000,1000,1,0);
    VWSetPosition(x, y, phi);
    VWGetPosition(&x, &y, &phi);
    printf("\nx: %d, y: %d, phi: %d\n", x, y, phi);
    spline();
    return 0;
}