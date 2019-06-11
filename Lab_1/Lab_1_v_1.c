#include <stdio.h>
#include "eyebot.h"




void straight(){
    // function that will go straight and check sensors until they are below limit
    VWStraight(50, 30);
    VWWait();
    float a = PSDGet(1);
    printf("%f", a);
}

int main() {
    straight();
}