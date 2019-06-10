#include "eyebot.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int i = 0;
int v_des = 10000;
int count = 0;
static int v_act;
static int enc_old;
int scale = 50;
int LCD_scale = 50;
FILE *fp = NULL;

void timed_function(void);
void onoff_controller();
void disp_LCD();

int main() {
    TIMER t1;
    TIMER t2;
    fp = fopen("/home/pi/usr/bangbang.csv", "w");
    LCDInit();
    usleep(1000);
    LCDClear();
    t1 = OSAttachTimer(scale, &onoff_controller);
    t2 = OSAttachTimer(LCD_scale, &disp_LCD);

    while (count < 1000) {
        //onoff_controller();
        //disp_LCD();
    }
    
    OSDetachTimer(t1);
    OSDetachTimer(t2);

    return 0;
}

void onoff_controller() {

    int enc_new, r_mot;

    enc_new = ENCODERRead(1);
    if(enc_new < enc_old) {
        r_mot = 100;
    }
    else {
        v_act = (enc_new - enc_old)*(1000/scale);
        if (v_act < v_des) {
            r_mot = 100;
        }
        //else if(v_act < 100) {
        //    r_mot = -100;
        //}
        else {
            r_mot = 0;
        }
    }
    MOTORDrive(1, r_mot);
    //MOTORDrive(2, r_mot);

    enc_old = enc_new;
   
}

void timed_function(void) {
    i++;
    printf("i: %d\n", i);
}


void disp_LCD() {
    if(count <= 1000) fp = fopen("/home/pi/usr/bangbang.csv", "a");
    LCDClear();
    LCDPrintf("%i\n", count);
    LCDPrintf("Motor Speed: %i\n", v_act);
    LCDPrintf("Encoder Value: %i\n", enc_old);
    fprintf(fp, "%i\n", v_act);
    fclose(fp);
    count++;
}
