#include "eyebot.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// CREATE TEST CODE BEFORE RUNNING THIS???

#define NO_HUE 0
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int size = QVGA_SIZE; // 240 by 320
int width = QVGA_X;
int height = QVGA_Y;

int count;  // used as global variable to count red pixels in image

int pixel(int r, int w, int c) {
    return (r * w) + c;
}
// HUE
BYTE RGBtoHue(BYTE r, BYTE g, BYTE b){
    int hue, delta, max, min;
    max = MAX(r, MAX(g,b));
    min = MIN(r, MIN(g,b));
    delta = max - min;
    hue =0; /* init hue*/
    if (2*delta <= max) hue = NO_HUE; /* gray, no color */
    else {
        if (r==max) hue = 42 + 42*(g-b)/delta; /* 1*42 */
        else if (g==max) hue = 126 +42*(b-r)/delta; /* 3*42 */
        else if (b==max) hue = 210 +42*(r-g)/delta; /* 5*42 */
    }
    return hue; /* now: hue is in range [0..252] */;
}

// SATURATION
BYTE RGBtoSat(float r, float g, float b){
    r = r/255;
    g = g/255;
    b = b/255;
    float max = MAX(r, MAX(g,b));
    float min = MIN(r, MIN(g,b));
    float L = 100*(max + min)/2;
    float sat;
    if (L < 50){
        sat = ((max-min)/(max+min))*100;
    }
    else {
        sat = ((max-min)/(2-max-min))*100;
    }
    return sat;
}

// INTENSITY
BYTE RGBtoInten(float r, float g, float b){
    r = r/255;
    g = g/255;
    b = b/255;
    float max = MAX(r, MAX(g,b));
    float min = MIN(r, MIN(g,b));
    float L = 100*(max + min)/2;
    return L;

}

void RGBtoHSV(BYTE img[]){
    BYTE hue, s, i, r, g, b;
    int a = 0, j = 0, pix;
    for (a = 0; a < width; a++) {
        for (j = 0; j < height; j++) {
            pix = pixel(j, width, a) * 3;
            r = img[pix], g = img[pix + 1], b = img[pix + 2]; // FINDING RGB
            hue = IPPRGB2Hue(r, g, b), s = RGBtoSat(r, g, b), i = RGBtoInten(r, g, b); // CONVERTING TO HSV
            if ((hue > 10) && (hue < 80) && (i > 20) && (s > 20)) { // PICKING COLOUR RANGE ** WILL NEED TO CHANGE
                img[pix] = 255; // converting to RED only
                img[pix + 1] = 0;
                img[pix + 2] = 0;
            }
        }
    }
}

int Crosshair(BYTE img[]){
    int crosshair,a ,j, pix;
    int max = 0;
    int count_col = 0;
    count = 1;
    for (a=0; a< width; a++){ // cycling through columns
        count_col = 0;
        for (j = 0; j < height; j++){ // cycling through rows
            pix = pixel(j, width, a) * 3;
            if (img[pix] == 255){
                count_col++;
                pix = pixel(height - count_col, width, a) * 3; // CHANGING DEF OF PIX
                img[pix] = 20;  // making the lines green
                img[pix + 1] = 255;
                img[pix + 2] = 20;
                count += 1; // count # of red pixels in frame
                if (count_col > max) crosshair = a;
                max = MAX(max, count_col);
            }

        }
    }
    return crosshair; // will return column value of highest red
}


int main() {
    LCDMenu("Start", " ", " ", " ");
    KEYWait(1);
    int state = 0, error = 0, output = 126, row = 18, x = 0, y = 0, phi = 0; // starting state is 0, starting error is 0, output should be midpoint of servo
    float Kp = 0.1, angle, PI = 3.14, i, j, theta, distance; // Kp for p-controller, angle is return angle, i = x, j = y, m is gradient for return
    // starting row for all comments
    // initialising position variables

    VWSetPosition(y, x, phi); // Setting position
    CAMInit(QVGA);
    BYTE img[size];
    SERVOSetRaw(1, output); // suppose to keep servo pointing forward, making sure to use Raw
    LCDImageStart(0, 0, width, height);

    while(1){
        CAMGet(img);
        RGBtoHSV(img); // finding and converting red pixels
        int center = Crosshair(img);
        LCDImage(img);
        VWGetPosition(&y, &x, &phi);

        switch(state) {
            case 0: // CHECKING AREA FOR RED CANS
                LCDSetPrintf(1, 55, "State: %i  ", state);
                if (phi > -90) { // DO A 90 DEGREE TURN TO THE RIGHT
                    //VWTurn(-1, 50);  // CHANGE THIS MAYBE AS HEARD IT INTRODUCES DELAY
                    MOTORDrive(1, 10);
                    MOTORDrive(2, -10);
                    usleep(1000);
                    LCDSetPrintf(row + 1, 0, "count in: %i  phi: %i  state: %i count: %i                    ", count, phi, state, count);
                }
                else LCDSetPrintf(row + 1, 0, " no red cans :(                " ), LCDMenu("Turn off", " ", " ", " "), KEYWait(1), state = 10;

                if (count > 150) state = 1; // MIGHT NEED TO CHANGE **************************************************************************************

                //LCDSetPrintf(row, 0, "======= Press Key to continue to state 1 =======" );
                //KEYWait(1);

                break;




            case 1: // GOING TO HIGH VALUES AND FURTHEST LEFT RED CAN
                LCDSetPrintf(1, 55, "State: %i  ", state);
                error = center - (width/2);  // error relative to center of QVGA width (320)
                output = /*output*/ 125 + (Kp * error); // CHANGE THIS SIGN TO A MINUS FOR REAL TEST ***********************************************************************
                if (output < 0) output = 0;         // setting limits
                if (output > 255) output = 255;
                //SERVOSetRaw(1, output);             // servo writing - MIGHT NOT NEED THIS
                LCDSetPrintf(row, 0, " error: %i servo output: %i PSD front: %i center: %i          ",error, output, PSDGetRaw(1), center);


                if (output < 123) VWCurve(10, 3, 100), usleep(1000); // NEED TO FIND BETTER WAY OF CONTROLLING?
                else if (output > 127) VWCurve(10, -3, 100), usleep(1000);
                else VWStraight(10, 100), usleep(1000);
                LCDSetPrintf(row + 1, 0, "x: %i  y: %i  phi: %i                          ",-x, y, phi);

                if (PSDGetRaw(1) < 300){  // WILL NEED TO CHANGE THIS FOR REAL TEST ***********************************************************************
                    state = 2;
                }
                break;

            case 2: // TURN TO FACE STARTING POSITION
                LCDSetPrintf(1, 55, "State: %i  ", state);
                i = -x, j = y; // not too sure why, but this works
                angle = atan(j/i)*180/PI;
                angle = (int) angle, angle = angle + 90; // converting to int and adding required 90 degrees
                LCDSetPrintf(row, 0, " angle: %i  phi %i  x: %f  y: %f      ",angle, phi, i, j);

                //LCDSetPrintf(row, 0, "======= Press Key to continue =======" );
                //KEYWait(1);

                while (phi < angle){
                    VWTurn(3, 50); // turn left until reaching angle
                    VWGetPosition( &y, &x, &phi); // update x, y, phi
                    LCDSetPrintf(row + 1, 0, " angle: %i  phi %i  x: %i  y: %i    ",angle, phi,-x ,y);
                }
                state = 3;

                //LCDSetPrintf(row, 0, "======= Press Key to continue to state 3 =======" );
                //KEYWait(1);

                break;

            case 3: // use phi as control and get back to initial x,y withing certain bounds
                LCDSetPrintf(1, 55, "State: %i  ", state);
                i = -x;
                j = y;
                theta = (-atan(j/x)*180/PI) + 90; // not sure why i need to negate this
                LCDSetPrintf(19, 0, " theta: %f y: %f  x: %f  distance left: %f                 ", theta, j, i, distance);

                distance = sqrt(i*i + y*y);


                if (phi < (theta - 1)) VWCurve(30, 5, 100), usleep(1000), LCDSetPrintf(row + 1, 0, " turning left                   ");
                else if (phi > (theta + 1)) VWCurve(30, -5, 100), usleep(1000), LCDSetPrintf(row + 1, 0, " turning right                    ");
                else VWStraight(10, 100), usleep(1000), LCDSetPrintf(row + 1, 0, " straight                      ");

                if (PSDGetRaw(1) < 300  && distance > 400) state = 4;  // CHANGE ***************************************************************************

                if (i < 30 || j < 30) state = 5;

                break;

            case 4: // Object detection
                LCDSetPrintf(1, 55, "State: %i  ", state);
                LCDSetPrintf(row, 0, "OBJECT DETECTED                                            ");
                LCDSetPrintf(row + 1, 0, "OBJECT DETECTED                                         ");
                LCDSetPrintf(row + 2, 0, "OBJECT DETECTED                                             ");
                VWTurn(90, 100);
                VWWait();
                VWStraight(300, 100);
                VWWait();
                VWTurn(-90, 100);
                VWWait();
                state = 3;
                break;


            case 5: // turn back to 0 degree position
                LCDSetPrintf(1, 55, "State: %i  ", state);
                LCDSetPrintf(row, 0, "RESTARTING                                                        ");
                LCDSetPrintf(row + 1, 0, "RESTARTING                                                     ");
                LCDSetPrintf(row + 2, 0, "RESTARTING                                                    ");
                while (phi > 3 || phi < 3) {
                    VWTurn(3, 50); // turn left
                    VWGetPosition( &x, &y, &phi);
                }
                state = 0;
                VWSetPosition(x, y, phi);
                break;

            case 10:
                LCDSetPrintf(1, 55, "State: %i  ", state);
                CAMRelease();
                LCDSetPrintf(row, 0, "================================                        ");
                LCDSetPrintf(row + 1, 0, "===========COMPLETE================             ");
                LCDSetPrintf(row + 2, 0, "================================                    ");
                break;
        }
    }
}