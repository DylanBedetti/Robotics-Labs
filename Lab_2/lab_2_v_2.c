#include <stdio.h>
#include "eyebot.h"
#include <time.h>
#include <math.h>

// https://www.cubic.org/docs/hermite.htm
float sum = 0;
int steps = 20;
float hermite[4][4] = {
    {2, -2, 1, 1},
    {-3, 3, -2, -1},
    {0, 0, 1, 0},
    {1, 0, 0, 0}
};
float pos[4][2] = {
    {0, 0}, // P1 -> Start Point (x,y)
    {100, 100}, // P2 -> End Point (x,y)
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

void test_spline(){
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

        // multiplying the output of above by the stand and end 
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
            printf("vector %f\n", vector[i][1]);
        }
    }
}


int main() {
    // SIMSetRobot(0,1000,1000,1,0);
    test_spline();
    return 0;
}