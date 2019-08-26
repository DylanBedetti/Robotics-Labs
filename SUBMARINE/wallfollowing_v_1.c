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


int main()
{ 
    SIMSetRobot(0,10000,10000,1,30);
    while (true){
        MOTORDrive(FRONT, -speed);
        MOTORDrive(BACK,  -speed);
        printf("FRONT: %d \t FRONT LEFT: %d \t FRONT RIGHT: %d \t DOWN: %d \n", PSDGet(PSD_FRONT),PSDGet(PSD_FRONT_LEFT) ,PSDGet(PSD_FRONT_RIGHT) ,PSDGet(PSD_DOWN));
    }

    // VWWait();
    printf("done");

}