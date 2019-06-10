#include "eyebot.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int i = 0;
int count = 0;
// -----------------------------
// DEFINE VARIABLES TO SET
#define kp 1
#define v_des 5000
#define control_scale 1000
int scale = 10;
int LCD_scale = 50;
// -----------------------------

static int v_act;
static int enc_old;

FILE *fp = NULL; // what the heck

void timed_function();
void onoff_controller();
void disp_LCD();
void writeCSV();

int main(int argc, char *argv[]) {
    //kp = (int)argv[1];
    LCDInit(); // Start the LCD
    TIMER t1 = OSAttachTimer(scale, &onoff_controller); //Attach that mf timer
    TIMER t2 = OSAttachTimer(LCD_scale, &disp_LCD);

    while (/*KEYRead() != KEY4*/count < 1000) {
        usleep(100);
        //count++;
        //disp_LCD();
    }
    
    OSDetachTimer(t1);
    OSDetachTimer(t2);

    return 0;
}

void onoff_controller() {
    static int r_mot = 50;   
    static int enc_new;
    enc_new = ENCODERRead(1);
    v_act = (enc_new - enc_old) * (1000/scale);
    //v_act = (enc_new - enc_old);

    int e_func = (v_des - v_act);
    if (e_func > 0) {
        r_mot = (r_mot + (kp * e_func)/control_scale);
        //r_mot = (r_mot*control_scale + (kp * e_func))/control_scale;
        printf("Bigger Motor Power: %i\tError Function: %i\tModifier: %i\t V Actual: %i\n", r_mot, e_func, kp*e_func/control_scale, v_act);
        //printf("Bigger Motor Power: %i\tError Function: %i\tModifier: %i\t V Actual: %i\n", r_mot, e_func, kp*e_func/control_scale, v_act);
        //printf("Bigger %i\n", r_mot);
    }
    else if (e_func < 0) {
        //r_mot = (r_mot*control_scale + (kp * e_func))/control_scale;
        r_mot = (r_mot + ( kp * e_func)/control_scale);
        printf("Smaller Motor Power: %i\tError Function: %i\tModifier: %i\t V Actual: %i\n", r_mot, e_func, kp*e_func/control_scale, v_act);
        //printf("Smaller %i\n", r_mot);
        //r_mot = ((1000-kp) * e_func)/1000;
    }
    //else if(v_act < 100) {
    //    r_mot = -100;
    //}
    else {
        r_mot = r_mot;
    }

    if(r_mot > 100) {
        r_mot = 100;
    }
    else if (r_mot < -100) {
        r_mot = -100;
    }

    MOTORDrive(1, r_mot);
    //MOTORDrive(2, r_mot);

    enc_old = enc_new;
    writeCSV(v_act);
}

void timed_function(void) {
    i++;
    printf("i: %d\n", i);
}

void writeCSV(int v) {
    if (count == 0) fp = fopen("/cygdrive/c/Users/David/Documents/Programming/myscripts/test_kp0.csv", "w");
    if(count <= 1000) fp = fopen("/cygdrive/c/Users/David/Documents/Programming/myscripts/test_kp0.csv", "a");
    fprintf(fp, "%i\n", v);
    fclose(fp);
    count++;
}

void disp_LCD() {
    LCDClear(); 
    LCDPrintf("%i\n", count);
    LCDPrintf("Motor Speed: %i\n", v_act);
    LCDPrintf("Encoder Value: %i\n", enc_old);
    LCDPixel(i%640, v_act/100+100, WHITE);
    i++;
}