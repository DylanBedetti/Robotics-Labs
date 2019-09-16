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
	printf("\n\nISALLEMPYT DEPTH: %d\n", depth);
	int sum = 0;
	for (int i = x; i <(64 / pow(2, depth)) + x; i++){
		for (int j = y; j < (64 / pow(2, depth)) + y; j++){
			if (img2D[i][j] == 1){
				sum = 1;
			}
		}
	}
	if (sum > 0){
		return false;
	} else {
		return true;
	}
}

bool isAllFull(int x, int y, int depth){
	int sum = 0;
	for (int i = x; i < (64 / pow(2, depth)) + x; i++){
		for (int j = y; j < (64 / pow(2, depth)) + y; j++){
			if (img2D[i][j] == 1){
				sum = 1;
			}
		}
	}
	if (sum ==  1){
		return true;
	} else {
		return false;
	}
}


int block_quad_number = 0;
int free_quad_number = 0;
int count = 0;
void quadtree(int x, int y, int width, int depth) {
	count++; printf("count: %d\n", count);
	if (width >= DEPTH_LIMIT) {
		if (isAllEmpty(x, y, depth)){
			printf("all empty\n");
			free_quads[free_quad_number].xcoord = x;
            free_quads[free_quad_number].ycoord = y;
            free_quads[free_quad_number].width = width;
			free_quad_number += 1;
		} else if (isAllFull(x, y, width)){
			printf("all full\n");
			blocked_quads[block_quad_number].xcoord = x;
            blocked_quads[block_quad_number].ycoord = y;
            blocked_quads[block_quad_number].width = width;
			block_quad_number++; 
		} else { // dive further
			int width2 = width/2;
			int depth2 = depth + 1;
            quadtree(x, y, width2, depth2);
            quadtree(x + width2, y, width2, depth2);
            quadtree(x, y + width2, width2, depth2);
            quadtree(x + width2, y + width2, width2, depth2);
		}
	} else { // depth limit reached
		if (isAllEmpty(x, y, depth)){
			printf("all empty\n");
			free_quads[free_quad_number].xcoord = x;
            free_quads[free_quad_number].ycoord = y;
            free_quads[free_quad_number].width = width;
			free_quad_number += 1;
		} else {
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
	printf("starting!\n");

    // LCDImageStart(0, 0, 128, 128);
    // LCDImageBinary(img);

    // for (int t = 0; t < 100; t++){
    //     usleep(100000);
    //     printf("%d\n", t);
    // }
	vector2matrix();
	quadtree(0, 0, 128, 0);

	printf("blocked: %d", block_quad_number);
	printf("free: %d", free_quad_number);

	// printf("%d\n", isAllEmpty(64, 100, 2));
	// printf("%d\n", isAllFull(64, 0, 2));

    return 0;
}
