#include "eyebot.h"
#include "image.c"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

BYTE* img;
char* filename = "fourblocks.pbm";
BYTE img2D[128][128]; 

void vector2matrix(){
    for(int i = 0; i <128 ;i++){ 
        for(int j = 0; j <128 ; j++){
            img2D[i][j]= img[(i*128+j)];
			// printf("%d", img2D[i][j]);
        }
	// printf("\n");
    }
}


int main(){
	// reading in image file
    read_pbm(filename, &img);
	printf("starting program!\n\n");

	// starting LCD
    LCDImageStart(0, 0, 128, 128);
    LCDImageBinary(img);

    vector2matrix();


    printf("Press enter to end program\n");
    getchar(); 

    return 0;
}
