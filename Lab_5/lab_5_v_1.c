#include "eyebot.h"
#include "image.c"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

// Colours
/*
Color Constants (COLOR is data type "int" in RGB order):
RED (0xFF0000), GREEN (0x00FF00), BLUE (0x0000FF), WHITE (0xFFFFFF), GRAY (0x808080), BLACK (0)
ORANGE, SILVER, LIGHTGRAY, DARKGRAY, NAVY, CYAN, TEAL, MAGENTA, PURPLE, MAROON, YELLOW, OLIVE
*/
int colours[] = {0 ,RED, GREEN, BLUE, WHITE, GRAY, ORANGE, SILVER, LIGHTGRAY, DARKGRAY, NAVY, CYAN, TEAL, MAGENTA, PURPLE, MAROON, YELLOW, OLIVE};
BYTE* img;
BYTE img2D[128][128] = {0}; 
int colour_array[128][128];
char* filename = "rectangles.pbm";

void vectorToMatrix(){
    for(int i = 0; i <128 ;i++){ 
        for(int j = 0; j <128 ; j++){
            img2D[i][j]= img[(i*128+j)];
        }
    }
}

void printOutArray(int c){
    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 128; j++){
            // printf("%d", img2D[i][j]);
            if (img2D[i][j] == 1){
                LCDPixel(j, i, c);
            }
        }
    // printf("\n");
}
}

void printOutColourArray(){
    for(int r = 0; r < 128; r++){
        for(int c = 0; c < 128; c++){
            // printf("%d", img2D[i][j]);
            LCDPixel(c, r, colour_array[r][c]);
        }
    // printf("\n");
}
}


void brushfire(){
    //label each object independently
    int object_num = 1;
    for (int r = 0; r < 128; r ++){
        for (int c = 0; c < 128; c++){
            if (img2D[r][c] == 1){
                if(colour_array[r][c - 1] != 0 || colour_array[r - 1][c] != 0){
                    colour_array[r][c] = object_num;
                }
                else{
                    object_num++;
                    colour_array[r][c] = object_num;
                }


                // if ((img2D[r][c + 1] == 1) || (img2D[r - 1][c + 1] == 1)){
                //     colour_array[r][c] = colours[object_num];
                //     printf("r: %d, c: %d\n", r, c);
                // }
                // else{
                //     printf("new object num: %d, r: %d, c: %d\n", object_num, r, c);
                //     object_num++;
                //     colour_array[r][c] = object_num;
                // }
            }
        }
    }
}





int main(){
	// reading in image file
    read_pbm(filename, &img);
	printf("starting program!\n\n");

	// starting LCD
    LCDImageStart(0, 0, 128, 128);
    LCDImageBinary(img);

    vectorToMatrix();
    // printOutArray(GREEN);
    // LCDSetPrintf(15, 0, "HELLOO");

    // brushfire();
    // printOutColourArray();

    printf("%d\n",img2D[13][82]);
    printf("%d\n",img2D[13][83]);
    printf("%d\n",img2D[13][84]);

    printf("Press enter to end program\n");
    getchar(); 

    return 0;
}
