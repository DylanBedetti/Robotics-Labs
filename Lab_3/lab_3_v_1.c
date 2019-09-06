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
int straight_speed = 50, turn_speed = 50;
int push = 30;
int x_pos, y_pos, phi = 0;
float angle_diff, dist_diff;
int x_goal = 3500, y_goal = 3500;
int start_x_pos, start_y_pos;
float k;

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
    while(distance[FRONT] > 200 && distance[FRONT_LEFT] > 200 && distance[FRONT_RIGHT] > 300){
        MOTORDrive(1, straight_speed);
        MOTORDrive(2, straight_speed);

        LIDARGet(distance);
        //for (int i = 0; i < 8; i ++){printf("%d\t", distance[i]);}

        // update_angle();
        // printf("angle: %f", angle_diff);
        // printf("\n");
    }
    MOTORDrive(1, 0);
    MOTORDrive(2, 0);

    VWTurn(90, turn_speed);
    VWWait();
}

void follow_thicc_object(){
    printf("FOLLOWING THICC OBJECT\n\n");
    // hit point
    VWGetPosition(&x_pos, &y_pos, &phi);
    start_x_pos = x_pos; start_y_pos = y_pos;
    printf("start_x_pos: %d, start_y_pos: %d\n\n", start_x_pos, start_y_pos);

    VWStraight(50, 100);
    VWWait();
    VWGetPosition(&x_pos, &y_pos, &phi);

    // LIDARSet(360, 0, 360);  // range, tilt, points
    // int distance[360];
    // LIDARGet(distance);

    while (x_pos != start_x_pos && y_pos != start_y_pos){
        // if (distance[FRONT_RIGHT] > distance[BACK_RIGHT] && (distance[FRONT_RIGHT] < 250 || distance[RIGHT] < 250 || distance[BACK_RIGHT] < 250)){
        //     MOTORDrive(LEFT_WHEEL, straight_speed + push);
        //     MOTORDrive(RIGHT_WHEEL,  straight_speed - push);
        //     printf("\rTurning Left\t xpos: %d \t ypos: %d \t phi: %d \t FRONT_RIGHT: %d \t BACK_RIGHT: %d", x_pos, y_pos, phi, distance[FRONT_RIGHT], distance[BACK_RIGHT]);
        //     fflush(stdout);
        // }
        // else if (distance[FRONT_RIGHT] < distance[BACK_RIGHT] && (distance[FRONT_RIGHT] < 250 || distance[RIGHT] < 250 || distance[BACK_RIGHT] < 250)){
        //     MOTORDrive(LEFT_WHEEL, straight_speed - push);
        //     MOTORDrive(RIGHT_WHEEL,  straight_speed + push);
        //     printf("\rTurning Right\t xpos: %d \t ypos: %d \t phi: %d \t FRONT_RIGHT: %d \t BACK_RIGHT: %d", x_pos, y_pos, phi, distance[FRONT_RIGHT], distance[BACK_RIGHT]);
        //     fflush(stdout);
        // }
        // else {
        //     MOTORDrive(LEFT_WHEEL, straight_speed + push);
        //     MOTORDrive(RIGHT_WHEEL,  straight_speed - push);
        //     printf("\rTurning Left\t xpos: %d \t ypos: %d \t phi: %d \t FRONT_RIGHT: %d \t BACK_RIGHT: %d", x_pos, y_pos, phi, distance[FRONT_RIGHT], distance[BACK_RIGHT]);
        //     fflush(stdout);
        // }

        if (distance[FRONT_RIGHT] > distance[BACK_RIGHT]){
            k = (distance[FRONT_RIGHT] - distance[BACK_RIGHT]) / 500.0;
            MOTORDrive(LEFT_WHEEL, straight_speed + push*MIN(k, 2));
            MOTORDrive(RIGHT_WHEEL,  straight_speed - push*MIN(k, 2));
            printf("\rTurning Left, xpos: %d, ypos: %d, phi: %d, FRONT_RIGHT: %d, BACK_RIGHT: %d, k: %f ", x_pos, y_pos, phi, distance[FRONT_RIGHT], distance[BACK_RIGHT],k);
            fflush(stdout);
        }
        else {
            k = (distance[BACK_RIGHT] - distance[FRONT_RIGHT])/ 500.0;
            MOTORDrive(LEFT_WHEEL, straight_speed - push*MIN(k, 2));
            MOTORDrive(RIGHT_WHEEL,  straight_speed + push*MIN(k, 2));
            printf("\rTurning Right, xpos: %d, ypos: %d, phi: %d, FRONT_RIGHT: %d, BACK_RIGHT: %d, k:%f", x_pos, y_pos, phi, distance[FRONT_RIGHT], distance[BACK_RIGHT],k);
            fflush(stdout);
        }


        VWGetPosition(&x_pos, &y_pos, &phi);
        LIDARGet(distance);

    }
    printf("Done!\n");
    MOTORDrive(LEFT_WHEEL, 0);
    MOTORDrive(RIGHT_WHEEL,  0);

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