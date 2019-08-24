#include <stdio.h>
#include "eyebot.h"
#include <time.h>
#include <math.h>

// https://www.cubic.org/docs/hermite.htm

/*
           +x
           |
           |            + <--
+y ---------------- -y       |
           |               --
           |
           -x
 */

// TO DO
// get this pls_move function working
// I should try angle control for a variety of inputs, see if it works for all

#define pi acos(-1.0)

int x; int y; int phi = 0; // VWGetPosition variables
float x_val = 0; float y_val = 0; // recording spline x and y coordinates
float sum = 0;
int steps = 15; // number of discrete steps of spline!

float phi_error; float phi_to_point;
int speed = 30; float push = 1; float distance = 100;
float turn_rate;

float hermite[4][4] = {
    {2, -2, 1, 1},
    {-3, 3, -2, -1},
    {0, 0, 1, 0},
    {1, 0, 0, 0}
};
float pos[4][2] = {
    {0, 0}, // P1 -> Start Point (x,y)
    {300, 500}, // P2 -> End Point (x,y)
    {40, 0}, // T1 -> Start Tangent (x,y)
    {0, 200} // T2 -> End Tangent (x,y)
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


void follow_curve(int x_p, int y_p){
    printf("\n\n ------ FOLLOWING CURVE ------- \n\n");
    VWGetPosition(&x, &y, &phi);
    phi_error = atan2(y_p - y,x_p - x)*180/pi - phi;
    distance = sqrt((y_p - y)*(y_p - y) + (x_p - x)*(x_p - x) );


    while (distance > 2){
        // need to consider when phi error is negative
        turn_rate = fmin(abs(phi_error)*push*fmax(-distance + 5, 1), 10);
        // should speed be altered by distance?
        
        

        if(phi_error > 1){
            // turn left
            MOTORDrive(1, speed - turn_rate);
            MOTORDrive(2, speed + turn_rate);
        }
        else if(phi_error < -1){
            // turn right
            MOTORDrive(1, speed + turn_rate);
            MOTORDrive(2, speed - turn_rate);
        }
        else{
            VWStraight(10, 30);
        }

        VWGetPosition(&x, &y, &phi);
        distance = sqrt((y_p - y)*(y_p - y) + (x_p - x)*(x_p - x) );
        phi_error = atan2(y_p - y,x_p - x)*180/pi - phi;

        printf("\r distance: %f, phi_error: %f, turn rate: %f, x: %d, y: %d", distance, phi_error,turn_rate,x, y);
        fflush(stdout);
    }
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);

    VWGetPosition(&x, &y, &phi);
    printf("\n\nx: %d, y: %d, phi: %d, phi error: %f, distance: %f\n\n", x, y, phi, phi_error , distance);
}

void spline(){
    for(t=0;t <= steps; t++){
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

        x_val = vector[0][1];
        y_val = vector[1][1];

        // follow_curve(x_val, y_val);
        follow_points(x_val, y_val);
    }
}



int main() {
    // set robot position
    SIMSetRobot(0,1000,1000,1,0); // robot id, x pos, y pos, z pos, angle

    // Setting position and updating position parameters x, y and phi
    VWSetPosition(x, y, phi);
    VWGetPosition(&x, &y, &phi);

    printf("\n\n STARTING PROGRAM \n\n");
    printf("\nx: %d, y: %d, phi: %d\n", x, y, phi);


    spline();
    return 0;
}