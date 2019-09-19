#include "eyebot.h"
#include "image.c"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define LINE_MAX 255
#define DEPTH_LIMIT 8 //in terms of pixels!
#define MAX_QUADS 256
#define PI 3.1415
BYTE *img;
BYTE img2D[128][128];

char *filename = "corner.pbm";
int counter = 0;



typedef struct Quadbox {
    int xcoord;
    int ycoord;
    int width;
} Quadbox;

typedef struct Freebox {
    int x;
    int y;
    int d;
} Freebox;

typedef struct Path {
    float x;
    float y;
} Path;

typedef struct PathDrive {
    int x;
    int y;
    int x2;
    int y2;
    float dist;
} PathDrive;

PathDrive freePaths[1000];
Path pathToFreeSpace[182];
Quadbox blocked_quads[MAX_QUADS];
Quadbox free_quads[MAX_QUADS];


void vector2matrix() {
    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 128; j++) {
            img2D[i][j] = img[(i * 128 + j)];
            // printf("%d", img2D[i][j]);
        }
        // printf("\n");
    }
}

int isAllEmpty(int y, int x, int depth) {
    // check if quad is all empty, returns true if it is!
    int sum = 0;
    for (int i = x; i < (128 / pow(2, depth)) + x; i++) {
        for (int j = y; j < (128 / pow(2, depth)) + y; j++) {
            if (img2D[i][j] == 1) {
                sum = 1;
            }
        }
    }
    if (sum == 1) {
        return false;
    } else {
        return true;
    }
}

bool isAllFull(int y, int x, int depth) {
    // checks if quad is all full, returns true if it is!
    int sum = 0;
    for (int i = x; i < (128 / pow(2, depth)) + x; i++) {
        for (int j = y; j < (128 / pow(2, depth)) + y; j++) {
            if (img2D[i][j] == 0) {
                sum = 1;
            }
        }
    }
    if (sum == 1) {
        return false;
    } else {
        return true;
    }
}


int block_quad_number = 0;
int free_quad_number = 0;
int count = 0;

void quadtree(int x, int y, int width, int depth) {
    // print out debug info
    count++;
    printf("\ncount: %d, x: %d, y: %d, width: %d, depth: %d\n", count, x, y, width, depth);

    // check if current width is greater than the depth limit
    if (width > DEPTH_LIMIT) {
        // if isAllEmpty then add x, y, depth to free_quads
        if (isAllEmpty(x, y, depth)) {
            printf("all empty\n");
            free_quads[free_quad_number].xcoord = x;
            free_quads[free_quad_number].ycoord = y;
            free_quads[free_quad_number].width = width;
            free_quad_number += 1;
        }
            // if isAllFull then add x, y, depth to blocked_quads
        else if (isAllFull(x, y, depth)) {
            printf("all full\n");
            blocked_quads[block_quad_number].xcoord = x;
            blocked_quads[block_quad_number].ycoord = y;
            blocked_quads[block_quad_number].width = width;
            block_quad_number++;
        } else { // dive further into block because is not full or empty, is mixed!
            printf("mixed\n");
            int width2 = width / 2;
            int depth2 = depth + 1;
            quadtree(x, y, width2, depth2);
            quadtree(x + width2, y, width2, depth2);
            quadtree(x, y + width2, width2, depth2);
            quadtree(x + width2, y + width2, width2, depth2);
        }
    } else { // depth limit reached
        printf("depth limit reached\n");
        if (isAllEmpty(x, y, depth)) {
            printf("all empty\n");
            free_quads[free_quad_number].xcoord = x;
            free_quads[free_quad_number].ycoord = y;
            free_quads[free_quad_number].width = width;
            free_quad_number += 1;
        } else {
            // if its not completely empty, then we cound it as full
            printf("all full\n");
            blocked_quads[block_quad_number].xcoord = x;
            blocked_quads[block_quad_number].ycoord = y;
            blocked_quads[block_quad_number].width = width;
            block_quad_number++;
        }
    }
}


void DrawQuadboxes() {
    for (int i = 0; i <= free_quad_number; i++) { //draw GREEN free quads
        LCDArea(free_quads[i].xcoord, free_quads[i].ycoord, (free_quads[i].xcoord + free_quads[i].width),
                (free_quads[i].ycoord + free_quads[i].width), GREEN, 0);
    }
    for (int i = 0; i <= block_quad_number; i++) { //draw RED blocked quads
        LCDArea(blocked_quads[i].xcoord, blocked_quads[i].ycoord, (blocked_quads[i].xcoord + blocked_quads[i].width),
                (blocked_quads[i].ycoord + blocked_quads[i].width), RED, 0);
    }
}

bool generatePath(int xi, int yi, int xf, int yf) {
    float dist = sqrt( pow( (xf-xi),2 )  +  pow( (yf-yi), 2 ) );
    float x_dist = xf - xi;
    float y_dist = yf - yi;
    int x, y;
    int pathx;
    int pathy;
    LCDCircle(xi, yi, 2, ORANGE, 1);
    if(dist > 0.0) {
        for (int i = 0; i < 182; i++) {
            pathToFreeSpace[i].x = xi + (i * x_dist) / dist;
            pathToFreeSpace[i].y = yi + (i * y_dist) / dist;
            x = pathToFreeSpace[i].x;
            y = pathToFreeSpace[i].y;

            //LCDPixel(x, y, GREEN);
            if (x > 128 || y > 128) {
                continue;
            }
            if (img2D[y][x] == 1) {
                //LCDCircle(x, y, 4, WHITE, 1);
                return False;
            }
        }
    }
    else {
        //LCDCircle(x, y, 4, WHITE, 1);
        return False;
    }
    //LCDCircle(x, y, 4, WHITE, 1);
    return True;
}

void PrintCentreQuads() {
    float xf, yf, widthf;
    int x, y, width;
    float dist;
    int x2, y2 = 0;
    int count = 0;
    for (int i = 0; i < free_quad_number; i++) {
        x = free_quads[i].xcoord;
        y = free_quads[i].ycoord;
        width = free_quads[i].width;
        widthf = (float) free_quads[i].width / 2;
        xf = (float) free_quads[i].xcoord + widthf;
        yf = (float) free_quads[i].ycoord + widthf;


        LCDPixel(x+width/2, y+width/2, BLUE);
    }
    for(int j = 0; j < free_quad_number; j++) {
        x = free_quads[j].xcoord + free_quads[j].width / 2;
        y = free_quads[j].ycoord + free_quads[j].width / 2;
        LCDCircle(x, y, 4, YELLOW, 4);
        while(count < free_quad_number) {
            x2 = free_quads[count].xcoord + free_quads[count].width / 2;
            y2 = free_quads[count].ycoord + free_quads[count].width / 2;
            if(generatePath(x, y, x2, y2)) {

                dist = sqrt((x2 - x) * (x2 - x) + (y2 - y) * (y2 - y));
                freePaths[counter].x = x;
                freePaths[counter].y = y;
                freePaths[counter].x2 = x2;
                freePaths[counter].y2 = y2;
                freePaths[counter].dist = dist;
                counter++;
                printf("Path from (%d, %d) to (%d, %d): %f\n", x, y, x2, y2, dist);
                LCDLine(x, y, x2, y2, BLUE);
            }
            count++;
        }
        count = 0;
    }
}

void driveFuckBot() {
    int angle_diff = 0;
    int current_x = (4000/128)*(free_quads[0].xcoord+free_quads[0].width/2);
    int current_y = 4000-(4000/128)*(free_quads[0].ycoord+free_quads[0].width/2);
    int highest_x, highest_y = 0, highest_dist = 0;
    int driveTo_x = 0;
    int driveTo_y = 0;
    int x_deg, y_deg, phi;
    //convert all of the LCD coords to EYESIM coords
    for(int i = 0; i < counter; i++) {
        freePaths[i].x = (4000/128)*(freePaths[i].x+free_quads[i].width/2);
        freePaths[i].y = 4000-(4000/128)*(freePaths[i].y+free_quads[i].width/2);
        freePaths[i].x2 = (4000/128)*(freePaths[i].x2+free_quads[i].width/2);
        freePaths[i].y2 = 4000-(4000/128)*(freePaths[i].y2+free_quads[i].width/2);
        freePaths[i].dist = (4000/128)*freePaths[i].dist;
    }
    //make some paths
    while(current_y < 3600) {
        for(int i = 0; i < counter; i++) {
                if (freePaths[i].x == current_x && freePaths[i].y2 < current_y && freePaths[i].dist > highest_dist) {
                    highest_dist = freePaths[i].dist;
                    driveTo_y = freePaths[i].y2;
                    driveTo_x = freePaths[i].x2;
                }
            }
            VWGetPosition(&x_deg, &y_deg, &phi);
            angle_diff = atan2(driveTo_y - y_deg, driveTo_x - x_deg) * 180 / PI - phi;
            while (!(angle_diff < 5 && angle_diff > -5)) {
                printf("TURNING\n");
                VWTurn(-90, 10);
                VWGetPosition(&x_deg, &y_deg, &phi);
                angle_diff = atan2(driveTo_y - y_deg, driveTo_x - x_deg) * 180 / PI - phi;
                printf("Angle difference: %d\n", angle_diff);
            }
            VWStraight(0,0);
            printf("Driving\n");
            VWStraight(highest_dist, 300);
            VWWait();
            printf("Done driving\n");
            VWGetPosition(&x_deg, &y_deg, &phi);
            current_x = x_deg;
            current_y = y_deg;
    }
}





int main() {
    int x, y;
    int alpha = 0;
    // reading in image file
    read_pbm(filename, &img);
    printf("starting program!\n\n");

    // starting LCD
    LCDImageStart(0, 0, 128, 128);
    LCDImageBinary(img);

    //converting img to img2D
    vector2matrix();

    // run quadtree
    quadtree(0, 0, 128, 0);

    // debug info
    printf("\nblocked: %d\n", block_quad_number);
    printf("free: %d\n", free_quad_number);

    // draw quadboxes
    DrawQuadboxes();
    PrintCentreQuads();
    x = (4000/128)*(free_quads[0].xcoord+free_quads[0].width/2);
    y = 4000-(4000/128)*(free_quads[0].ycoord+free_quads[0].width/2);
    SIMSetRobot(2, x, y, 0, alpha);
    VWSetPosition(x, y, alpha);
    driveFuckBot();
    // DAVID
    // also need to print centre coordinates + print intersecting lines (that do not cross into '1' boundary)

    // Then find correct path
    // then follow path

    printf("Press something to end program\n");
    getchar();

    return 0;
}