#include <stdio.h>
#include "eyebot.h"
#include <time.h>
#include <math.h>

#define pi acos(-1.0)
#define GREEEN   "\033[32m"
#define RESET   "\033[0m"
#define REED     "\033[31m" 

int points[20][2];
int i = 0;
int x_pos, y_pos, phi = 0;
int x_point, y_point;
int speed = 50;
 
void Rotate(int x, int y){
    float angle_diff;
    printf("\n---------Rotating---------\n\n");

    VWGetPosition(&x_pos, &y_pos, &phi);

    angle_diff = atan2(y - y_pos,x - x_pos)*180/pi - phi;

    if (angle_diff > 180){angle_diff = angle_diff - 360;}
    else if (angle_diff < -180){angle_diff = angle_diff + 360;}

    printf("turning angle: %f\n", angle_diff);
    VWTurn(angle_diff, speed);
    VWWait();

    MOTORDrive(1, 0);
    MOTORDrive(2, 0);
}

void Drive_until_close(int x, int y){
    float dist_diff;
    printf("\n---------Driving---------\n\n");

    VWGetPosition(&x_pos, &y_pos, &phi);
    dist_diff = sqrt((y_pos - y)*(y_pos - y) + (x_pos - x)*(x_pos - x) );

    while (dist_diff > 100){
        VWGetPosition(&x_pos, &y_pos, &phi);
        dist_diff = sqrt((y_pos - y)*(y_pos - y) + (x_pos - x)*(x_pos - x) );

        MOTORDrive(1, 50);
        MOTORDrive(2, 50);
    }

    MOTORDrive(1, 0);
    MOTORDrive(2, 0);

}

void Drive_end(int x, int y){
    float dist_diff; float x_acc; float y_acc;
    printf("\n---------Driving Final---------\n\n");

    VWGetPosition(&x_pos, &y_pos, &phi);
    dist_diff = sqrt((y_pos - y)*(y_pos - y) + (x_pos - x)*(x_pos - x) );

    VWStraight(dist_diff, speed);
    VWWait();

    printf("\t\t=======SCORE======\n");
    VWGetPosition(&x_pos, &y_pos, &phi);

    x_acc = ((x_pos - x)/(float)x_pos)*100.0;
    y_acc = ((y_pos - y)/(float)y_pos)*100.0;
    // printf("xpos - x: %d\n", (x_pos - x));
    // printf("divide by xpos: %d\n", (x_pos - x)/x_pos);

    printf("x_pos: %d, y_pos: %d, x: %d, y: %d\n", x_pos, y_pos, x, y);

    if (x_acc < 5 && y_acc < 5){
        printf( GREEEN "DIFFERENCE: X:%f%%, Y:%f%%\n", x_acc, y_acc); printf(RESET);
    } else {
        printf( REED "DIFFERENCE: X:%f%%, Y:%f%%\n", x_acc, y_acc); printf(RESET);
    }

}


int main(){
    // set robot position
    // robot id, x pos, y pos, z pos, angle
    SIMSetRobot(0,1800,200,1,0); 

    printf("\n\n\n \t\t STARTING BOIIII \n\n\n");

    // Set position parameters
    VWSetPosition(x_pos, y_pos, phi);
    VWGetPosition(&x_pos, &y_pos, &phi);

    // opening text file
    FILE* file = fopen("way.txt", "r");
    while (fscanf(file,"%d %d", &points[i][0], &points[i][1]) == 2) {
    	printf("POINT x:%d y:%d\n", points[i][0], points[i][1]);
        x_point = points[i][0]; y_point = points[i][1];

        Rotate(x_point, y_point);

        Drive_until_close(x_point, y_point);

        Rotate(x_point, y_point);

        Drive_end(x_point, y_point);
	    i++;
	}

    Rotate(0, 0);
    Drive_until_close(0, 0);
    Rotate(0, 0);
    Drive_end(0, 0);
    Rotate(100, 0);

    fclose(file);

}