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
// TO DO
// Pretty sure I need to look at the VWGetPos and Start and End alpha's as they are most likely what is stuffing it up!


int x_pos; int y_pos; int phi = 0;
float steps = 30; float rot_factor = 1; float lin_factor = 1; int travel_speed = 90;
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

    // Debug 
    printf("angle_diff: %f, rot_new: %f, dist_diff: %f, lin_new: %f\n", angle_diff, rot_new, dist_diff, lin_new);

    // limiting angle turns
    if (rot_new > 180){rot_new = rot_new - 360;}
    if (rot_new < -180){rot_new = rot_new + 360;}

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

void SplineDrive(int x, int y){
    // Updating Current position - need to work this shit out
    VWGetPosition(&x_pos, &y_pos, &phi);
    float alpha_end = atan2(y - y_pos,x - x_pos)*180/pi;
    printf("alpha_end: %f\n", alpha_end);
    printf("current phi: %d", phi);

    // defining all variables for hermite curve
    float len = sqrt(pow(x, 2) + pow(y, 2));
    float Dpx = len*cos(0); float Dpy = len*sin(0);
    float Dp1x = len*cos(0); float Dp1y = len*sin(0);
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
    }
    // stop vehicle
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);
}

int main(){
    printf("\n\n------------------STARTING SIMULATION------------------\n\n");
    
    // set robot position
    // robot id, x pos, y pos, z pos, angle
    SIMSetRobot(0,1800,200,1,0); 

    // Set position parameters
    VWSetPosition(x_pos, y_pos, phi);
    VWGetPosition(&x_pos, &y_pos, &phi);

    // open and loop through txt file
    FILE* file = fopen("way.txt", "r");
    while (fscanf(file,"%d %d", &points[i][0], &points[i][1]) == 2) {
    	printf("Point x:%d y:%d\n", points[i][0], points[i][1]);
        
        SplineDrive(points[i][0],points[i][1]);

	    i++;
	}
    fclose(file);

    // SplineDrive(600,600,180,0);

    return 0;
}