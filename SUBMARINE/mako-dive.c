// Mako Basic Movement Demo, M. Finn May 2018
// extended by T. Braunl, June 2018
#include "eyebot.h"

// Thruster IDs.
#define LEFT   1
#define FRONT  2
#define RIGHT  3
#define BACK   4

#define PSD_DOWN 6   // PSD direction

void dive(int speed)
{ MOTORDrive(FRONT, speed);
  MOTORDrive(BACK,  speed);
}

int main()
{ BYTE img[QVGA_SIZE];
  char key;
  
  LCDMenu("DIVE", "STOP", "UP", "END");
  CAMInit(QVGA);
  do { LCDSetPrintf(19,0, "Dist to Ground:%6d\n", PSDGet(PSD_DOWN));
       CAMGet(img);
       LCDImage(img);

       switch(key=KEYRead())
       { case KEY1: dive(-100);  break;
         case KEY2: dive(   0);  break;
         case KEY3: dive(+100);  break;
       }
     } while (key != KEY4);
  return 0;  
}