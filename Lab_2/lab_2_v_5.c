#include <stdio.h>
#include "eyebot.h"
#include <time.h>
#include <math.h>

#define pi acos(-1.0)

/*
           +x
           |
           |            + <--
+y ---------------- -y       |
           |               --
           |
           -x
 */

int x_pos; int y_pos; int phi = 0; // VWGetPosition variables
float steps = 15;

void SplineDrive(int x, int y, int alpha_end, int alpha_start){
    // x units forward, y units left, alpha counter clockwise in degrees

    float len = sqrt(pow(x, 2) + pow(y, 2));
    // float Dpx = len*cos(alpha_start); float Dpy = len*sin(alpha_start);
    float Dpx = len; float Dpy = 0;
    float Dp1x = len*cos(alpha_end); float Dp1y = len*sin(alpha_end);
    float px = 0; float py = 0; float p1x = x; float p1y = y;
    float u2; float u3; float H1; float H2; float H3; float H4; float spline_x; float spline_y;
    float sum_x; float sum_y;

    printf("\n\nSPLINE DRIVE STARTING \nlen: %f, Dpx: %f, Dpy: %f, Dp1x: %f, Dp1y: %f, alpha_end: %d\n\n", len, Dpx, Dpy, Dp1x, Dp1y, alpha_end);

    for(float u = 0; u <= 1; u += 1/steps){
        printf("Step %f\n", u*steps);

        u2 = u*u; u3 = u2*u;

        H1 = 2*u3 - 3*u2 + 1;
        H2 = -2*u3 + 3*u2;
        H3 = u3 - 2*u2 + u;
        H4 = u3 - u2;

        spline_x = H1*px + H2*p1x * H3*Dpx + H4*Dp1x;
        spline_y = H1*py + H2*p1y * H3*Dpy + H4*Dp1y;

        sum_x += spline_x; sum_y += spline_y;
        

        printf("u: %f, u2: %f, u3: %f\n", u, u2, u3);
        printf("H1: %f, H2: %f, H3: %f, H4: %f\n", H1, H2, H3, H4);
        printf("px: %f, py: %f, p1x: %f, p1y %f\n", px, py, p1x, p1y);
        printf("Dpx: %f, Dpy: %f, Dp1x: %f, Dp1y: %f\n", Dpx, Dpy, Dp1x, Dp1y);
        printf("spline: X: %f, Y: %f\n", spline_x, spline_y);
        printf("sum: X: %f, Y: %f\n\n", sum_x, sum_y); 
        // printf("%.1f,%.1f,", spline_x, spline_y);

    }
}

int main(){
    // set robot position
    // robot id, x pos, y pos, z pos, angle
    // SIMSetRobot(0,1000,1000,1,0); 

    // Set position parameters
    // VWSetPosition(x_pos, y_pos, phi);
    // VWGetPosition(&x_pos, &y_pos, &phi);

    SplineDrive(20,30,0,0);
}