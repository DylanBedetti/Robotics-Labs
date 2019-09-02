#include <stdio.h>
#include "eyebot.h"
#include <time.h>
#include <math.h>

#define pi acos(-1.0)
#define GREEEN   "\033[32m"
#define RESET   "\033[0m"
#define REED     "\033[31m"   

/*
           +x
           |
           |            + <--
+y ---------------- -y       |
           |               --
           |
           -x
 */

int x_pos, y_pos, phi = 0; // VWGetPosition variables
float steps = 20; float rot_factor = 1; float lin_factor = 1;
int travel_speed = 60;
int points[20][2]; int i = 0;

void Drive_Control(float x, float y, float u){
    float angle_diff; float rot_new; float dist_diff; float lin_new; float x_acc; float y_acc;
    
    printf("\n---------Driving---------\n\n");
    VWGetPosition(&x_pos, &y_pos, &phi);

    // Updating angle and dist diff
    angle_diff = atan2(y - y_pos,x - x_pos)*180/pi - phi;
    rot_new = rot_factor * angle_diff;

    dist_diff = sqrt((y_pos - y)*(y_pos - y) + (x_pos - x)*(x_pos - x) );
    lin_new = lin_factor * dist_diff; //(u + 0.5)

    // limiting angle turns
    if (rot_new > 180){rot_new = rot_new - 360;}
    if (rot_new < -180){rot_new = rot_new + 360;}

    // Debug 
    printf("angle_diff: %f, rot_new: %f, dist_diff: %f, lin_new: %f\n", angle_diff, rot_new, dist_diff, lin_new);


    // vehicle control
    VWCurve(lin_new, rot_new, travel_speed);
    VWWait();

    // Debug
    VWGetPosition(&x_pos, &y_pos, &phi);
    printf("END POSITION: X: %d, Y: %d, phi: %d\n", x_pos, y_pos, phi);

    // Debug - WITH COLOURS THOOO
    x_acc = ((x_pos - x)/x_pos)*100; y_acc = ((y_pos - y)/y_pos)*100;
    if (abs(x_acc) < 5 && abs(y_acc) < 5){
        printf( GREEEN "ACCURACY: X:%f%%, Y:%f%%\n", x_acc, y_acc); printf(RESET);
    } else {
        printf( REED "ACCURACY: X:%f%%, Y:%f%%\n", x_acc, y_acc); printf(RESET);
    }
    
}

void SplineDrive(int x, int y, int alpha_end, int alpha_start){ 
    VWGetPosition(&x_pos, &y_pos, &phi);

    // x units forward, y units left, alpha counter clockwise in degrees
    float len = sqrt(pow(x, 2) + pow(y, 2));
    float Dpx = len*cos(alpha_start); float Dpy = len*sin(alpha_start);
    float Dp1x = len*cos(alpha_end); float Dp1y = len*sin(alpha_end);
    float px = x_pos; float py = y_pos; float p1x = x; float p1y = y;
    float u2; float u3; float H1; float H2; float H3; float H4; float spline_x; float spline_y;


    for(float u = 0; u <= 1; u += 1/steps){
        printf("\n\nStep %.0f\n", u*steps);

        u2 = u*u; u3 = u2*u;

        H1 = 2*u3 - 3*u2 + 1;
        H2 = -2*u3 + 3*u2;
        H3 = u3 - 2*u2 + u;
        H4 = u3 - u2;

        spline_x = H1*px + H2*p1x + H3*Dpx + H4*Dp1x;
        spline_y = H1*py + H2*p1y + H3*Dpy + H4*Dp1y;

        printf("spline: X: %f, Y: %f\n", spline_x, spline_y);
        Drive_Control(spline_x, spline_y, u);
        
        // Debug
        // printf("%.1f,%.1f,", spline_x, spline_y);
    }
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);
}

int main(){
    float alpha_end; 
    // set robot position
    // robot id, x pos, y pos, z pos, angle
    SIMSetRobot(0,1800,200,1,0); 

    // Set position parameters
    VWSetPosition(x_pos, y_pos, phi);
    VWGetPosition(&x_pos, &y_pos, &phi);

    // opening text file
    FILE* file = fopen("way.txt", "r");
    while (fscanf(file,"%d %d", &points[i][0], &points[i][1]) == 2) {
        fscanf(file, "%d %d", &points[i+1][0], &points[i+1][1]);

    	printf("\n\n---------POINT %d:  x:%d y:%d---------\n\n", i, points[i][0], points[i][1]);
        
        VWGetPosition(&x_pos, &y_pos, &phi);

        // need to figure out end angle 
        alpha_end = atan2(points[i+1][1] - points[i][1],points[i+1][0] - points[i][0])*180/pi;// - phi;

        // if (alpha_end > 180){alpha_end = alpha_end - 360;}
        // if (alpha_end < -180){alpha_end = alpha_end + 360;}
        // printf("atan2: x: %d, y: %d \n", (points[i+1][0] - points[i][0]),(points[i+1][1] - points[i][1]));
        alpha_end = -alpha_end - 90;
        // phi = -phi - 90;
        printf("alpha_end: %f, phi: %d\n", alpha_end, phi);
        SplineDrive(points[i][0],points[i][1], alpha_end, phi);
        // if (i == 0){
        //     return 0;
        // }

	    i++;
    }
	
    fclose(file);

    //SplineDrive(300,600,180,0);

    return 0;
}