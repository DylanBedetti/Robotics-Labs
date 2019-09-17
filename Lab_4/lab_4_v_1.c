#include "eyebot.h"
#include "image.c"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define LINE_MAX 255
#define DEPTH_LIMIT 8 //in terms of pixels!
#define MAX_QUADS 256

BYTE* img;
BYTE img2D[128][128]; 
char* filename = "corner.pbm";

typedef struct Quadbox {
    int xcoord;
    int ycoord;
    int width;
} 
Quadbox;
Quadbox blocked_quads[MAX_QUADS];
Quadbox free_quads[MAX_QUADS];


void vector2matrix(){
    for(int i = 0; i <128 ;i++){ 
        for(int j = 0; j <128 ; j++){
            img2D[i][j]= img[(i*128+j)];
			// printf("%d", img2D[i][j]);
        }
	// printf("\n");
    }
}

int isAllEmpty(int x, int y, int depth){
	// check if quad is all empty, returns true if it is!
	int sum = 0;
	for (int i = x; i <(128 / pow(2, depth)) + x; i++){
		for (int j = y; j < (128 / pow(2, depth)) + y; j++){
			if (img2D[i][j] == 1){
				sum = 1;
			}
		}
	}
	if (sum == 1){
		return false;
	} else {
		return true;
	}
}

bool isAllFull(int x, int y, int depth){
	// checks if quad is all full, returns true if it is!
	int sum = 0;
	for (int i = x; i < (128 / pow(2, depth)) + x; i++){
		for (int j = y; j < (128 / pow(2, depth)) + y; j++){
			if (img2D[i][j] == 0){
				sum = 1;
			}
		}
	}
	if (sum ==  1){
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
	count++; printf("\ncount: %d, x: %d, y: %d, width: %d, depth: %d\n", count, x, y, width, depth);

	// check if current width is greater than the depth limit
	if (width > DEPTH_LIMIT) {
		// if isAllEmpty then add x, y, depth to free_quads
		if (isAllEmpty(x, y, depth)){
			printf("all empty\n");
			free_quads[free_quad_number].xcoord = x;
            free_quads[free_quad_number].ycoord = y;
            free_quads[free_quad_number].width = width;
			free_quad_number += 1;
		} 
		// if isAllFull then add x, y, depth to blocked_quads
		else if (isAllFull(x, y, depth)){
			printf("all full\n");
			blocked_quads[block_quad_number].xcoord = x;
            blocked_quads[block_quad_number].ycoord = y;
            blocked_quads[block_quad_number].width = width;
			block_quad_number++; 
		} else { // dive further into block because is not full or empty, is mixed!
			printf("mixed\n");
			int width2 = width/2;
			int depth2 = depth + 1;
            quadtree(x, y, width2, depth2);
            quadtree(x + width2, y, width2, depth2);
            quadtree(x, y + width2, width2, depth2);
            quadtree(x + width2, y + width2, width2, depth2);
		}
	} else { // depth limit reached
		printf("depth limit reached\n");
		if (isAllEmpty(x, y, depth)){
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


int main(){
    read_pbm(filename, &img);
	printf("starting program!\n\n");

    // LCDImageStart(0, 0, 128, 128);
    // LCDImageBinary(img);


	vector2matrix();
	quadtree(0, 0, 128, 0);

	printf("\nblocked: %d\n", block_quad_number);
	printf("free: %d", free_quad_number);

	// printf("%d\n", isAllEmpty(64, 64, 1));
	// printf("%d\n", isAllFull(64, 0, 2));

    return 0;
}
