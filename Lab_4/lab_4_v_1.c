#include "eyebot.h"
#include "image.c"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LINE_MAX 255
#define DEPTH_LIMIT 4
#define MAX_QUADS 256

BYTE* img;
BYTE img2D[128][128]; 

typedef struct Quadbox {
    int xcoord;
    int ycoord;
    int width;
} Quadbox;

Quadbox blocked_quads[MAX_QUADS];
Quadbox free_quads[MAX_QUADS];


void vector2matrix(){
    int k = 0; 
    for(int i = 0; i <128 ;i++){ 
        for(int j = 0; j <128 ; j++){
            img2D[i][j]= img[(i*128+j)];
            k++;
        }
    }
}

bool checkEmpty(int x, int y, int depth){
	for (int j = 0; j < 64 / pow(depth, 2); j++){
		for (int i = 0; i < 64 / pow(depth, 2); i++){

		}
	}
}

void quadtree(int x, int y, int width, int depth_limit) {
	if (depth_limit > 0) {
}           


int main(){
    char* filename = "corner.pbm";
    read_pbm(filename, &img);
    printf("%d\n", img[0]);

    LCDImageStart(0, 0, 128, 128);
    LCDImageBinary(img);

    for (int t = 0; t < 100; t++){
        usleep(100000);
        printf("%d\n", t);
    }
    return 0;
}
