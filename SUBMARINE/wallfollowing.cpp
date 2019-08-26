/**
 * This program was designed as a tester for SubSim.  It should appear, however, almost
 * identical to a regular C program written for the Eyebot.
 */


#include "eyebot.h"
#include <stdio.h>

/**
 * Main program
 */
int main(int argc, char* argv[])
{
	int psdRange = 20;
	int maxScalingValue = 1023;
	
	LCDPrintf("Starting Wall Following testing program\n");
	LCDPrintf("Press KEY4 to stop\n");
	
	printf("Starting Wall Following testing program");
	
	MotorHandle motorL = 0;
	MotorHandle motorR = 0;

	PSDHandle psdF = 0;
	PSDHandle psdB = 0;
	PSDHandle psdL = 0;
	PSDHandle psdR = 0;

	motorL = MOTORInit(MOTOR_LEFT);
	motorR = MOTORInit(MOTOR_RIGHT);

	psdF = PSDInit(PSD_FRONT);
	psdB = PSDInit(PSD_BACK);
	psdL = PSDInit(PSD_LEFT);
	psdR = PSDInit(PSD_RIGHT);
	
	MOTORDrive(motorL,10);
	MOTORDrive(motorR,10);

	int df=0;
	int db=0;
	int dl=0;
	int dr=0;
	int vl=0;
	int vr=0;
	int key=0;
	
	while( (key=KEYRead())!=KEY4 )
	{
		df = PSDGet(psdF) * psdRange / maxScalingValue;
		db = PSDGet(psdB) * psdRange / maxScalingValue;
		dl = PSDGet(psdL) * psdRange / maxScalingValue;
		dr = PSDGet(psdR) * psdRange / maxScalingValue;

		printf("F:%2d B:%2d L:%2d R:%2d\n",df, db, dl, dr);

		// set default as driving forward
		vl=5;
		vr=5;

		// check if sub is about to hit a wall
		if(df<10)
		{
			vl=0;
		}
		else
		{
			// check if too close to right wall
			if(dr<2)
			{
				vl=0;
			}
			if(dr>5)
			{
				vr=3;
			}
		}

		MOTORDrive(motorL,vl * 10);
		MOTORDrive(motorR,vr * 10);

		OSWait(10);
	}

	/* release all PSDs */
	PSDRelease();

	/* release motors */
	MOTORDrive(motorL,0);
	MOTORDrive(motorR,0);
	MOTORRelease(motorL);
	MOTORRelease(motorR);

	printf("Exiting Wall Following testing program");
	return 0;
}
