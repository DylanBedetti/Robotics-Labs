#include "eyebot.h"

// Thruster IDs.
#define LEFT   1
#define FRONT  2
#define RIGHT  3
#define BACK   4

// PSD direction
#define PSD_FRONT       1
#define PSD_FRONT_LEFT  2
#define PSD_FRONT_RIGHT 3
#define PSD_BACK_LEFT   4
#define PSD_BACK_RIGHT  5
#define PSD_DOWN        6 

const int speed = 100;
const int wall_dist = 1000;
const int cruising_speed = 80;
const int push = 10;
int dist[6] = {0,0,0,0,0,0}; 
int side;
int j;
int x; int y; int phi = 0; 

void update(){
    for(int i = 0; i < 6; i++){
        dist[i] = PSDGet(i+1);
    }
    printf("FRONT: %d \t FRONT LEFT: %d \t FRONT RIGHT: %d \t DOWN: %d \t BACK LEFT: %d \t BACK RIGHT: %d\n", dist[0], dist[1], dist[2], dist[5], dist[3], dist[4]);

}

void height_control(){
    if (PSDGet(PSD_DOWN) > 1000){
            MOTORDrive(FRONT, -speed/2);
            MOTORDrive(BACK,  -speed/2);
        } else {
            MOTORDrive(FRONT, speed/2);
            MOTORDrive(BACK,  speed/2);
        }
}

void straight(){
    while (dist[0] > wall_dist && dist[1] > wall_dist && dist[2] > wall_dist){
        MOTORDrive(LEFT, speed);
        MOTORDrive(RIGHT,  speed);

        //printf("FRONT: %d \t FRONT LEFT: %d \t FRONT RIGHT: %d \t DOWN: %d \t BACK LEFT: %d \t BACK RIGHT: %d\n", dist[0], dist[1], dist[2], dist[5], dist[3], dist[4]);

        height_control();
        update();
    }

    printf("\n\n FINISHED STRAIGHT \n\n");
    MOTORDrive(LEFT, 0);
    MOTORDrive(RIGHT,  0);

    if (dist[1] < dist[2]){
        // left is closer than right
        side = 1;
        j = -1;
    }
    else {
        // right is close than left
        side = 2;
        j = 1;
    }
}

void rotate(){
    printf("\nSTARTING ROTATE \n");
    if (side == 1){
        printf("rotating right\n");
        while (dist[1] < dist[3]){
            MOTORDrive(LEFT, speed);
            MOTORDrive(RIGHT,  -speed);
            update();
            height_control();
        }
    } else{
        printf("rotating left\n");
        while (dist[2] < dist[4]){
            MOTORDrive(LEFT, -speed);
            MOTORDrive(RIGHT,  speed);
            update();
            height_control();
        }
    }
    MOTORDrive(LEFT, 0);
    MOTORDrive(RIGHT,  0);

}

void follow_wall(){
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

    MOTORDrive(LEFT, j*-push);
    MOTORDrive(RIGHT,  j*push);
    sleep(3);

}

int main()
{ 
    SIMSetRobot(0,10000,20000,1000,250);
    update();

    printf("\n\n STARTING \n\n");

    straight();

    rotate();

    follow_wall();

    printf("\n\n DONE \n\n");

    return 0;

}