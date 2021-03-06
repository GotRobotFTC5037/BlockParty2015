#pragma config(Hubs,  S1, HTMotor,  HTMotor,  none,     none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     ,               sensorI2CCustom)
#pragma config(Motor,  mtr_S1_C1_1,     LF,            tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     RF,            tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     LR,            tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     RR,            tmotorTetrix, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "joystickdriver.c"


#include "drivers/common.h"
#include "drivers/hitechnic-sensormux.h"
#include "drivers/hitechnic-gyro.h"
#include "drivers/hitechnic-magfield.h"
#include "drivers/hitechnic-irseeker-v2.h"

#include "drivers/lego-touch.h"

const tMUXSensor HTGYRO = msensor_S2_1;
const tMUXSensor HTIRS2 = msensor_S2_4;

#define max(a, b)               (((a) > (b))? (a): (b))
#define min(a, b)               (((a) < (b))? (a): (b))


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// float heading=0;
int bearingAC = 0;
int gyroCalTime = 3;
int calibrate = 0;
float constHeading=0;
float relHeading=0;
long rotation_factor=0;
const long ROTATION_GAIN = 5;
//const int sampletime = 100;
float newgyro = 0;
long highest, lowest;
float currDir = 0.0;
float prevDir = 0.0;
bool SMUX_good = false;


float LF_angle=45;
float RF_angle=135;
float LR_angle=315;
float RR_angle=225;

float LF_speed, RF_speed, LR_speed, RR_speed;
int gyro_angle=0;
int x1, y1;
float drive_speed;
float drive_angle, hypoteneuse;



float Current_Angle=0;
long Current_Speed=0;

//=================================================================
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
float  MyHTCal(long caltime)
{
	float average = 0;
	long starttime = nPgmTime;
	long samples=0;
	long data;
	highest =  -10000;
	lowest = 10000;
	while (nPgmTime < starttime+caltime)
	{
		samples +=1;
		data = HTGYROreadRot(HTGYRO);
		average += (float)data;
		if (highest < data) highest = data;
		if (lowest> data) lowest = data;
	}
	return average/samples;
}
//------------
task gyro()
{
	long currtime,prevtime;
	int acS[5];
	while (HTSMUXreadPowerStatus(S2))  // check battery power is on
	{
		PlayTone(750,25);
		wait1Msec(500);
	}
	SMUX_good = true;
	// testvalue = HTGYROreadRot(HTGYRO);  // check gyro value appears meaningful
	// while (abs(testvalue)>50)
	// {
	// PlayTone(500,25);
	// wait1Msec(500);
	// }
	while(calibrate != 1){};
	wait1Msec(300);
	HTGYROstartCal(HTGYRO);
	float drift = MyHTCal(gyroCalTime*1000);

	for (int i=0;i<5;i++)            // check if there is too much spread in the data
	{
		if (abs(highest-lowest)>10)
		{
			PlayTone (250,25);
			wait1Msec(500);
		}
	}
	calibrate = 2;
	prevtime = nPgmTime;
	while(true)
	{
		//nxtDisplayBigTextLine(0, "G: %3f", relHeading);
		//nxtDisplayBigTextLine(2, "AC: %3d", bearingAC);
		//nxtDisplayBigTextLine(6, "IR: %2.2f", currDir);
		currtime=nPgmTime;
		newgyro = (float)HTGYROreadRot(HTGYRO);
		constHeading += (newgyro - drift) * (float)(currtime-prevtime)/1000;
		relHeading += (newgyro - drift) * (float)(currtime-prevtime)/1000;
		prevtime = currtime;
		wait1Msec(1);

		// now read the IR sensor values for both bearing and each of the 5 segments
		// this code courtesy of MHTS - RobotC forums
		/*	bearingAC = HTIRS2readACDir(HTIRS2);

		currDir = (float) bearingAC;
		if (bearingAC == 0)
		{
		currDir = prevDir;
		}
		else
		{if (HTIRS2readAllACStrength(HTIRS2, acS[0], acS[1], acS[2], acS[3], acS[4]))
		{
		bearingAC = (bearingAC - 1)/2;
		if ((bearingAC < 4) && (acS[bearingAC] != 0) && (acS[bearingAC + 1] != 0))
		{
		currDir += (float)(acS[bearingAC + 1] - acS[bearingAC])/
		max(acS[bearingAC], acS[bearingAC + 1]);
		}
		}
		}
		*/
		prevDir = currDir;
	}
}



task keypad()
{
	while(true)
	{
		if (nNxtButtonPressed==1)
		{
			while(nNxtButtonPressed==1){};
			gyro_angle+=1;
		}
		if (nNxtButtonPressed==2)
		{
			while(nNxtButtonPressed==2){};
			gyro_angle-=1;
		}
	}
}


task main()
{
	disableDiagnosticsDisplay();
	StartTask(keypad);
	StartTask(gyro);

	calibrate=1;
	wait10Msec(400);
	//waitForStart();

	while(true)
	{
		getJoystickSettings(joystick);							// get joystick data packet
		x1 = (joystick.joy1_x1*100)/127;						// extract X value for joystick one
		y1 = -(joystick.joy1_y1*100)/127;						// extract Y value for joystick one
		hypoteneuse=(float)sqrt((x1*x1)+(y1*y1));		// calculate third side of effective triangle
		if ((abs(x1)<10) && (abs(y1)<10))						// if joystick is in zero position
		{
			drive_speed=0;														// then set speed and effective angle to zero
			drive_angle=0;
		}
		else																				// joystick is active, so calculate motor power values
		{
			drive_angle=radiansToDegrees(acos(y1/hypoteneuse)); 	// calculate the desired drive direction
			if (x1<0)																							// if direction is backwards (south)
			{
				drive_angle=360-drive_angle;												// then recalculate the angle accordingly
			}
			drive_speed=hypoteneuse;									// the speed is defined by the extent of the hypoteneuse
		}

		if (drive_speed>100) drive_speed = 100;			// limit max speed to 100 percent

		gyro_angle=constHeading;

		LF_speed = cosDegrees(LF_angle-drive_angle+gyro_angle)*drive_speed; // individual motor speeds are calculated
		RF_speed = cosDegrees(RF_angle-drive_angle+gyro_angle)*drive_speed; // relative to the mounted angle of that
		LR_speed = cosDegrees(LR_angle-drive_angle+gyro_angle)*drive_speed; // motor, but adjusted for the current
		RR_speed = cosDegrees(RR_angle-drive_angle+gyro_angle)*drive_speed; // orientation of the robot per the GYRO

		//====================================================================
		// Now handle request for rotation, either standalone or while driving
		//====================================================================
		rotation_factor=0;											// assume no rotation is requested
		int speed_limit=drive_speed;
		if (joy1Btn(3)) rotation_factor=15;			// clockwise rotation
			if (joy1Btn(2)) rotation_factor=-15;		// counter clockwise rotation
			speed_limit-=abs(rotation_factor);


		//=================================================================================
		// now prorate the power levels so the highest equals the requested power level
		//=================================================================================
		float highest = max(max(abs(LF_speed),abs(RF_speed)),max(abs(LR_speed),abs(RR_speed)));
		float adjust_factor=1;
		if (highest!=0) adjust_factor=speed_limit/highest;
		LF_speed*=adjust_factor;
		RF_speed*=adjust_factor;
		LR_speed*=adjust_factor;
		RR_speed*=adjust_factor;

		LF_speed+=rotation_factor;
		RF_speed+=rotation_factor;
		LR_speed+=rotation_factor;
		RR_speed+=rotation_factor;
		//=======================================================================
		// finally operate the actual drive motors at the calculated power levels
		//=======================================================================
		motor[LF]=(int)LF_speed;
		motor[RF]=(int)RF_speed;
		motor[LR]=(int)LR_speed;
		motor[RR]=(int)RR_speed;

		nxtDisplayBigTextLine(0, "X%03d %03.0f",x1,drive_speed);
		nxtDisplayBigTextLine(2, "Y%3.1f %03.0f",constHeading, drive_angle);
		nxtDisplayBigTextLine(4, "%03.0f %03.0f",LF_speed, RF_speed);
		nxtDisplayBigTextLine(6, "%03.0f %03.0f",LR_speed, RR_speed);

		//88888888888888888888888888888888888888888
		if(joy1Btn(7))
		{
			motor[LF] = 100;
		}

		//88888888888888888888888888888888888888888
	}
}
