#include <stdio.h> //basic 
#include <stdlib.h> //exit functions
#include "eyebot.h" //eyebot RoBIOS-7 library functions
#include <math.h> //math
#include "image.c" //the image code given

//print colours
    #define colour_red "\x1b[31m"
    #define colour_green "\x1b[32m"
    #define colour_yellow "\x1b[33m"
    #define colour_blue "\x1b[34m"
    #define colour_magenta "\x1b[35m"
    #define colour_cyan "\x1b[36m"
    #define reset_colour "\x1b[0m"

//Initial Parameters and Constants
int lin_spd = 60; // mm/sec
int ang_spd = 50; //deg/sec
float increment = 0.03; //for 'u' loop
//int location_tolerance = 120; //dist for location point tolerance

int px = 500; //initial x pos
int py = 3500; //initial y pos
int phi = 0; //starting angle deg, orientation
int cx, cy, cphi; //for current pose
//int pre_x, pre_y; //pre_alpha; //for input raw values
int Tx, Ty; //target x y

//pointers
int * point_cx = &cx;
int * point_cy = &cy;
int * point_cphi = &cphi;

//For driving
int driving_dist;
float driving_ang;
float driving_ang_actual; //compensate for current orientation

//sense
int sensors[3] = {0,0,0};
#define mid 0
#define left 1
#define right 2
void sense() {
    sensors[mid]= PSDGet(1);
    sensors[left]= PSDGet(2);
    sensors[right]= PSDGet(3);
    //sensors[mid,left,right] = {PSDGet(1), PSDGet(2), PSDGet(3)}; //help why doesn't work
}

//Get Current Pose
void CurrentPose(){
    VWGetPosition(point_cx, point_cy, point_cphi);
}

//Drive Function
void DriveToNew(int Nx,int Ny){
    VWGetPosition(point_cx, point_cy, point_cphi); //update current pose
    driving_dist = sqrt( pow(Nx-cx,2) + pow(Ny-cy,2) );
    driving_ang = atan2(Ny-cy, Nx-cx); //in radians
    driving_ang = driving_ang * 180 / M_PI; //in degrees
    driving_ang_actual = driving_ang - cphi;
    if (driving_ang_actual > 180){
        driving_ang_actual= driving_ang_actual - 360;
    } else if (driving_ang_actual < -180){
        driving_ang_actual= driving_ang_actual + 360;
    }
    //printf("Set drive dist: %d , ang %f \n", driving_dist,driving_ang_actual);
    //fflush(stdout); // flushing stdout as it is buffered
    VWTurn(driving_ang_actual, ang_spd);           // Turn on spot, angle [degrees], ang. speed [degrees/s]  
    VWWait();
    VWStraight(driving_dist,lin_spd);        // Drive straight, dist [mm], lin. speed [mm/s]
    VWWait();
    VWGetPosition(point_cx, point_cy, point_cphi); //update current pose
}

BYTE *img;
BYTE img2D[128][128]; //actual number of rows and columns
void vector2matrix(){
    int k = 0; //indexing starts from 0
    for(int i = 0; i <128 ;i++){ //0 to 127
        //printf("ROW  i number %d \n",i);
        for(int j = 0; j <128 ; j++){ //0 to 127
            //printf("COL j number %d \n",j);
            img2D[i][j]= img[(i*128+j)];
            printf("k: %d. %d  \n",k,img2D[i][j]);
            k++;
        }
    }
}

#define DEPTH_LIMIT 4 
#define MAX_QUADS 256  //4^Depth_limit

typedef struct Quadbox { 
    int xcoord;              // x,y is top left of quadbox
    int ycoord;
    int width;               // how long quadbox is 
} Quadbox;

Quadbox blocked_quads[MAX_QUADS];
Quadbox free_quads[MAX_QUADS];

void quadtree(int x, int y, int width, int depth_limit) {
        if (depth_limit > 0) {
            bool free_quads, blocked_quads = true;
            for(int i=x; i<x+width;i++){
                for(int=y; J<y+width; j++){
                    if(img2D[i][j]){
                        free_quads = false;
                    }
                    else{
                        blocked_quads = false;
                    }
                }
            }
            else { //SPLIT QUADBOX FURTHER Quadbox quads[4];
                int x_mult[] = {0, 1, 0, 1};
                int y_mult[] = {0, 0, 1, 1};
                for (int i = 0; i< 4; i++) {
                    quads[i].width = width / 2;
                    quads[i].xcoord = x + quads[i].width * x_mult[i];
                    quads[i].ycoord = y + quads[i].width * y_mult[i];
                    }
                //CALL QUADTREE FUNCTION RECURSIVELY ON EACH QUAD
            }
    } else { //REACHED DEPTH LIMIT, NO NEED TO DIVIDE ANYMORE
      
    }
}                                                                                                                           } }
//ADD QUADBOX TO FREE OR BLOCKED ARRAY

int  main(){\
    char *filename = "corner.pbm";
    //BYTE *img;
    read_pbm(filename, &img);
    LCDImageStart(0, 0, 128, 128);
    LCDImageBinary(img); //displays image on LCD (XQuartz)

    SIMSetRobot(0,px,py,1,0); //set initial pose based on sim grid
    VWSetPosition(px,py,90); //set up robot coord sys, match world coord sys
    CurrentPose(); 
    printf(colour_blue"Start pose: x %d , y %d , phi %d \n", cx,cy,cphi); printf(reset_colour);
    Tx =3500; Ty =500; //target bottom right corner
    printf(colour_magenta"Target Pose: Tx %d , Ty %d \n", Tx,Ty); printf(reset_colour);

    vector2matrix();


    CurrentPose();
    printf(colour_blue"End pose: x %d , y %d , phi %d \n", cx,cy,cphi); printf(reset_colour);
    printf(colour_magenta"Target Pose: Tx %d , Ty %d \n", Tx,Ty); printf(reset_colour);
    printf("Press 'ENTER' to end program :D \n");
    getchar(); 
}