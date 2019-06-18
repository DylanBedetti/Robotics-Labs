#include <stdio.h>
#include "eyebot.h"
#include <time.h>

int number_of_points = 20;
float pos;
float t;
int m0, m1, p0, p1;

void SplineDrive(int x, int y, int alpha){
// x in robot’s forward direction, y perpendicular (right HR)
// relative distance in mm; relative angle in degrees

}

void test_spline(m0, m1, p0, p1){
    for(int i =0; i < number_of_points; i++){
        t = i / (number_of_points = 1.0);
        pos =  (2.0 * t * t * t - 3.0 * t * t + 1.0) * p0 
		+ (t * t * t - 2.0 * t * t + t) * m0 
		+ (-2.0 * t * t * t + 3.0 * t * t) * p1 
		+ (t * t * t - t * t) * m1;
        printf("pos: %f", pos);

    }
}


int main() {
    test_spline();
    SIMSetRobot(0,1000,1000,1,0);

}