// Mako Basic Movement Demo, M. Finn May 2018
// extended by T. Braunl, June 2018
#include "eyebot.h"

// Thruster IDs.
#define LEFT   1
#define FRONT  2
#define RIGHT  3
#define BACK   4

// PSD direction
#define PSD_FRONT       1
#define PSD_FRONT_LEFT  2
#define PSD_FRONT_RIGHT 3
#define PSD_BACK_LEFT   4
#define PSD_BACK_RIGHT  5
#define PSD_DOWN        6 

void drive(int l_speed, int r_speed)
{ MOTORDrive(LEFT,  l_speed);
  MOTORDrive(RIGHT, r_speed);
}

void PSD_All(int psd[])
{ int i;
  for (i=1; i<=6; i++) psd[i] = PSDGet(i);
}


int main()
{ BYTE img[QVGA_SIZE];
  int  psd[7];
  char key;
  
  LCDMenu("FORWARD", "LEFT", "RIGHT", "END");
  CAMInit(QVGA);
  do { CAMGet(img);
       LCDImage(img);
       PSD_All(psd);
       LCDSetPrintf(19,0, "Dist to Front:%6d\n", psd[PSD_FRONT]);

       switch(key=KEYRead())
       { case KEY1: drive( 100,  100);  break;
         case KEY2: drive(-100,  100);  break;
         case KEY3: drive( 100, -100);  break;
       }
     } while (key != KEY4);
  return 0;  
}
