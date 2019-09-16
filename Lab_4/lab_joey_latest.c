//fix organise where i declare var
//fix astar? or is that it?

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

//for LCD coord and EyeSim coord
int TargetLCDx, TargetLCDy;
int LCDx,LCDy;
int EyeSx, EyeSy;

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
            //printf("k: %d. %d  \n",k,img2D[i][j]);
            k++;
        }
    }
}

#define DEPTH_LIMIT 8 //depth limit can actually go down to 2 for pixel (width =1) level boxes, but min 4 for whole no. centre point coords
#define MAX_QUADS 256
typedef struct Quadbox { 
    int xcoord;
    int ycoord;
    int width;
} 
Quadbox;
Quadbox blocked_quads[MAX_QUADS]; //declare quadbox var with length of 256 entries
Quadbox free_quads[MAX_QUADS]; //declare quadbox var with length of 256 entries

//should implement breakcondition in test allfree/allblock, but not needed
int block_quad_number = 0;
int free_quad_number = 0;
void quadtree(int x, int y, int width) { //opinion: removed depth limit, width detemrines if depth limit reached
    bool allFree= true;
    bool allBlocked= true;
    if (width >= DEPTH_LIMIT) { //DEPTH LIMIT NOT REACHED. //do tests
        for (int i = y; i < y+width; i++){ //run through each pixel for tests //    Note i being the rows going up y and j being the columns going up x
            for (int j = x; j < x+width; j++){
                if(img2D[i][j] ==1){ //blocked pixel exists, defs not allFree
                    allFree = false;
                }
                if(img2D[i][j] ==0){ //free pixel exists, defs not allBlocked
                    allBlocked = false;
                }
            }
        }
        if (allFree == true || allBlocked == true) { /*QUADBOX IS either ALL FREE OR ALL BLOCKED*/ 
            if (allFree ==true){    //ADD QUADBOX TO FREE ARRAY
                free_quads[free_quad_number].xcoord = x;
                free_quads[free_quad_number].ycoord = y;
                free_quads[free_quad_number].width = width;
                //printf(colour_green"%d. FreeQuad at x:%d y:%d width:%d \n",free_quad_number,x,y,width); printf(reset_colour);
                free_quad_number++; //move to next element
            }
            if (allBlocked ==true){    //ADD QUADBOX TO BLOCKED ARRAY
                blocked_quads[block_quad_number].xcoord = x;
                blocked_quads[block_quad_number].ycoord = y;
                blocked_quads[block_quad_number].width = width;
                //printf(colour_red"%d. BlockedQuad at x:%d y:%d width:%d \n",block_quad_number,x,y,width); printf(reset_colour);
                block_quad_number++; //move to next element

            }       
        } else { //SPLIT QUADBOX FURTHER 
            int width2 = width/2;
            quadtree(x, y, width2);
            quadtree(x + width2, y, width2);
            quadtree(x, y + width2, width2);
            quadtree(x + width2, y + width2, width2);
            /* // dylan's version of splitting, expand to see 
                Quadbox quads[4]; //split previous box into 4 quadboxes
                int x_mult[] = {0, 1, 0, 1}; //clever: top left, top right, bot left, bot right
                int y_mult[] = {0, 0, 1, 1}; //clever: top left, top right, bot left, bot right
                for (int i = 0; i< 4; i++) {
                    quads[i].width = width / 2;
                    quads[i].xcoord = x + quads[i].width * x_mult[i];
                    quads[i].ycoord = y + quads[i].width * y_mult[i];
                }
                //CALL QUADTREE FUNCTION RECURSIVELY ON EACH QUAD
                for (int i = 0; i< 4; i++) {
                    quadtree(quads[i].xcoord,quads[i].ycoord,quads[i].width);
                }
            */
        }
    } else { //REACHED DEPTH LIMIT, NO NEED TO DIVIDE ANYMORE ie. width < 4 (depth limit)
         //ADD QUADBOX TO FREE OR BLOCKED ARRAY
        for (int i = y; i < y+width; i++){ //run through each pixel for tests
            for (int j = x; j < x+width; j++){
                if(img2D[i][j] ==1){ //blocked pixel exists, defs not allFree
                    allFree = false;
                }
                if(img2D[i][j] ==0){ //free pixel exists, defs not allBlocked
                    allBlocked = false;
                }
            }
        }
        if (allFree == true || allBlocked == true) { /*QUADBOX IS either ALL FREE OR ALL BLOCKED*/ 
            if (allFree ==true){    //ADD QUADBOX TO FREE ARRAY
                free_quads[free_quad_number].xcoord = x;
                free_quads[free_quad_number].ycoord = y;
                free_quads[free_quad_number].width = width;
                //printf(colour_green"%d. FreeQuad at x:%d y:%d width:%d \n",free_quad_number,x,y,width); printf(reset_colour);
                free_quad_number++; //move to next element
            }
            if (allBlocked ==true){    //ADD QUADBOX TO BLOCKED ARRAY
                blocked_quads[block_quad_number].xcoord = x;
                blocked_quads[block_quad_number].ycoord = y;
                blocked_quads[block_quad_number].width = width;
                //printf(colour_red"%d. BlockedQuad at x:%d y:%d width:%d \n",block_quad_number,x,y,width); printf(reset_colour);
                block_quad_number++; //move to next element
            }       
        } 
    }
}

void DrawQuadboxes(){ 
    for (int i = 0; i <= free_quad_number;i++){ //draw GREEN free quads
        LCDArea(free_quads[i].xcoord,free_quads[i].ycoord, (free_quads[i].xcoord + free_quads[i].width),(free_quads[i].ycoord + free_quads[i].width),GREEN,0);
    } 
    for (int i = 0; i <= block_quad_number;i++){ //draw RED blocked quads
        LCDArea(blocked_quads[i].xcoord,blocked_quads[i].ycoord, (blocked_quads[i].xcoord + blocked_quads[i].width),(blocked_quads[i].ycoord + blocked_quads[i].width),RED,0);
    } 
}

#define MAX_POINTS 256
typedef struct Point {
    float ptx;
    float pty;
    int goaldist;
}
Point;
Point free_center_points[MAX_POINTS];

void PrintCenterCoords(){
    float center_x, center_y, width2;
    int x,y,width;
    for (int i = 0; i < free_quad_number;i++){
        x = free_quads[i].xcoord;
        y = free_quads[i].ycoord;
        width = free_quads[i].width;

        width2 = (float) free_quads[i].width /2;
        center_x = (float) free_quads[i].xcoord + width2;
        center_y = (float) free_quads[i].ycoord + width2;

        free_center_points[i].ptx = center_x;
        free_center_points[i].pty = center_y;
        free_center_points[i].goaldist = round(sqrt( pow(TargetLCDx-center_x,2)+pow(TargetLCDy-center_y,2) )); //dist to goal
        printf(colour_green"%d. FreeNode Center x:%f y:%f GoalDist:%d \n",i,center_x,center_y,free_center_points[i].goaldist); printf(reset_colour);
    }
}

void Convert_EyeSimCoord2ImgCoord(int x, int y){
    LCDx =  round( ((float)x/4000)*128) ;
    LCDy = round( ( (4000-(float)y)/4000)*128 );
}
void Convert_ImgCoord2EyeSimCoord(int LCDx, int LCDy){
    EyeSx =  round( ((float)LCDx/128)*4000) ;
    EyeSy = round( ( (128-(float)LCDy)/128)*4000 );
}

#define MAX_PATHS 5000
typedef struct Path {
    int nodeA; 
    float ax;
    float ay;
    int nodeB;
    float bx;
    float by;
    int pathdist;
} 
Path;
Path collision_free_paths[MAX_PATHS];

int colfreepath_number = 0;
void CopyToColFreePaths(int nodeA, float ax, float ay, int nodeB, float bx, float by){
    collision_free_paths[colfreepath_number].nodeA = nodeA;
    collision_free_paths[colfreepath_number].ax = ax;
    collision_free_paths[colfreepath_number].ay = ay;
    collision_free_paths[colfreepath_number].nodeB = nodeB;
    collision_free_paths[colfreepath_number].bx = bx;
    collision_free_paths[colfreepath_number].by = by;
    int pathdist = round(sqrt( pow(ax-bx,2)+pow(ay-by,2) )); //calc path length
    collision_free_paths[colfreepath_number].pathdist = pathdist;
    printf(colour_yellow"%d. ColFreePath node(%d) ax:%f ay:%f node(%d) bx:%f by:%f DIST:%d \n",colfreepath_number,nodeA, ax,ay,nodeB, bx,by,pathdist); printf(reset_colour);
    //LCDPrintf("%d ColFreePath ax:%f ay:%f bx:%f by:%f DIST:%d \n",colfreepath_number,ax,ay,bx,by,pathdist); // Print string and arguments on LCD
    colfreepath_number++ ;
}

void LineSqIntersections(){
    float Rx, Ry, Sx, Sy, Tx, Ty, Ux, Uy;
    float ax,ay,bx,by;
    float RFxy, SFxy, TFxy, UFxy;
    int no_intersection_count;

    for(int j = 0; j < free_quad_number;j++){ //free quad number = number of free center pt, cycle through paths
        ax = free_center_points[j].ptx;
        ay = free_center_points[j].pty;
        for(int k = 0; k < free_quad_number;k++){
            if (k != j) {
                bx = free_center_points[k].ptx;
                by = free_center_points[k].pty;
                no_intersection_count= 0;

                for(int i = 0; i < block_quad_number;i++){ //set RTSU pts, test all blocked boxes
                    Rx = (float) blocked_quads[i].xcoord;
                    Ry = (float) blocked_quads[i].ycoord;
                    Sx = (float) blocked_quads[i].xcoord + (float) blocked_quads[i].width;
                    Sy = (float) blocked_quads[i].ycoord;
                    Tx = (float) blocked_quads[i].xcoord;
                    Ty = (float) blocked_quads[i].ycoord + (float) blocked_quads[i].width;
                    Ux = (float) blocked_quads[i].xcoord + (float) blocked_quads[i].width;
                    Uy = (float) blocked_quads[i].ycoord + (float) blocked_quads[i].width;

                    RFxy = (by-ay)*Rx + (ax-bx)*Ry + (bx*ay-ax*by);
                    SFxy = (by-ay)*Sx + (ax-bx)*Sy + (bx*ay-ax*by);
                    TFxy = (by-ay)*Tx + (ax-bx)*Ty + (bx*ay-ax*by);
                    UFxy = (by-ay)*Tx + (ax-bx)*Ty + (bx*ay-ax*by);
                    if ((RFxy > 0 && SFxy > 0 && TFxy > 0 && UFxy > 0) || (RFxy < 0 && SFxy < 0 && TFxy < 0 && UFxy < 0) ){
                        no_intersection_count++; //line is above or below blocked box

                    } else if ( (ax>Sx && bx>Sx) || (ax<Tx && bx<Tx) || (ay>Sy && by>Sy) || (ay<Ty && by<Ty) ) {
                        no_intersection_count++;
                         //line is outside blocked box at left/right
                    }
                    if (no_intersection_count == block_quad_number-1){ //not intesecting with any blocked box
                        CopyToColFreePaths(j,ax,ay,k,bx,by);
                        break;
                    }
                }
            }
        }
    }
}

void DrawPaths(){
    for (int i = 0; i <= colfreepath_number;i++){ //draw GREEN free quads
        LCDLine(collision_free_paths[i].ax, collision_free_paths[i].ay, collision_free_paths[i].bx, collision_free_paths[i].by, BLUE); // Draw line
    }
}

// AStar Attempt
typedef struct NodeRecord {
    int node_number[MAX_PATHS]; //vector of node numbers used in path entry
    int totallength; //length of path entry
}
NodeRecord;
NodeRecord paths[MAX_PATHS];
NodeRecord minpath[1];

int paths_index = 0;
int minpath_nodeindex =0;

void Next(int currentNodeNo){
    int nextNodeNo;
    int mindist = 200;
    bool skip;
    int addtoDist;

    for(int i= 0;i <=colfreepath_number;i++){ //check all potential paths from current node, i index of collision_free_paths
        //add path length to next node + dist to goal at next node
        //break at min length (best first)
        //paths[paths_index].node_number[+] = currentNodeNo;

        if(collision_free_paths[i].nodeA ==currentNodeNo && collision_free_paths[i].nodeB != currentNodeNo){ //look at all colfreepaths from current node only and ignore path to self
            // Loop through those paths, check if next node is already in it
            skip = false;
            for (int j = 0; j <= minpath_nodeindex; j++) {
                if (minpath[0].node_number[j] == collision_free_paths[i].nodeB) {
                    skip = true;
                    break;   // break if node already in path
                }
            }
            if (!skip){ //ie if skip = false
                nextNodeNo = collision_free_paths[i].nodeB;
                if( (collision_free_paths[i].pathdist + free_center_points[nextNodeNo].goaldist) < mindist){
                    mindist = collision_free_paths[i].pathdist + free_center_points[nextNodeNo].goaldist ;
                    minpath[0].node_number[minpath_nodeindex+1] =  nextNodeNo; //add nextnode with min dist of path + goaldist, to end of minpath nodes
                    addtoDist = collision_free_paths[i].pathdist;
                }
                //ignore the rest
            }          
        }
    }
    minpath[0].totallength = minpath[0].totallength + addtoDist ;
    minpath_nodeindex++; //move to next element of minpath nodes
}

int goalnode= 34;
void AStar2(){
    //set start node
    minpath[0].node_number[0]= 0;
    minpath[0].totallength = 0;
    int currentNode;
    while(minpath[0].node_number[minpath_nodeindex] != goalnode){
        currentNode = minpath[0].node_number[minpath_nodeindex];
        Next(currentNode);
    }
    printf(colour_magenta"minpath nodes:"reset_colour);
    for (int printloop = 0; printloop <= minpath_nodeindex; printloop++){
        printf("%d  ",minpath[0].node_number[printloop]);
    }
    printf("totaldist pixels: %d \n",minpath[0].totallength);
    int totaldist_eyesim = ((float)minpath[0].totallength /128)*4000;
    printf("totaldist eyesim mm: %d \n",totaldist_eyesim);
}

void FollowPath(NodeRecord path){
    size_t numberof_pathnodes = sizeof(path.node_number)/sizeof(path.node_number[0]);
    printf("Number of pathnodes %zu \n",numberof_pathnodes);

    numberof_pathnodes = minpath_nodeindex;
    printf("Actual Number of pathnodes %zu \n",numberof_pathnodes);
    for (int i=0; i<=numberof_pathnodes;i++){
        int pnode = path.node_number[i];
        Convert_ImgCoord2EyeSimCoord(free_center_points[pnode].ptx, free_center_points[pnode].pty);
        if (pnode ==0 || pnode ==goalnode){
            DriveToNew(EyeSx,EyeSy);
            printf("Node %d reached \n",pnode);
        } else {
            DriveToNew(EyeSx+200,EyeSy+200);
            printf("Node %d reached \n",pnode);
        }
    }
}

void AStar(){
    int currentNodeNo =0; //free center pts
    int nextNodeNo;
    //int mindist = 200;
    //start node number = 0, goal at node 34 (last node)
    for(int i= 0;i <=colfreepath_number;i++){ //check all potential paths from current node, i index of collision_free_paths
        //add path length to next node + dist to goal at next node
        //break at min length (best first)
        paths[paths_index].node_number[0] = currentNodeNo;

        if(collision_free_paths[i].nodeA ==currentNodeNo && collision_free_paths[i].nodeB != currentNodeNo){ //look at all colfreepaths from current node only
            nextNodeNo = collision_free_paths[i].nodeB;
            paths[paths_index].node_number[1] =  nextNodeNo;
            paths[paths_index].totallength = collision_free_paths[i].pathdist + free_center_points[nextNodeNo].goaldist;

            if (paths[paths_index].totallength)
            paths_index++; //next entry for pathlist
        }
    //check next possible paths, add to shortest to paths,
    //break when goal node reached in last node_number and min dist found in, return that path_index

    }
    for (int printloop = 0; printloop < paths_index; printloop++){
        printf("%d. %d %d totaldist: %d \n",printloop,paths[printloop].node_number[0],paths[printloop].node_number[1],paths[printloop].totallength);
    }
}


int  main(){
    char *filename = "corner.pbm";
    //BYTE *img;
    read_pbm(filename, &img);
    LCDImageStart(0, 0, 128, 128);
    LCDImageBinary(img); //displays image on LCD (XQuartz)

    SIMSetRobot(0,px,py,0,0); //set initial pose based on sim grid
    VWSetPosition(px,py,90); //set up robot coord sys, match world coord sys
    CurrentPose(); 
    printf(colour_blue"Start pose: x %d , y %d , phi %d \n", cx,cy,cphi); printf(reset_colour);
    Tx =3500; Ty =500; //target bottom right corner
    printf(colour_magenta"Target Pose: Tx %d , Ty %d \n", Tx,Ty); printf(reset_colour);
    Convert_EyeSimCoord2ImgCoord(Tx,Ty);
    TargetLCDx = LCDx;
    TargetLCDy = LCDy;
    printf(colour_cyan"Target Pose: TLCDx %d , TLCDy %d \n", LCDx,LCDy); printf(reset_colour);

    vector2matrix();
    quadtree(0,0,128);
    PrintCenterCoords();
    LineSqIntersections(); //find col free paths
    DrawPaths();
    DrawQuadboxes(); //draw over paths

    //AStar();
    AStar2();
    FollowPath(minpath[0]); //3

    CurrentPose();
    printf(colour_blue"End pose: x %d , y %d , phi %d \n", cx,cy,cphi); printf(reset_colour);
    printf(colour_magenta"Target Pose: Tx %d , Ty %d \n", Tx,Ty); printf(reset_colour);
    printf("Press 'ENTER' to end program :D \n");
    getchar(); 
}