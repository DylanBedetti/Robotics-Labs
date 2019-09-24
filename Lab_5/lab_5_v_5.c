#include "eyebot.h"
#include "image.c"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#define TURN_SPEED 10
#define PI acos(-1)

typedef struct World_Route {
    int x;
    int y;
    int count;
} World_Route;
int counter = 0;
World_Route world_route[1000];
int colours[] = {0 ,RED, GREEN, BLUE, WHITE, GRAY, ORANGE, SILVER, LIGHTGRAY, DARKGRAY, YELLOW, CYAN, TEAL, MAGENTA, PURPLE, MAROON, NAVY, OLIVE};
BYTE* img;
BYTE img2D[128][128]; 
BYTE img2D_temp[128][128] = {0};
int colour_array[128][128] = {0};
int colour_array_temp[128][128] = {0};
int route[128][128] = {0};
char* filename = "u.pbm";
int object_num = 1;
int x_pos, y_pos, phi = 0;
int speed = 50;
int r_pos = 127;
int c_pos = 127;

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

void printRoute(){
    // terminal print of array
    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 128; j++){
            printf("%d", route[i][j]);
        }
    printf("\n");
    }
}

void printOutColourArray(){
    // printing to LCD 
    for(int r = 0; r < 128; r++){
        for(int c = 0; c < 128; c++){
            LCDPixel(c, r, colours[colour_array[r][c]]);
            printf("%d", colour_array[r][c]);
        }
        printf("\n");
    }
}

void printOutColourVeroni(){
    // printing to LCD 
    for(int r = 0; r < 128; r++){
        for(int c = 0; c < 128; c++){
            if (colour_array[r][c] == object_num){
                LCDPixel(c, r, colours[colour_array[r][c]]);
                printf("1");
            } else {
                LCDPixel(c, r, 0);
                printf("0");
            }
        }
        printf("\n");
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
                // colour_array_temp[i][j] = colour_array[i][j];
                // colour_array[i][j] = colour_array_temp[i][j];
                img2D[i][j] = img2D_temp[i][j];
            }

        }
    }
}

int eightNearest(int r, int c){
    if (img2D[r][c] == 0){
        for (int i = -1; i <= 1; i++){
            for (int j = -1; j <= 1; j++){
                if (i != 0 && j != 0){
                    if (img2D[r + i][c + j] != 0){
                        colour_array[r][c] = colour_array[r + i][c + j];
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int eightveroni(int r, int c){
    int c_1 = 0;
    for (int i = -1; i <= 1; i++){
            for (int j = -1; j <= 1; j++){
                if (i != 0 && j != 0){
                    if (colour_array[r + i][c + j] != 0 && colour_array[r + i][c + j] != object_num){
                        if (c_1 == 0){
                            c_1 = colour_array[r + i][c + j];
                        } else if (c_1 != colour_array[r + i][c + j]){
                            colour_array[r][c] = object_num;
                            return 0;
                        }
                    }
                }
            }
        }
    return 0;
}



void brushfire(){
    printf("doing brushfire!\n");
    printf("lablling objects\n");
    //label each object independently and assigning edges to 2
    // currently will only work for square shapes!
    for (int c = 128; c >= 0; c--){
        for (int r = 127; r >= 0; r--){
            if (img2D[r][c] == 1){
                if(colour_array[r - 1][c + 1] != 0){
                    colour_array[r][c] = colour_array[r - 1][c + 1];

                } else if(colour_array[r][c + 1] != 0){
                    colour_array[r][c] = colour_array[r][c + 1];

                } else if(colour_array[r + 1][c + 1] != 0){
                    colour_array[r][c] = colour_array[r + 1][c + 1];

                }else if(colour_array[r + 1][c] != 0){
                    colour_array[r][c] = colour_array[r + 1][c];
                }
                else{
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

    for (int i = 0; i < 128; i++){
        colour_array[i][0] = object_num;
    }
    object_num++;
    
    for (int i = 0; i < 128; i++){
        colour_array[0][i] = object_num;
    }
    object_num++;

    for (int i = 0; i < 128; i++){
        colour_array[i][127] = object_num;
    }
    object_num++;

        for (int i = 0; i < 128; i++){
        colour_array[127][i] = object_num;
    }
    object_num++;

    // making corners veroni
    colour_array[0][0] = object_num;
    colour_array[0][127] = object_num;
    colour_array[127][0] = object_num;
    colour_array[127][127] = object_num;

    printOutArray();
    getchar();

    // img2D_temp = img2D;
    copyArrays(1);

    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 128; j++){
            colour_array_temp[i][j] = colour_array[i][j];
        }
    }

    printf("Starting numbering\n");
    printf("Object num: %d\n", object_num);
    // looping through
    int contains_zeros = 0;
    for (int i = 2; contains_zeros == 0; i++){
        for (int r = 1; r < 127; r++){
            for (int c = 1; c < 127; c++){
                // 8-nearest neighbour
                if (eightNearest(r, c)){
                    img2D_temp[r][c] = i;
                    eightveroni(r, c);
                }
            }
        } 
        contains_zeros = check_zeros();

        copyArrays(0);
        // printOutArray(); //terminal
        // printOutColourArray(); //LCD
        // getchar(); // wait for next loop
    }
    for (int r = 1; r < 127; r++){
        for (int c = 1; c < 127; c++){
            if(colour_array[r + 1][c] == object_num && colour_array[r - 1][c] == object_num){
                colour_array[r][c] = object_num;
            } else if (colour_array[r][c + 1] == object_num && colour_array[r][c + 1] == object_num){
                colour_array[r][c] = object_num;
            }
        }
    }
    printOutColourArray();
}

int find_random(int i){
    for (int r = r_pos - 1; r <= r_pos + 1; r++){
        for (int c = c_pos - 1; c <= c_pos + 1; c++){
            printf("r: %d, c: %d\n", r, c);
            if (colour_array[r][c] == object_num && route[r][c] == 0){
                r_pos = r;
                c_pos = c;
                route[r_pos][c_pos] = i;
                if (c_pos * (4000/128) - 200 <= 200 ||  c_pos * (4000/128) - 200 >= 3800) {
                    return 0;
                }
                else if(r_pos * (4000/128) - 200 <= 200 || r_pos * (4000/128) - 200 >= 3800) {
                    return 0;
                }
                world_route[i].y = c_pos * (4000/128) - 200;
                world_route[i].x = r_pos * (4000/128) - 200;
                printf("x: %d, y: %d\n", world_route[i].x, world_route[i].y);
                world_route[i].count = i;
                counter = i;
                return 0;
            }
        }
    }
    return 0;
}

void findRoute(){
    printf("\n\n FINDING ROUTE\n\n");
    // function to find a route from bottom right to top left
    // at each pixel, choose the one that will get you closes to the top left
    int i = 1;
    route[127][127] = i;
    while(route[0][0] == 0){
        i++;
        // find_closest();
        find_random(i);
        // printf("ROUTE\n");
        // printRoute();
        // printf("VERONI\n");
        // printOutColourVeroni();
        // getchar();
    }
    printRoute();
}


int convertRouteToWorld() {
    // int fuck_count = 2;
    // while(world_route.count[fuck_count] != 0]) {
    //     if(world_route.x[fuck_count] <= 200 || world_route.x[fuck_count] >= 3800) continue;
    //     else if(world_route.y[fuck_count] * (4000/128) - 200 <= 200 || world_route.y[fuck_count] * (4000/128) - 200 >= 3800) continue;
    //     world_route.x[fuck_count] = world_route.x[fuck_count] * (4000/128) - 200;
    //     world_route.y[fuck_count] = world_route.y[fuck_count] * (4000/128) - 200;
    //     fuck_count++;
    // }
    // printf("fuck_count: %d\n", fuck_count);
    // return fuck_count;
    int iterations = 2;
    while(1) {
        printf("x: %d\ty:%d\n",world_route[iterations].x, world_route[iterations].y);
        if(!(world_route[iterations].count < world_route[iterations+1].count)) {
            return iterations;
        }
        iterations++;
    }

}

void driveFuckBot() {
    int iter = 2;
    int left = 0;
    int right = 0;
    int count_drive = counter;
    double angle_diff = 0.0;
    double dist_diff = 10;
    while(count_drive >= 2) {
        printf("iter: %d\n", count_drive);
        VWGetPosition(&x_pos, &y_pos, &phi);
        angle_diff = ((atan2(world_route[count_drive].y - y_pos, world_route[count_drive].x - x_pos) * 180) / PI) - phi;
        while(!(angle_diff < 2 && angle_diff > -2)) {
            if(angle_diff < 0) VWTurn(-90,(int)(TURN_SPEED*(1+-1*angle_diff/20)));
            else if(angle_diff > 0) VWTurn(90,(int)(TURN_SPEED*(1+angle_diff/20)));
            VWGetPosition(&x_pos, &y_pos, &phi);
            angle_diff = ((atan2(world_route[count_drive].y - y_pos, world_route[count_drive].x - x_pos) * 180) / PI) - phi;
            printf("Turn Speed: %d\tAngle: %f\tDistance:%f\tIter: %d\t x: %d\t y: %d\r", (int)(TURN_SPEED*(1+angle_diff/20)),angle_diff, dist_diff, count_drive, world_route[count_drive].x, world_route[count_drive].y);
            fflush(stdout);
        }
        VWTurn(0,0);
        VWGetPosition(&x_pos, &y_pos, &phi);
        dist_diff = sqrt(pow((world_route[count_drive].x - x_pos),2) + pow((world_route[count_drive].y - y_pos),2));
        printf("Angle: %f\tDistance:%f\tIter: %d\t x: %d\t y: %d\n", angle_diff, dist_diff, count_drive, world_route[count_drive].x, world_route[count_drive].y);
        fflush(stdout);
        left = PSDGet(2);
        right = PSDGet(3);
        if(left <= right) {
            printf("Closer to left\n");
            if(left < 200){
                VWCurve(dist_diff, ((200 - left)) +1, 200);
                printf("Too close LEFT\tCurving: %d\n", ((200 - left)/2));
                VWWait();
            }
            else VWStraight(dist_diff, 200);
            VWWait();
        }

        else if(left > right) {
            printf("Closer to right\n");
            if(right < 200) {
                VWCurve(dist_diff, -1*(((200 - right)) +1), 200);
                printf("Too close RIGHT\tCurving: %d\n", -1*((200 - right)/2));
                VWWait();
            }
            else VWStraight(dist_diff, 200);
            VWWait();

        }
        else {
            VWStraight(dist_diff, 200);
            VWWait();
        }
        if(PSDGet(1) < 80) {
            VWStraight(-100,100);
            VWWait();
        }
        count_drive -= 3;
    }
}


int main(){
    // setting robot top left
    SIMSetRobot(0,200,3800,1, 180);
    VWStraight(0,0);
    // getting current pos
    VWSetPosition(x_pos, y_pos, phi);
    VWGetPosition(&x_pos, &y_pos, &phi);

    // rotating into right angle
    //VWTurn(-135, speed);
    //VWWait();

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

    printf("see only veroni!\n");
    getchar();
    printOutColourVeroni();

    // need to add drive
    // map is 4000, 4000. Therefore GetPos is equivalent to (pos/4000)*128


    findRoute();
    //printf("Count: %d\n",convertRouteToWorld());
    printf("COUNTER =  %d\n", counter);
    driveFuckBot();

    printf("Press enter to end program\n");
    getchar(); 

    return 0;
}

