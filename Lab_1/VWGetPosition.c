
#include <stdio.h>
#include "eyebot.h"
#include <time.h>

int x, y, phi;

void travel(){
    VWStraight(400,100 );
    VWWait();

    VWTurn(-90, 100);
    VWWait();

    VWStraight(200,100 );
    VWWait();
}


int main() {
    SIMSetRobot(0,1000,1000,1,0);

    VWSetPosition(x, y, phi);
    VWGetPosition(&x, &y, &phi);
    printf("\nx: %d, y: %d, phi: %d\n", x, y, phi);

    travel();

    VWGetPosition(&x, &y, &phi);
    printf("\nx: %d, y: %d, phi: %d\n", x, y, phi);



    return 0;
}