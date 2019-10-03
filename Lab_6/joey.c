/* Joey Koh 21506379 and James Mok GENG5508 2019 Sem 2 */

//how to produce positive infinity, replace 9999. how to reverse array?

#include <stdio.h> //basic 
#include <stdlib.h> //exit functions
#include <string.h>
#include <math.h> //math
#include "eyebot.h" //eyebot RoBIOS-7 library functions

#define BUFFSIZE 1024
//#define MAXLEN 20
#define MY_FILE "grid.txt" //can also change to grid.txt

//print colours
    #define colour_red "\x1b[31m"
    #define colour_green "\x1b[32m"
    #define colour_yellow "\x1b[33m"
    #define colour_blue "\x1b[34m"
    #define colour_magenta "\x1b[35m"
    #define colour_cyan "\x1b[36m"
    #define reset_colour "\x1b[0m"

//Initial Parameters and Constants
int lin_spd = 50; // mm/sec
int ang_spd = 50; //deg/sec
float increment = 0.03; //for 'u' loop
//int location_tolerance = 120; //dist for location point tolerance

int px = 100; //initial x pos
int py = 100; //initial y pos
int phi = 0; //starting angle deg, orientation
int cx, cy, cphi; //for current pose
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


#define MAX_NODES 100
typedef struct Point {
    int ptx;
    int pty;
    int goaldist;
}
Point;
Point nodes[MAX_NODES];

double adjacency[MAX_NODES][MAX_NODES]; //for storing paths between nodes,
int countnode = 0;

void init() {
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            if (i != j){
                adjacency[i][j] = -1.0; //-1 represents no path between nodes
            }
        }
        Point new_node = { .ptx = 0, .pty = 0};
        nodes[i] = new_node; //set all nodes to 0
    }
}

void checkInit(){
    printf("Number of nodes: %d\n",MAX_NODES);
    for (int i = 0; i < MAX_NODES; i++){
        for (int j = 0; j < MAX_NODES; j++){
            printf(colour_cyan "%2.2f",adjacency[i][j]);
        }
        printf("\n");
    }
}

void readFile() {
    FILE* file = fopen(MY_FILE, "r"); //open in read mode
    char line[BUFFSIZE]; //buffsize 1024
    int i = 0;

    while (fgets(line, sizeof(line), file)) {//for each line
        char *token;
        token = strtok(line, " ");
        nodes[i].ptx = atoi(token);
        while (token != NULL) {//for each token
            token = strtok(NULL, " ");
            countnode ++;
            if (token != NULL){
                nodes[i].pty = atoi(token);
                i ++;
                break;
            }
            
        }
    }
    fclose(file);

    FILE* rfile = fopen(MY_FILE, "r"); //open in read mode
    char rline[BUFFSIZE]; //buffsize 1024
    int j = 0;
    while (fgets(rline, sizeof(rline), rfile)) {//for each line
        char *token;
        token = strtok(rline, " ");

        int cycle3 = 0;
        while (token != NULL) {//for each token
            token = strtok(NULL, " ");
            if (token != NULL && cycle3 == 0){
                cycle3 ++;
            }
            else if (token != NULL && cycle3 == 1){
                int xa = nodes[j].ptx;
                int xb = nodes[atoi(token)-1].ptx;
                int ya = nodes[j].pty;
                int yb = nodes[atoi(token)-1].pty;

                adjacency[j][ atoi(token)-1 ] = round( sqrt( pow(xa - xb, 2) + pow(ya - yb, 2) ) );
            } 
        }
        j++;
    }
    fclose(rfile);
}

void checkValues(){
    printf("Number of nodes: %d\n",countnode);
    for (int i = 0; i < countnode; i++){
        for (int j = 0; j < countnode; j++){
            printf(colour_cyan "%9.2f",adjacency[i][j]);
        }
        printf(reset_colour"\n");
    }
}

int goalnode;
void assignGoalDist(){
    goalnode = countnode - 1;
    for (int i =0; i <countnode;i++){
        nodes[i].goaldist = round(sqrt( pow(nodes[i].ptx - nodes[goalnode].ptx,2) - pow(nodes[i].ptx - nodes[goalnode].ptx,2)));
    }
    Tx = nodes[goalnode].ptx; 
    Ty = nodes[goalnode].pty; //update targers
}
// AStar Attempt
#define MAX_PATHS 1000

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
    int mindist = 9999;
    int addtoDist;
    bool skip;

    for(int i= 0;i < countnode;i++){
        // Loop through those paths, check if next node is already in it
        skip = false;
        for (int j = 0; j <= minpath_nodeindex; j++) {
            if (minpath[0].node_number[j] == i) {
                skip = true;
                break;   // break if node already in path
            }
        }
        if( !skip && i != currentNodeNo && adjacency[currentNodeNo][i] != -1){ //if not current node and adjacency shows path
            if (adjacency[currentNodeNo][i] + nodes[i].goaldist  < mindist){
                mindist = adjacency[currentNodeNo][i] + nodes[i].goaldist;
                minpath[0].node_number[minpath_nodeindex+1] = i;
                addtoDist = adjacency[currentNodeNo][i];
            }
            //ignore the rest
        }
    }
    minpath[0].totallength += addtoDist ;
    minpath_nodeindex++; //move to next element of minpath[0] nodes
}

void AStar(){
    //set start node
    minpath[0].node_number[0]= 0;
    minpath[0].totallength = 0;
    int currentNode;
    printf("goalnode: %d\n",goalnode); printf(reset_colour);
    while(minpath[0].node_number[minpath_nodeindex] != goalnode){
        currentNode = minpath[0].node_number[minpath_nodeindex];
        Next(currentNode);
    }
    printf(colour_magenta"minpath[0] nodes:"reset_colour);
    for (int printloop = 0; printloop <= minpath_nodeindex; printloop++){
        printf("%d  ",minpath[0].node_number[printloop]);
    }
    printf("totaldist eyesim mm: %d \n",minpath[0].totallength);
}

typedef struct InOutScores{
    bool openset;
    bool closedset;
    double g_score; //graph score: via node paths
    double h_score; //heuristic function, est cost. via direct air-line euclidian distance
    double f_score; // g + h
    int parent; //parent node index to this node
}
InOutScores;

InOutScores ascores[MAX_NODES];

void astar2() {
    //InOutScores ascores[countnode];
    bool goalreachable = false;

    for (int i = 0; i<MAX_NODES;i++){ //initialise
        InOutScores initial_score = { .openset = false, .closedset = false, .g_score = 9999, .f_score = 9999};
        ascores[i] = initial_score; 
    }
    ascores[0].g_score = 0;
    ascores[0].h_score = 0;
    ascores[0].f_score = 0;
    ascores[0].openset = true;
    int checkOpens = checkOpenSet();

    while (checkOpens >= 1) {
        int currentNode = lowestFScore(); //node in openset with the LOWEST f_score
        if (currentNode == goalnode) { //finished, grab your path and drive via the parent array
            goalreachable = true;
            printf("Goal path found!\n");
            break;
        }
        ascores[currentNode].openset = False; //openset.remove(current);
        ascores[currentNode].closedset = True; //closedset.add(current);
        
        for(int neighbour= 0;neighbour < countnode; neighbour++){
            if (ascores[neighbour].closedset == true){
                continue; //continue if already explored (ignore this neighbour)
            }
            if(neighbour != currentNode && adjacency[currentNode][neighbour] != -1){ //if not current node and adjacency shows path
                double tentative_g_score = ascores[currentNode].g_score + adjacency[currentNode][neighbour]; //tentative_g_score = g_score[current] + graph_dist(current, neighbour);
                if (tentative_g_score < ascores[neighbour].g_score) { //tentative path is better than recorded path
                    ascores[neighbour].parent = currentNode; //make the current node the parent of neighbouring node being looked at
                    ascores[neighbour].g_score = tentative_g_score;
                    ascores[neighbour].f_score = tentative_g_score + nodes[neighbour].goaldist;

                    if ( ascores[neighbour].openset == false) { //if neighbour NOT in openset
                        ascores[neighbour].openset = true; // openset.add(neighbour);
                    }
                }
            }//ignore the rest (no path to neigbour or looking at current node)
        } //endfor
        checkOpens = checkOpenSet();
        //printf("Press enter to cont\n");
        //getchar();
    } //endwhile
    if (goalreachable == false) {//if we get here, openset is empty, but goal was not reached.
        printf("Goal unreachable! \n");
    }
}

int checkOpenSet(){ //return number_opensets
    int number_open = 0;
    for (int i=0;i<countnode;i++){
        if (ascores[i].openset){ //if true
            number_open ++;
        }
    }
    return number_open;
}

int lowestFScore(){ //return lowest F score node index
    int lowestFScoreNode;
    int lowestFScore = 9999;
    for (int i=0;i<countnode;i++){
        if (ascores[i].f_score < lowestFScore && ascores[i].openset == true ){ //if true
            //printf("node's %d fscore: %f\n",i,ascores[i].f_score);
            lowestFScore = ascores[i].f_score;
            lowestFScoreNode = i;
        }
    }
    return lowestFScoreNode;
}

NodeRecord parentpath; //keep track of nodes prev (nodes numbers and totallength)
int parpath_index;

void getParentPath(){
    int currentNode = goalnode;
    parentpath.node_number[0] = goalnode;
    parentpath.totallength = 0;
    parpath_index = 1;
    while (currentNode != 0){ //not equal to start node
        //printf("currentnode in parentpath: %d\n enter to cont\n",currentNode);
        //getchar();
        parentpath.node_number[parpath_index] = ascores[currentNode].parent ;
        parpath_index ++;
        parentpath.totallength += round(adjacency[ascores[currentNode].parent][currentNode]); //add path dist from parent to current node
        currentNode = ascores[currentNode].parent;
    }
    printf(colour_green"A-Star pathv2 at: \n"reset_colour);
    int no_ofpathnodes = parpath_index;
    for (int i = no_ofpathnodes-1; i >= 0; i--){ //drive in reverse path order
        printf("%d  ",parentpath.node_number[i]);
    }
    printf("\nA-Star Path distance: %d\n", parentpath.totallength); printf(reset_colour);
}

void FollowPath(NodeRecord path){
    int numberof_pathnodes = parpath_index;
    printf("Actual Number of pathnodes %d \n",numberof_pathnodes);

    for (int i = numberof_pathnodes-1; i >= 0; i--){ //drive in reverse path order
        int pnode = path.node_number[i];

        printf(colour_green"drive to x: %d y: %d\n",nodes[pnode].ptx,nodes[pnode].pty); printf(reset_colour);
        DriveToNew(nodes[pnode].ptx,nodes[pnode].pty);
        printf("Node %d reached \n",pnode);
    }
}

int  main(){
    SIMSetRobot(0,px,py,0,0); //set initial pose based on sim grid
    VWSetPosition(px,py,90); //set up robot coord sys, match world coord sys
    CurrentPose(); 
    printf(colour_blue"Start pose: x %d , y %d , phi %d \n", cx,cy,cphi); printf(reset_colour);
    
    init();
    //checkInit();
    readFile();
    checkValues();
    assignGoalDist();
    //AStar();
    //FollowPath(minpath[0]);
    
    astar2();
    getParentPath();
    FollowPath(parentpath);
   
    CurrentPose();
    //printf(colour_blue"End pose: x %d , y %d , phi %d \n", cx,cy,cphi); printf(reset_colour);
    printf(colour_magenta"Target Pose: Tx %d , Ty %d \n", Tx,Ty); printf(reset_colour);
    printf("Press 'ENTER' to end program :D \n");
    getchar(); 
}