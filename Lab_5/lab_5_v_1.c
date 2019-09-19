#include "eyebot.h"
#include "image.c"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


int colours[] = {0 ,RED, GREEN, BLUE, WHITE, GRAY, ORANGE, SILVER, LIGHTGRAY, DARKGRAY, NAVY, CYAN, TEAL, MAGENTA, PURPLE, MAROON, YELLOW, OLIVE};
BYTE* img;
BYTE img2D[128][128]; 
BYTE img2D_temp[128][128] = {0};
int colour_array[128][128] = {0};
char* filename = "rectangles.pbm";

void vectorToMatrix(){
    // chaning img vector to img2D matrix
    for(int i = 0; i <128 ;i++){ 
        for(int j = 0; j <128 ; j++){
            img2D[i][j]= img[(i*128+j)];
        }
    }
}

void printOutArray(){
    // terminal print of array
    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 128; j++){
            printf("%d", img2D[i][j]);
        }
    printf("\n");
    }
}

void printOutColourArray(){
    // printing to LCD 
    for(int r = 0; r < 128; r++){
        for(int c = 0; c < 128; c++){
            // LCDPixel(c, r, colours[img2D[r][c]]);
            LCDPixel(c, r, colours[colour_array[r][c]]);
        }
    }
}

int check_zeros(){
    // checking if zeros left in img2D
    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 128; j++){
            if (img2D[i][j] == 0){
                return 0;
            }
        }
    }
    return 1;
}

void copyArrays(int k){
    // function for copying arrays between eachother
    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 128; j++){
            if (k == 1){
                img2D_temp[i][j] = img2D[i][j];
            } else{
                img2D[i][j] = img2D_temp[i][j];
            }

        }
    }
}

int two_neighbours(int r, int c){
    // only considering neighbours left, right, above, below
    int sum = 0;
    for (int i = -1; i <= 1; i++){
        for (int j = -1; j <= 1; j++){
            if (i == 0 || j == 0){
                if (img2D[r + i][c + j] > 0){
                sum++;
                }
            }
        }
    }
    return sum;
}


void brushfire(){
    printf("doing brushfire!\n");
    printf("lablling objects\n");
    //label each object independently and assigning edges to 2
    int object_num = 1;
    for (int r = 0; r < 128; r ++){
        for (int c = 0; c < 128; c++){
            if (img2D[r][c] == 1){
                if(colour_array[r][c - 1] != 0){
                    colour_array[r][c] = colour_array[r][c-1];
                    // printf("r: %d, c: %d\n", r, c);
                } else if(colour_array[r - 1][c] != 0){
                    colour_array[r][c] = colour_array[r-1][c];
                    // printf("r: %d, c: %d\n", r, c);
                }
                else{
                    // printf("new object num: %d, r: %d, c: %d\n", object_num, r, c);
                    colour_array[r][c] = object_num;
                    object_num++;
                }
            }
            // setting edges to 1
            if (r == 0 || r == 127 || c == 0 || c == 127){
                img2D[r][c] = 1;
            }
        }
    }

    printOutArray();
    getchar();

    // img2D_temp = img2D;
    copyArrays(1);

    printf("Starting numbering\n");
    printf("Object num: %d\n", object_num);
    // looping through
    int contains_zeros = 0;
    for (int i = 2; contains_zeros == 0; i++){
        for (int r = 1; r < 127; r++){
            for (int c = 1; c < 127; c++){
                // 8-nearest neighbour
                if ((img2D[r + 1][c] != 0 || img2D[r - 1][c] != 0 || img2D[r][c + 1] != 0 || img2D[r][c - 1] != 0 || img2D[r + 1][c + 1] != 0 || img2D[r - 1][c - 1] != 0 || img2D[r - 1][c + 1] != 0 || img2D[r + 1][c - 1] != 0) && (img2D[r][c] == 0)){
                    img2D_temp[r][c] = i;
                }
                // checking for atleast two neighbours (left, right, top, bottom)
                if (two_neighbours(r, c) >= 2 && (img2D[r][c] == 0)){
                    colour_array[r][c] = object_num; // setting Voronoi point
                }

            }
        } 
        contains_zeros = check_zeros();

        // img2D = img2D_temp;
        copyArrays(0);
        printOutArray(); //terminal
        printOutColourArray(); //LCD
        getchar();
    }
}





int main(){
	// reading in image file
    read_pbm(filename, &img);
	printf("starting program!\n\n");

	// starting LCD
    LCDImageStart(0, 0, 128, 128);
    LCDImageBinary(img);
    LCDSetPrintf(15, 0, "PRESS ENTER!");
    
    // reading image file to matrix
    vectorToMatrix();

    //brushfire functon
    brushfire();

    // need to add drive


    printf("Press enter to end program\n");
    getchar(); 

    return 0;
}
