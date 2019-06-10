#include "eyebot.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int i = 0;
int count = 0;
// -----------------------------
// DEFINE VARIABLES TO SET
#define v_des 2000
#define control_scale
int scale = 10;
int LCD_scale = 10;
// -----------------------------

float kp = 0.03;
float ki = 0.01;
float kd = 0.001;
static int v_act;
static int enc_old;

FILE *fp = NULL; // what the heck

void timed_function();
void onoff_controller();
void disp_LCD();
void writeCSV();

int v_act_old;

int main(int argc, char *argv[]) {
    /*
    kp = (float)atof(argv[1]);
    ki = (float)atof(argv[2]);
    kd = (float)atof(argv[3]);
     */


    LCDInit(); // Start the LCD
    TIMER t1 = OSAttachTimer(scale, &onoff_controller); //Attach that mf timer
    TIMER t2 = OSAttachTimer(LCD_scale, &disp_LCD);

    while (/*KEYRead() != KEY4*/count < 400) {
        usleep(100);
    }

    OSDetachTimer(t1);
    OSDetachTimer(t2);

    return 0;
}

void onoff_controller() {
    static float e_func_float1;
    static float e_func_float2;
    static float r_mot = 50.0;
    int r_mot_int;
    static int enc_new = 0;
    enc_new = ENCODERRead(1);
    v_act = (enc_new - enc_old) * (1000/scale);
    //v_act = (enc_new - enc_old);

    int e_func = (v_des - v_act);
    float e_func_float = (float)e_func;
    if (e_func > 0 || e_func < 0) {
        r_mot = r_mot + kp*(e_func_float - e_func_float1) + ki*(e_func_float + e_func_float1)/2 + kd*(e_func_float-2*e_func_float1+e_func_float2);
        printf("Bigger Motor Power: %f\tError Function: %f\tModifier: %f\t V Actual: %d\tkp: %f\n", r_mot, e_func_float, kp*e_func_float, v_act, kp);

    }
    /*
    else if (e_func < 0) {
        //r_mot = (r_mot*control_scale + (kp * e_func))/control_scale;
        r_mot = r_mot + kp*(e_func_float - e_func_float1) + ki*(e_func_float + e_func_float1)/2 + kd*(e_func_float-2*e_func_float1+e_func_float2);
        printf("Smaller Motor Power: %f\tError Function: %f\tModifier: %f\t V Actual: %d\tkp: %f\n", r_mot, e_func_float, kp*e_func_float, v_act, kp);

    }
     */
    else {
        r_mot = r_mot;
    }

    if(r_mot > 100.00) {
        r_mot = 100.00;
    }
    else if (r_mot < -100.00) {
        r_mot = -100.00;
    }
    r_mot_int = (int)r_mot;
    MOTORDrive(1, r_mot_int);
    //MOTORDrive(2, r_mot);

    e_func_float2 = e_func_float1;
    e_func_float1 = e_func_float;

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
    static int j = 0;
    //LCDPrintf("%i\n", count);
    //LCDPrintf("Motor Speed: %i\n", v_act);
    //LCDPrintf("Encoder Value: %i\n", enc_old);
    LCDLine(i+j,(-1*(v_act_old-4500))/50+150,i+j+2,(-1*(v_act-4500))/50+150,RED);
        v_act_old = v_act;
    i++;
    j = j + 2;
    printf("%i\n", i+j);

    if(i+j >= 480) {
        LCDClear();
        j = 0;
        i = 0;
        printf("\n\n\n\n\n\n\n");
    }
}