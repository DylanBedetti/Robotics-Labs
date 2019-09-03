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

// x and y goal is around 3500, 3500??

int distance[8];
int straight_speed = 50, turn_speed = 50;
int x_pos, y_pos, phi = 0;
float angle_diff, dist_diff;
int x_goal = 3500, y_goal = 3500;

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
    while(distance[FRONT] > 300 && distance[FRONT_LEFT] > 300 && distance[FRONT_RIGHT] > 300){
        MOTORDrive(1, straight_speed);
        MOTORDrive(2, straight_speed);

        LIDARGet(distance);
        for (int i = 0; i < 8; i ++){printf("%d\t", distance[i]);}

        // update_angle();
        // printf("angle: %f", angle_diff);
        printf("\n");
    }
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);

    VWTurn(90, turn_speed);
    VWWait();
}

void follow_thicc_object(){


    while (dist[0] > 1500){
        if (dist[side] > dist[side+2]){
            MOTORDrive(LEFT, cruising_speed + j*push);
            MOTORDrive(RIGHT,  cruising_speed - j*push);
        }
        else {
            MOTORDrive(LEFT, cruising_speed - j*push);
            MOTORDrive(RIGHT,  cruising_speed + j*push);
        }
        update();
        height_control();
    }
}
int main(){
    

    SIMSetRobot(0,500,500,1,90); // !! this rotate plane so x-y are positive top left

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