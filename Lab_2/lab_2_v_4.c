#include <stdio.h>
#include "eyebot.h"
#include <time.h>
#include <math.h>

#define pi acos(-1.0)

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



int x; int y; int phi = 0; // VWGetPosition variables
float x_val = 0; float y_val = 0; // recording spline x and y coordinates
float sum = 0; // used for spline calculation
int steps = 15; // number of discrete steps of spline!
float phi_error; float distance; 
int speed = 50; // base speed

// txt file stuff
int points[20][2];
int i = 0;

float hermite[4][4] = {
    {2, -2, 1, 1},
    {-3, 3, -2, -1},
    {0, 0, 1, 0},
    {1, 0, 0, 0}
};
float pos[4][2] = {
    {0, 0}, // P1 -> Start Point (x,y)
    {400, 1000}, // P2 -> End Point (x,y)
    {0, 0}, // T1 -> Start Tangent (x,y)
    {0, 0} // T2 -> End Tangent (x,y)
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

void follow_points(int x_p, int y_p){
    // control function for each point on spline 
    printf("\n------- FOLLOWING POINTS ----------\n");

    // Updating position, phi error and distance
    VWGetPosition(&x, &y, &phi);
    phi_error = atan2(y_p - y,x_p - x)*180/pi - phi;
    distance = sqrt((y_p - y)*(y_p - y) + (x_p - x)*(x_p - x) );

    printf("START: phi error: %f\tdistance: %f\tphi: %d\tx: %d\ty: %d\n", phi_error, distance, phi, x, y);

    // using basic curve function as control
    VWCurve(distance, phi_error, speed);
    VWWait();

    // again updating position, phi error and distance
    VWGetPosition(&x, &y, &phi);
    phi_error = atan2(y_p - y,x_p - x)*180/pi - phi;
    distance = sqrt((y_p - y)*(y_p - y) + (x_p - x)*(x_p - x) );
    printf("END: phi error: %f\tdistance: %f\tphi: %d\tx: %d\ty: %d\n", phi_error, distance, phi, x, y);
}
void spline(){
    for(t=0;t <= steps; t++){
        
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
        printf("\n\nNEW SPLINE POINTS (x, y) element %.1f\n", t);
        for(int i = 0; i < 2; i++){
            printf("%f\t", vector[i][1]);
        }
        printf("\n");

        x_val = vector[0][1];
        y_val = vector[1][1];

        // follow_curve(x_val, y_val);
        follow_points(x_val, y_val);
    }
}

void print_array(){
    // function for printing out pos array
    for (int k = 0; k < 4; k++){
            for (int j = 0; j < 2; j++){
                printf("%.0f\t", pos[k][j]);
            }
            printf("\n");
        }
}

int main() {
    // set robot position
    SIMSetRobot(0,1800,200,1,0); // robot id, x pos, y pos, z pos, angle

    // Setting position and updating position parameters x, y and phi
    VWSetPosition(x, y, phi);
    VWGetPosition(&x, &y, &phi);

    printf("\n\n STARTING PROGRAM");
    printf("\nx: %d, y: %d, phi: %d\n\n", x, y, phi);

    // looping through text file and calling spline();
    FILE* file = fopen("way.txt", "r");
    while (fscanf(file,"%d %d", &points[i][0], &points[i][1]) == 2) {
        printf("\nPoint: %d\n", i);
        if (i == 0){
            // printf("x:%d y:%d\n", points[i][0], points[i][1]);
            pos[1][0] = points[i][0];
            pos[1][1] = points[i][1];
            print_array();
            spline();
            
        } else{
    	    // printf("x:%d y:%d\n", points[i][0] - points[i-1][0], points[i][1]-points[i-1][1]);
            pos[0][0] = points[i-1][0];
            pos[0][1] = points[i-1][1];
            
            pos[1][0] = points[i][0];
            pos[1][1] = points[i][1];

            print_array();
            spline();
        }
	i++;
    }
    pos[0][0] = pos[1][0];
    pos[0][1] = pos[1][1];

    pos[1][0] = 0;
    pos[1][1] = 0;
    
    printf("\nPoint: %d\n", i);
    print_array();

    spline();

    fclose(file);
    return 0;
}