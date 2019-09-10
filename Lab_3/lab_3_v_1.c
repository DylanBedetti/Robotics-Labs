#include <stdio.h>
#include "eyebot.h"
#include <time.h>
#include <math.h>

#define pi acos(-1.0)
#define GREEEN   "\033[32m"
#define RESET   "\033[0m"
#define REED     "\033[31m" 

#define BACK_LEFT 0
#define LEFT 1
#define FRONT_LEFT 2
#define FRONT 3
#define FRONT_RIGHT 4
#define RIGHT 5
#define BACK_RIGHT 6
#define BACK 7

#define LEFT_WHEEL 1
#define RIGHT_WHEEL 2

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

// x and y goal is around 3500, 3500??

int distance[8];
int straight_speed = 100, turn_speed = 50;
int push = 30;
int x_pos, y_pos, phi = 0;
float angle_diff, dist_diff;
int x_goal = 3000, y_goal = -3000;
int start_x_pos, start_y_pos;
float k;
int wall_dist = 400;

void update_angle(){
    VWGetPosition(&x_pos, &y_pos, &phi);
    angle_diff = atan2(y_goal - y_pos,x_goal - x_pos)*180/pi - phi;

    if (angle_diff > 180){angle_diff = angle_diff - 360;}
    else if (angle_diff < -180){angle_diff = angle_diff + 360;}
}

void update_goal_dist(){
    VWGetPosition(&x_pos, &y_pos, &phi);
    dist_diff = sqrt((y_pos - y_goal)*(y_pos - y_goal) + (x_pos - x_goal)*(x_pos - x_goal) );
}

void striaght(){
    printf("\nGOING STRAIGHT AND TURNING\n");
    while(distance[FRONT] > wall_dist && distance[FRONT_LEFT] > wall_dist && distance[FRONT_RIGHT] > wall_dist){
        MOTORDrive(1, straight_speed);
        MOTORDrive(2, straight_speed);

        LIDARGet(distance);
    }
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);

    printf("finished going straight\n");
}

void follow_thicc_object(){
    printf("FOLLOWING THICC OBJECT\n\n");

    update_goal_dist();
    LIDARGet(distance);

    while (dist_diff > 50){
        if (distance[FRONT_RIGHT] < wall_dist || distance[FRONT] < wall_dist){
            printf("turning left\n");
            // MOTORDrive(LEFT_WHEEL, 20);
            // MOTORDrive(RIGHT_WHEEL, 50);
            VWCurve(100,40,200);

        }
        else {

            update_angle();
            VWCurve(100,angle_diff,200);
            printf("chasing end point\n");
        }
        LIDARGet(distance);
        update_goal_dist();
        printf("dist_diff: %f, angle_diff: %f\n\n", dist_diff, angle_diff);
    }
    
    printf("Done!\n");
    MOTORDrive(LEFT_WHEEL, 0);
    MOTORDrive(RIGHT_WHEEL,  0);

}
int main(){

    SIMSetRobot(0,500,500,1, 0); // !! this rotate plane so x-y are positive top left

    VWSetPosition(x_pos, y_pos, phi);
    VWGetPosition(&x_pos, &y_pos, &phi);

    LIDARSet(360, 0, 8);  // range, tilt, points
    LIDARGet(distance);

    for (int i = 0; i < 8; i ++){printf("Lidar value %d: %d,\n", i, distance[i]);}

    update_angle();
    VWTurn(angle_diff, turn_speed);
    VWWait();

    striaght();

    follow_thicc_object();

}