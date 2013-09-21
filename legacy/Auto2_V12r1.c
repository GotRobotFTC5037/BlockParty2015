#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTServo)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     IR_MUX,         sensorI2CCustom)
#pragma config(Sensor, S3,     GYRO_MUX,       sensorI2CCustom)
#pragma config(Sensor, S4,     THRD_MUX,       sensorI2CCustom)
#pragma config(Motor,  motorA,          lightMotor,    tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C1_1,     LF_motor,      tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     RF_motor,      tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     LR_motor,      tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     RR_motor,      tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     lifterMotor,   tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     lightStrip,    tmotorTetrix, openLoop, reversed)
#pragma config(Servo,  srvo_S1_C4_1,    left_servo,           tServoStandard)
#pragma config(Servo,  srvo_S1_C4_2,    shoulder,             tServoStandard)
#pragma config(Servo,  srvo_S1_C4_3,    wrist,                tServoStandard)
#pragma config(Servo,  srvo_S1_C4_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_6,    right_servo,          tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//
//=====================================================================================
//
//  ***    ***   *****      ****    ***   ****    ***   *****     ***
// *   *  *   *    *        *   *  *   *  *   *  *   *    *      *   *
// *      *   *    *        *   *  *   *  *   *  *   *    *          *
// *  **  *   *    *        ****   *   *  ****   *   *    *         *
// *   *  *   *    *        * *    *   *  *   *  *   *    *        *
// *   *  *   *    *        *  *   *   *  *   *  *   *    *
//  ***    ***     *        *   *   ***   ****    ***     *        *
//
//=====================================================================================
//
// FTC TEAM 5037
// Elgin Technlogy Center, Illinois
// Ring It Up (2012-2013)
// Autonomous Template
//
//==========================================================================
// Include files for FTC hardware, 3rd party sensors and sensor multiplexors
//==========================================================================
#include "joystickdriver.c"
#include "drivers/hitechnic-sensormux.h"
#include "drivers/hitechnic-gyro.h"
#include "drivers/hitechnic-magfield.h"
#include "drivers/hitechnic-irseeker-v2.h"
#include "drivers/lego-touch.h"
#include "drivers/lego-ultrasound.h"
#include "drivers/lego-light.h"

//=============================================================
// Define constants used in various autonomous driving routines
//=============================================================
#define MOVERIGHT true
#define MOVELEFT false
#define BACK 1
#define SIDE 2
#define SIDE_BACK 3
#define ByTIME 4
#define REL false
#define CONSTANT true

//========================================================================
// Include got robot? library routines and user interface tasks
//========================================================================
#include "GR_robot_specific.h"
#include "GR_library.h"
#include "GR_user_interface.h"

//====================================
// Gyro Calibration helper function
//====================================
float  MyHTCal(long caltime)
{
	long highest = -1000, lowest = 10000;
	float average = 0;
	long starttime = nPgmTime;
	long samples=0;
	long data;
	while (nPgmTime < starttime+caltime)			// loop for the requested number of seconds
	{
		samples +=1;														// count the number of iterations for averaging
		data = HTGYROreadRot(HTGYRO);						// get a new reading from the GYRO
		average += (float)data;									// add in the new value to the average
		if (highest < data) highest = data;			// adjust the highest value if necessary
			if (lowest> data) lowest = data;				// likewise for the lowest value
	}
	gyro_noise=abs(highest-lowest);						// save the spread in the data for diagnostic display
	return average/samples;										// and return the average drift
}
//===================================================
// task to read in all sensors to workspace variables
//===================================================
task sensors()
{
	float currDir = 0.0; //prevDir = 0.0,
	long currtime,prevtime;
	LSsetActive(LEGOLS);							// set the LEGO light sensor to active mode
	//-------------------------
	// gyro
	//-------------------------
	ir_mux_status=HTSMUXreadPowerStatus(IR_MUX);				// read the sensor multiplexor status
	gyro_mux_status=HTSMUXreadPowerStatus(GYRO_MUX);		// read the sensor multiplexor status
	while (ir_mux_status || gyro_mux_status)  					// check good battery power on both muxes
	{
		PlayTone(750,25);																	// if not we beep indefinitely
		wait1Msec(500);
	}
	//SMUX_good = true;
	while(calibrate != 1){};														// wait for a request to start calibrating the gyro
	wait1Msec(300);																			// short delay to ensure that user has released the button
	HTGYROstartCal(HTGYRO);															// initiate the GYRO calibration
	drift = MyHTCal(gyroCalTime*1000);
	Driver_Cal = HTGYROreadCal(HTGYRO);						// read the calculated calibration value for saving to file

	//---------------------------------------
	// write the GYRO calibration data to file for Tele-Op
	//---------------------------------------
	Delete(sFileName, nIoResult);												// delete any pre-existing file
	nFileSize = 100;																		// new file size will be 100 bytes
	OpenWrite(  hFileHandle, nIoResult, sFileName, nFileSize);	// create and open the new file
	WriteFloat( hFileHandle, nIoResult, drift);					// write the current drift value to the file
	WriteFloat( hFileHandle, nIoResult, Driver_Cal);		// write the driver calibration to the file
	Close(hFileHandle, nIoResult);											// close the file
	//---------------------------------------

	for (int i=0;i<5;i++)            // check if there is too much spread in the data
	{
		if (gyro_noise>10)						// if there is too much spread we beep 5 times to alert the drive team
		{
			PlayTone (250,25);
			wait1Msec(500);
		}
	}
	calibrate = 2;										// this signifies to the main program that calibration has been completed
	prevtime = nPgmTime;
	while(true)
	{
		currtime=nPgmTime;
		rawgyro = HTGYROreadRot(HTGYRO);
		constHeading += (rawgyro - drift) * (float)(currtime-prevtime)/1000;
		relHeading += (rawgyro - drift) * (float)(currtime-prevtime)/1000;
		prevtime = currtime;
		//wait1Msec(1);
		//---------------------------------------------------------------------
		// Read both sonar sensors and filter out non-valid echo readings (255)
		// If there is no echo the filter just retains the last good reading
		//---------------------------------------------------------------------
		sonarRaw = USreadDist(LEGOUS);								// read the rear mounted sensor
		if (sonarRaw!=255) sonarLive = sonarRaw;			// and copy valid results to workspace
			sonarRaw2 = USreadDist(LEGOUS2);							// read the side mounted sensor
		if (sonarRaw2!=255) sonarLive2 = sonarRaw2;		// and copy valid results to workspace

		//-------------------------
		// LEGO light sensor
		//-------------------------
		light_normalised = LSvalNorm(LEGOLS);				// read the LEGO light sensor

		//-------------------------
		// HiTechnic IR Sensor
		//-------------------------
		bearingAC = HTIRS2readACDir(HTIRS2);				// Read the IR bearing from the sensor
		currDir = (float) bearingAC;								// copy into workspace -
		/*if (bearingAC == 0)													// No IR signal is being detected
		{
		currDir = prevDir;												// so retain the previous reading
		}
		else																				// otherwise read all the IR segments
		{
		{
		bearingAC = (bearingAC - 1)/2;
		if ((bearingAC < 4) && (acS[bearingAC] != 0) && (acS[bearingAC + 1] != 0))
		{
		currDir += (float)(acS[bearingAC + 1] - acS[bearingAC])/
		max(acS[bearingAC], acS[bearingAC + 1]);
		}
		}
		}
		prevDir = currDir;
		IR_Bearing=currDir-5;						// and setup the main variable for others to use
		*/
		HTIRS2readAllACStrength(HTIRS2, acS[0], acS[1], acS[2], acS[3], acS[4]);
		if (bearingAC!=0)								// we have a valid IR signal
		{
			int maximum = -1;
			int peak = 0, offset=0;
			for (int i=0;i<5;i++)	// scan array to find the peak entry
			{	if (acS[i]>maximum)
				{peak = i;
					maximum = acS[i];
				}
			}
			offset=0;
			if ((peak < 4) && (peak>0) && (acS[peak] != 0))  // we are not working with extreme value
			{
				if (acS[peak-1]!=acS[peak+1]) // if the values either side of the peak are identical then peak is peak
				{
					if (acS[peak-1]>acS[peak+1])	// otherwise decide which side has higher signal
					{
						offset = -25*(1-(float)(acS[peak]-acS[peak-1])/		// calculate the bias away from the peak
						max(acS[peak], acS[peak-1]));
					}
					else
					{
						offset = 25*(1-(float)(acS[peak]-acS[peak+1])/
						max(acS[peak], acS[peak+1]));
					}
				}
			}
			IR_Bearing = (float)((peak-2)*50) + offset;		// direction is the total of the peak bias plus the adjacent bias
			// range is -100 to +100, zero is straight ahead
		}
	}
}

//===================================================
// Function to turn the robot with gyro V2
//===================================================
void Gyro_TurnV2 (float degrees, int power, bool ConstOrRel)
{
	relHeading=0;
	if (degrees<0)
	{
		motor[LF_motor]=power;     // start turning left
		motor[LR_motor]=power;
		motor[RF_motor]=power;     // start turning left
		motor[RR_motor]=power;
	}
	else
	{
		PlaySound(soundBeepBeep);
		motor[LF_motor]=-power;      // start turning right
		motor[LR_motor]=-power;
		motor[RF_motor]=-power;      // start turning right
		motor[RR_motor]=-power;
	}
	if(gyro_mux_status==false)		// the GYRO mux seems to be active
	{
		if(ConstOrRel)						// decide whether to turn relative to start position
			// or relative to current robot orientation
		{
			while(abs((int)degrees) > abs((int)constHeading)){}	// relative to field
		}
		else
		{
			while(abs((int)degrees) > abs((int)relHeading)){}		// relative to robot start posn
		}
	}
	else												// we don't have a good gyro reading
	{
	}
	StopRobot();						// once done we shutdown all drive motors
}


//========================================================================
// Gyro turn with only 3 motors, for now
//========================================================================
void Gyro3MT_V2 (float degrees, int power, bool ConstOrRel, bool LorR)
{
	relHeading=0;
	if (degrees<0)
	{
		if(LorR == true)
		{
			motor[LF_motor]=power/2;
			motor[RF_motor]=-1;
			motor[RR_motor]=power/2;
			motor[LR_motor]=power;
		}
		else
		{
			motor[LF_motor]=-1;
			motor[RF_motor]=power/2;
			motor[RR_motor]=power;
			motor[LR_motor]=power/2;
		}
	}
	else
	{
		PlaySound(soundBeepBeep);
		if(LorR == true)
		{
			motor[LF_motor]=power/2;
			motor[RF_motor]=-1;
			motor[RR_motor]=power/2;
			motor[LR_motor]=power;
		}
		else
		{
			motor[LF_motor]=-1;
			motor[RF_motor]=power/2;
			motor[RR_motor]=power;
			motor[LR_motor]=power/2;
		}
	}
	if(gyro_mux_status==false)				// the GYRO mux seems to be active
	{
		if(ConstOrRel) while(abs((int)degrees) > abs((int)constHeading)){}

		else while(abs((int)degrees) > abs((int)relHeading)){}
	}
	StopRobot();										// shutdown all drive motors
}

// =======================================================================
// Function to move the robot by the gyro by time V2
// =======================================================================
void GyroTime_moveV2(int time, int power,bool StopWhenDone, bool adjust, bool ConstOrRel)
{
	long adj_power;
	long adj_deg;
	long current_power;
	relHeading =0;
	wait1Msec(200);
	motor[LF_motor] = -power;
	motor[RF_motor] = power;
	motor[LR_motor] = -power;
	motor[RR_motor] = power;
	current_power = power;
	ClearTimer(T1);
	bool Done = false;
	while(!Done)
	{
		if(time1[T1] > time)
		{
			Done = true;
		}

		if(adjust == true)
		{
			if(ConstOrRel) adj_deg = (long) constHeading;
			else adj_deg = (long) relHeading;
			adj_power = adj_deg*GYRO_PROPORTION;

			motor[LF_motor] = -(current_power - adj_power);
			motor[RF_motor] = (current_power + adj_power);
			motor[LR_motor] = -(current_power);
			motor[RR_motor] = (current_power);
		}
	}
	if(StopWhenDone==true)
	{
		StopRobot();
	}
}
// =======================================================================
// Function to move the robot by the gyro by time sideways V2
// =======================================================================
void GyroTimeS_moveV2(int time, int power, bool light,bool StopWhenDone, bool adjust, bool ConstOrRel)
{
	long adj_power;
	long adj_deg;
	long current_power;
	int i = 0;
	relHeading =0;
	wait1Msec(200);
	motor[LF_motor] = -power;
	motor[RF_motor] = -power;
	motor[LR_motor] = power;
	motor[RR_motor] = power;
	current_power = power;
	ClearTimer(T1);
	bool Done = false;
	int addPower = 0;
	while(!Done)
	{
		RequestedScreen=S_LIGHT;		// force the light value screen to be displayed
		if(light == true)
		{
			if(light_normalised > light_threshold) i++;
			if(i > 5) Done = true;			// was 10, but that may be too high, now 5 at 21 Nov 2012
		}

		if(time1[T1] > time)
		{
			Done = true;
		}

		if(adjust == true)
		{
			if(ConstOrRel) adj_deg = (long) constHeading;
			else adj_deg = (long) relHeading;
			adj_power = adj_deg*GYRO_PROPORTION;

			addPower = (current_power*5)/100;

			if(power > 0)
			{
				motor[LF_motor] = -(current_power-addPower); //more
				motor[RF_motor] = -((current_power+addPower) - adj_power);
				motor[LR_motor] = (current_power-addPower); //more
				motor[RR_motor] = ((current_power+addPower) + adj_power);
			}
			else
			{
				motor[LF_motor] = -(current_power+addPower);
				motor[RF_motor] = -((current_power-addPower) - adj_power);
				motor[LR_motor] = (current_power+addPower);
				motor[RR_motor] = ((current_power-addPower) + adj_power);
			}
		}
	}
	if(StopWhenDone==true)
	{
		StopRobot();
	}
}
// =======================================================================
// Function to move the robot by the gyro and the sonar sensors V2
// =======================================================================
void GyroSonar_moveV2(int time, int WhichSensors, int distanceX, int distanceY, int power,bool StopWhenDone, bool adjust, bool ConstOrRel)
{

	long adj_power;			// Reset all the values
	long adj_deg;				//
	int count = 0;					//
	int sonar_adj = 0;	//-----------------
	int lastvalue=9999;
	long current_power;
	relHeading =0;			//-----------------
	wait1Msec(20);
	motor[LF_motor] = -power;	//---------------
	motor[RF_motor] = power;	// Set beginning speeds
	motor[LR_motor] = -power; // for the motors
	motor[RR_motor] = power;	//---------------
	current_power = power;
	ClearTimer(T1);
	bool Done = false;
	while(!Done)
	{
		if(WhichSensors != 2)										//----------------
		{																				//
			if(sonarLive2 > distanceY)						// we have exceeded our target distance
			{
				if (sonarLive2!=lastvalue)					// we have a new reading above the target distance
				{
					count++;													// so increase our count of values above distance
					lastvalue=sonarLive2;							// and save the current reading for next time around
				}
			}
			else count=0;													// the measured distance is below the target distance
				if(count > 2) Done = true;						// once we get two readings above target we exit the loop
		}																				//
		else if(time1[T1] > time) Done = true;	//----------------

		if(adjust == true)
		{
			if(WhichSensors != 1) sonar_adj = ((distanceX - sonarLive)*SONAR_PROPORTION);
			if(ConstOrRel) adj_deg = (long) constHeading;
			else adj_deg = (long) relHeading;
			adj_power = adj_deg*GYRO_PROPORTION;
			/*
			motor[LF_motor] = -(current_power + adj_power + sonar_adj);
			motor[RF_motor] = current_power + adj_power - sonar_adj; // was +
			motor[LR_motor] = -(current_power + adj_power - sonar_adj);
			motor[RR_motor] = current_power + adj_power + sonar_adj;
			*/
			motor[LF_motor] = -(current_power - (adj_power + sonar_adj));
			motor[RF_motor] = (current_power + (adj_power + sonar_adj));
			motor[LR_motor] = -(current_power);
			motor[RR_motor] = (current_power);
		}
	}
	if(StopWhenDone==true)
	{
		StopRobot();
	}
}
// =======================================================================
// Function to move the robot by the gyro and the sonar sensors V2
// =======================================================================
void GyroSonar_moveV3(int time, int WhichSensors, int distanceX, int distanceY, int power,int direction,bool StopWhenDone, bool adjust, bool ConstOrRel)
{

	long adj_power;			// Reset all the values
	long adj_deg;				//
	int count = 0;					//
	int sonar_adj = 0;	//-----------------
	int lastvalue=9999;
	long current_power;
	relHeading =0;			//-----------------
	wait1Msec(20);
	current_power = power;
	ClearTimer(T1);
	bool Done = false;
	while(!Done)
	{/*
		if(WhichSensors != ByTIME || WhichSensors != SIDE)//----------------
		{																				//
		if(sonarLive2 > distanceY )						// we have exceeded our target distance
		{
		if (sonarLive2!=lastvalue)					// we have a new reading above the target distance
		{
		count++;													// so increase our count of values above distance
		lastvalue=sonarLive2;							// and save the current reading for next time around
		}
		}
		else count=0;													// the measured distance is below the target distance
		if(count > 2) Done = true;						// once we get two readings above target we exit the loop
		}																				//
		else if(time1[T1] > time) Done = true;	//----------------*/
		switch(WhichSensors)
		{
		case SIDE:
			if(time1[T1] > time) Done = true;
			break;
		case SIDE_BACK:
			if(sonarLive2 > distanceY )						// we have exceeded our target distance
			{
				if (sonarLive2!=lastvalue)					// we have a new reading above the target distance
				{
					count++;													// so increase our count of values above distance
					lastvalue=sonarLive2;							// and save the current reading for next time around
				}
			}
			else count=0;													// the measured distance is below the target distance
				if(count > 2) Done = true;
			break;
		case BACK:
			if(sonarLive2 > distanceY )						// we have exceeded our target distance
			{
				if (sonarLive2!=lastvalue)					// we have a new reading above the target distance
				{
					count++;													// so increase our count of values above distance
					lastvalue=sonarLive2;							// and save the current reading for next time around
				}
			}
			else count=0;													// the measured distance is below the target distance
				if(count > 2) Done = true;
			break;
		case ByTIME:
			if(time1[T1] > time) Done = true;
			break;
		}

		if(adjust == true)
		{
			if(WhichSensors != 1) sonar_adj = ((distanceX - sonarLive)*SONAR_PROPORTION);
			if(ConstOrRel) adj_deg = (long) constHeading;
			else adj_deg = (long) relHeading;
			adj_power = adj_deg*GYRO_PROPORTION;
			if(WhichSensors == ByTIME) sonar_adj = 0;
			SetDrive(direction+(-sonar_adj+adj_power),power);
		}
	}
	if(StopWhenDone==true)
	{
		StopRobot();
	}
}
// =======================================================================
// Function to drive the robot towards the IR beacon
// =======================================================================
void IR_TimeMove(int power, long maxtime)
{
	int limitReached = 0;
	long current_power;
	long starttime = nPgmTime;
	ResetDriveEncoders();
	motor[LF_motor] = power;
	motor[RF_motor] = power;
	motor[LR_motor] = power;
	motor[RR_motor] = power;
	current_power = power;
	bool Done = false;
	while(!Done)
	{
		if (nPgmTime> starttime + (maxtime* 1000))
		{
			Done = true;  // the timer has run out, so stop the move
		}
		if(limitReached > 4)
		{
			Done = true;
		}
		float adj_power = (IR_Bearing-5.0)*IR_PROPORTION;
		motor[LF_motor] = (current_power - (int)adj_power);
		motor[RF_motor] = (current_power + (int)adj_power);
	}
	StopRobot();
}

// =======================================================================
// Function to move the robot by the gyro by time V2
// =======================================================================
void GyroTime45_V2(int time, int power,bool StopWhenDone, bool adjust, bool ConstOrRel, bool LorR)
{
	long adj_power;
	long adj_deg;
	long current_power;
	relHeading =0;
	wait1Msec(20);
	if(LorR)
	{
		motor[LF_motor] = 0;
		motor[RF_motor] = power;
		motor[LR_motor] = -power;
		motor[RR_motor] = 0;
	}
	else
	{
		motor[LF_motor] = -power;
		motor[RF_motor] = 0;
		motor[LR_motor] = 0;
		motor[RR_motor] = power;
	}
	current_power = power;
	ClearTimer(T1);
	bool Done = false;
	while(!Done)
	{
		if(time1[T1] > time)
		{
			Done = true;
		}

		if(adjust == true)
		{
			if(ConstOrRel) adj_deg = (long) constHeading;
			else adj_deg = (long) relHeading;
			adj_power = adj_deg*GYRO_PROPORTION;

			if(LorR)
			{
				motor[LF_motor] = 0;
				motor[RF_motor] = (current_power + adj_power);
				motor[LR_motor] = (-current_power - adj_power);
				motor[RR_motor] = 0;
			}
			else
			{
				motor[LF_motor] = (-current_power - adj_power);
				motor[RF_motor] = 0;
				motor[LR_motor] = 0;
				motor[RR_motor] = (current_power + adj_power);
			}
		}
	}
	if(StopWhenDone==true)
	{
		StopRobot();
	}
}

task lightFun()
{
	while(true)
	{
		for(int i=10;i<100;i=i+1)
		{
			motor[lightStrip] = i;
			wait1Msec(5);
		}
		motor[lightStrip] = 100;
		wait1Msec(400);
		for(int i=100;i>10;i=i-1)
		{
			motor[lightStrip] = i;
			wait1Msec(5);
		}
		motor[lightStrip] = 10;
		wait1Msec(400);
	}
}

void initializeRobot()
{
	disableDiagnosticsDisplay();
	RequestedScreen=S_MISSION;			// display mission selection screen at startup
	StartTask(buttons);							// start user interface = buttons
	StartTask(display);							// start user interface - display
	StartTask(sensors);							// sensors task runs continuously
	StartTask(lightFun);
	motor[lightMotor] = 50;					// drop the light sensoor
	wait1Msec(500);									// wait for it to move down
	motor[lightMotor] = 0;					// and shut off the motor
	servo[right_servo] = RIGHT_GRIPPER_START;
	servo[left_servo] = LEFT_GRIPPER_START;
	wait1Msec(100);									// give some stabilizing time before doing GYRO cal
	HTGYROstartCal(HTGYRO);
	alive();
	return;
}

//=========================================
// Main Program
//=========================================
task main()
{
	initializeRobot();
	waitForStart();									// wait for FCS to tell us to go!

	if(calibrate != 2)							// GYRO calibration hasn't been run during the wait time
	{
		gyroCalTime = 3;							// so setup the default calibrate time
		calibrate = 1;								// start the calibration going
		while(calibrate != 2)					// and wait for it to complete before moving the robot
		{
			EndTimeSlice();
		}
	}
	constHeading = 0;								// reset the GYRO headings to eliminate any drift while we waited
	relHeading = 0;									// same thing for relative heading

	wait1Msec(Start_Delay*1000);		// implement the user configurable delay before moving
	PlaySound(soundBeepBeep);				// tell everyone we're about to start going

	servo[wrist] = WRIST_CLOSED;
	servo[shoulder] = SHOULDER_DOWN;
	servo[right_servo] = RIGHT_GRIPPER_START;
	servo[left_servo] = LEFT_GRIPPER_START;

	switch(MissionNumber)						// now go run whichever mission we have been asked to run
	{
		//================================================
		// start on the right side of the blue dispenser delivers to IR, then stops
		//================================================
	case 1:

		GyroSonar_moveV2(0, SIDE_BACK, 100, 108, -55,true, true, CONSTANT);
		GyroTimeS_moveV2(750,40,true,false,false,true);
		Gyro_TurnV2(36,-15,CONSTANT);

		if(abs(IR_Bearing) <=1) column = 2;
		if(IR_Bearing <-1 ) column = 1;
		if(IR_Bearing > 1) column = 3;

		wait1Msec(800);
		GyroTime_moveV2(1200,-30,true,false,false);
		GyroTimeS_moveV2(120,-15,true,false,false,false);
		motor[motorA] = -20;
		wait1Msec(1000);
		motor[motorA] = -3;
		GyroTimeS_moveV2(8000,18,true,true,false,false);
		GyroTimeS_moveV2(8000,-15,true,true,false,false);
		GyroTime_moveV2(1200,-30,true,false,false);

		//if(abs(IR_Bearing) <=1) column = 2;
		//if(IR_Bearing <-1 ) column = 1;
		//if(IR_Bearing > 1) column = 3;

		switch(column)
		{
		case 1:
			GyroTimeS_moveV2(700,-30,false,true,false,false);
			break;
			GyroTimeS_moveV2(8000,30,true,true,false,false);
			GyroTime_moveV2(250,-20,true,false,false);
			GyroTimeS_moveV2(140,-15,false,true,false,false);
			wait1Msec(1500);
			servo[shoulder] = SHOULDER_UP;
			wait1Msec(2000);
			servo[wrist] = WRIST_OPEN;
			wait1Msec(3000);
			GyroTime_moveV2(300,30,true,false,false);
			wait1Msec(3000);
			servo[shoulder] = SHOULDER_DOWN;
			wait1Msec(3000);
			break;

		case 2:
			wait1Msec(1500);
			servo[shoulder] = SHOULDER_UP;
			wait1Msec(1000);
			servo[wrist] = WRIST_OPEN;
			wait1Msec(3000);
			GyroTime_moveV2(400,25,true,false,false);
			wait1Msec(3000);
			servo[shoulder] = SHOULDER_DOWN;
			wait1Msec(3000);
			break;

		case 3:
			GyroTimeS_moveV2(700,30,false,true,false,false);
			break;
			GyroTimeS_moveV2(8000,-30,true,true,false,false);
			GyroTime_moveV2(250,20,true,false,false);
			GyroTimeS_moveV2(140,-15,false,true,false,false);
			wait1Msec(1500);
			servo[shoulder] = SHOULDER_UP;
			wait1Msec(2000);
			servo[wrist] = WRIST_OPEN;
			wait1Msec(3000);
			GyroTime_moveV2(300,30,true,false,false);
			wait1Msec(3000);
			servo[shoulder] = SHOULDER_DOWN;
			wait1Msec(3000);
			break;
		}
		break;
		//================================================
		// CENTER PEG
		//================================================

	case 2:
		GyroSonar_moveV2(0, SIDE_BACK, 100, 108, -55,true, true, CONSTANT);
		GyroTimeS_moveV2(750,40,true,false,false,true);
		Gyro_TurnV2(36,-15,CONSTANT);

		wait1Msec(800);
		GyroTime_moveV2(1080,-30,true,false,false);
		motor[motorA] = -20;
		wait1Msec(1000);
		motor[motorA] = -3;
		GyroTimeS_moveV2(8000,-15,true,true,false,false);
		GyroTimeS_moveV2(8000,18,true,true,false,false);
		GyroTimeS_moveV2(8000,-15,true,true,false,false);
		GyroTimeS_moveV2(90,10,true,false,false,false);
		GyroTimeS_moveV2(180,-15,true,false,false,false);
		GyroTime_moveV2(1200,-30,true,false,false);

		wait1Msec(1500);
		servo[shoulder] = SHOULDER_UP;
		wait1Msec(2000);
		servo[wrist] = WRIST_OPEN;
		wait1Msec(1000);
		GyroTime_moveV2(400,25,true,false,false);
		wait1Msec(1000);
		servo[shoulder] = SHOULDER_DOWN;

		switch(fieldRed)
		{
		case STOP:
			StopRobot();
			break;

		case RED:
			Gyro_TurnV2(42,-15,REL);
			GyroTime_moveV2(1200,50,true,false,REL);
			GyroTime45_V2(490,50,false, true, REL, true);
			GyroTime_moveV2(400,50,true,false,REL);
			break;

		case BLUE:
			Gyro_TurnV2(42,15,REL);
			GyroTime_moveV2(1600,45,true,false,false);
			break;
		}

		break;
		//================================================
		// LEFT PEG
		//================================================

	case 3:
		GyroSonar_moveV2(0, SIDE_BACK, 100, 108, -55,true, true, CONSTANT);
		GyroTimeS_moveV2(750,40,true,true,false,true);
		Gyro_TurnV2(36,-15,CONSTANT);

		wait1Msec(800);
		GyroTime_moveV2(1080,-30,true,false,false);
		motor[motorA] = -20;
		wait1Msec(1000);
		motor[motorA] = -3;
		GyroTimeS_moveV2(8000,-15,true,true,false,false);
		GyroTimeS_moveV2(8000,18,true,true,false,false);
		GyroTimeS_moveV2(8000,-15,true,true,false,false);
		Gyro_TurnV2(36,-15,CONSTANT);

		GyroTime_moveV2(1300,-30,true,false,false);
		GyroTime_moveV2(120,30,true,false,false);

		GyroTimeS_moveV2(420,20,true,false,false,false);
		GyroTimeS_moveV2(8000,15,true,true,false,false);
		GyroTimeS_moveV2(8000,-15,true,true,false,false);
		Gyro_TurnV2(36,-15,CONSTANT);
		GyroTimeS_moveV2(80,-18,true,false,false,false);
		//GyroTimeS_moveV2(80,15,true,false,false,false);
		GyroTime_moveV2(400,-30,true,false,false);
		wait1Msec(1500);
		servo[shoulder] = SHOULDER_UP;
		wait1Msec(2000);
		servo[wrist] = WRIST_OPEN;
		wait1Msec(1000);
		GyroTime_moveV2(600,25,true,false,false);
		wait1Msec(3000);
		servo[shoulder] = SHOULDER_DOWN;

		switch(fieldRed)
		{
		case STOP:
			StopRobot();
			break;

		case RED:
			wait1Msec(3000);
			Gyro_TurnV2(42,-15,REL);
			GyroTime_moveV2(1200,50,true,false,REL);
			GyroTime45_V2(490,50,false, true, REL, true);
			GyroTime_moveV2(400,50,true,false,REL);
			break;

		case BLUE:
			wait1Msec(3000);
			Gyro_TurnV2(42,15,REL);
			GyroTime_moveV2(1600,45,true,false,false);
			break;
		}
		break;
		//================================================
		// RIGHT PEG
		//================================================

	case 4:

		GyroSonar_moveV2(0, SIDE_BACK, 100, 108, -55,true, true, CONSTANT);
		GyroTimeS_moveV2(750,40,true,true,false,true);
		Gyro_TurnV2(36,-15,CONSTANT);

		wait1Msec(800);
		GyroTime_moveV2(1080,-30,true,false,false);
		motor[motorA] = -20;
		wait1Msec(1000);
		motor[motorA] = -3;
		GyroTimeS_moveV2(8000,-15,true,true,false,false);
		GyroTimeS_moveV2(8000,18,true,true,false,false);
		GyroTimeS_moveV2(8000,-15,true,true,false,false);
		Gyro_TurnV2(36,-15,CONSTANT);

		GyroTime_moveV2(1300,-30,true,false,false);
		GyroTime_moveV2(100,30,true,false,false);

		GyroTimeS_moveV2(460,-20,true,false,false,false);

		GyroTimeS_moveV2(8000,-18,true,true,false,false);
		GyroTimeS_moveV2(8000,18,true,true,false,false);
		GyroTimeS_moveV2(8000,-18,true,true,false,false);

		GyroTime_moveV2(240,-30,true,false,false);

		wait1Msec(1500);
		servo[shoulder] = SHOULDER_UP;
		wait1Msec(2000);
		servo[wrist] = WRIST_OPEN;
		wait1Msec(1000);
		GyroTime_moveV2(600,25,true,false,false);
		wait1Msec(3000);
		servo[shoulder] = SHOULDER_DOWN;

		switch(fieldRed)
		{
		case STOP:
			StopRobot();
			break;

		case RED:
			wait1Msec(3000);
			Gyro_TurnV2(42,-15,REL);
			GyroTime_moveV2(1200,50,true,false,REL);
			GyroTime45_V2(490,50,false, true, REL, true);
			GyroTime_moveV2(400,50,true,false,REL);
			break;

		case BLUE:
			wait1Msec(3000);
			Gyro_TurnV2(42,15,REL);
			GyroTime_moveV2(1600,45,true,false,false);
			break;
		}
		break;
		//================================================
		// CENTER PED SECOND POSITION
		//================================================
	case 5:

		GyroSonar_moveV2(0, BACK, 100, 120, -55,true, true, CONSTANT);
		GyroTimeS_moveV2(750,-40,true,true,false,true);
		Gyro_TurnV2(40,15,CONSTANT);

		wait1Msec(800);
		GyroTime_moveV2(1200,-30,true,false,false);
		motor[motorA] = -50;
		wait1Msec(1000);
		motor[motorA] = -3;
		GyroTimeS_moveV2(8000,-15,true,true,false,false);

		GyroTimeS_moveV2(8000,18,true,true,false,false);
		GyroTimeS_moveV2(8000,-15,true,true,false,false);

		Gyro_TurnV2(36,-15,CONSTANT);

		GyroTime_moveV2(360,-20,true,false,false);
		wait1Msec(1500);
		servo[shoulder] = SHOULDER_UP;
		wait1Msec(2000);
		servo[wrist] = WRIST_OPEN;
		wait1Msec(1000);
		GyroTime_moveV2(600,25,true,false,false);
		wait1Msec(1000);
		servo[shoulder] = SHOULDER_DOWN;

		switch(fieldRed)
		{
		case STOP:
			StopRobot();
			break;

		case RED:
			Gyro_TurnV2(42,-15,REL);
			GyroTime_moveV2(1200,50,true,false,REL);
			GyroTime45_V2(490,50,false, true, REL, true);
			GyroTime_moveV2(400,50,true,false,REL);
			break;

		case BLUE:
			Gyro_TurnV2(42,15,REL);
			GyroTime_moveV2(1600,45,true,false,false);
			break;
		}
		break;

		wait1Msec(1500);
		servo[shoulder] = SHOULDER_UP;
		wait1Msec(2000);
		servo[wrist] = WRIST_OPEN;
		wait1Msec(1000);
		GyroTime_moveV2(400,25,true,false,false);
		wait1Msec(3000);
		servo[shoulder] = SHOULDER_DOWN;

		switch(fieldRed)
		{
		case STOP:
			StopRobot();
			break;

		case RED:
			wait1Msec(3000);
			Gyro_TurnV2(42,-15,REL);
			GyroTime_moveV2(1200,50,true,false,REL);
			GyroTime45_V2(490,50,false, true, REL, true);
			GyroTime_moveV2(400,50,true,false,REL);
			break;

		case BLUE:
			wait1Msec(3000);
			Gyro_TurnV2(42,15,REL);
			GyroTime_moveV2(1600,45,true,false,false);
			break;
		}

		break;

		//================================================
		// RIGHT PEG SECOND POSITION
		//================================================
	case 6:

		GyroSonar_moveV2(0, BACK, 100, 120, -55,true, true, CONSTANT);
		GyroTimeS_moveV2(750,-40,true,true,false,true);
		Gyro_TurnV2(40,15,CONSTANT);

		wait1Msec(800);
		GyroTime_moveV2(1200,-30,true,false,false);
		motor[motorA] = -50;
		wait1Msec(1000);
		motor[motorA] = -3;
		GyroTimeS_moveV2(8000,-15,true,true,false,false);

		GyroTimeS_moveV2(8000,18,true,true,false,false);
		GyroTimeS_moveV2(8000,-15,true,true,false,false);

		Gyro_TurnV2(36,-15,CONSTANT);

		GyroTime_moveV2(1300,-30,true,false,false);
		GyroTime_moveV2(100,30,true,false,false);

		GyroTimeS_moveV2(360,-20,true,false,false,false);
		GyroTimeS_moveV2(8000,-18,true,true,false,false);
		GyroTimeS_moveV2(8000,18,true,true,false,false);
		GyroTimeS_moveV2(8000,-18,true,true,false,false);

		GyroTime_moveV2(240,-30,true,false,false);

		wait1Msec(1500);
		servo[shoulder] = SHOULDER_UP;
		wait1Msec(2000);
		servo[wrist] = WRIST_OPEN;
		wait1Msec(1000);
		GyroTime_moveV2(600,25,true,false,false);
		wait1Msec(3000);
		servo[shoulder] = SHOULDER_DOWN;

		switch(fieldRed)
		{
		case STOP:
			StopRobot();
			break;

		case RED:
			wait1Msec(3000);
			Gyro_TurnV2(42,-15,REL);
			GyroTime_moveV2(1200,50,true,false,REL);
			GyroTime45_V2(490,50,false, true, REL, true);
			GyroTime_moveV2(400,50,true,false,REL);
			break;

		case BLUE:
			wait1Msec(3000);
			Gyro_TurnV2(42,15,REL);
			GyroTime_moveV2(1600,45,true,false,false);
			break;
		}
		break;

		//================================================
		// LEFT PED SECOND POSITION
		//================================================

	case 7:

		GyroSonar_moveV2(0, BACK, 100, 120, -55,true, true, CONSTANT);
		GyroTimeS_moveV2(750,-40,true,true,false,true);
		Gyro_TurnV2(40,15,CONSTANT);

		wait1Msec(800);
		GyroTime_moveV2(1200,-30,true,false,false);
		motor[motorA] = -50;
		wait1Msec(1000);
		motor[motorA] = -3;
		GyroTimeS_moveV2(8000,-15,true,true,false,false);

		GyroTimeS_moveV2(8000,18,true,true,false,false);
		GyroTimeS_moveV2(8000,-15,true,true,false,false);

		Gyro_TurnV2(36,-15,CONSTANT);

		GyroTime_moveV2(1300,-30,true,false,false);
		GyroTime_moveV2(120,30,true,false,false);

		GyroTimeS_moveV2(260,20,true,false,false,false);
		GyroTimeS_moveV2(8000,15,true,true,false,false);
		GyroTimeS_moveV2(8000,-15,true,true,false,false);
		Gyro_TurnV2(36,-15,CONSTANT);
		GyroTimeS_moveV2(80,-18,true,false,false,false);
		GyroTime_moveV2(400,-30,true,false,false);
		wait1Msec(1500);
		servo[shoulder] = SHOULDER_UP;
		wait1Msec(2000);
		servo[wrist] = WRIST_OPEN;
		wait1Msec(1000);
		GyroTime_moveV2(600,25,true,false,false);
		wait1Msec(3000);
		servo[shoulder] = SHOULDER_DOWN;

		switch(fieldRed)
		{
		case STOP:
			StopRobot();
			break;

		case RED:
			wait1Msec(3000);
			Gyro_TurnV2(42,-15,REL);
			GyroTime_moveV2(1200,50,true,false,REL);
			GyroTime45_V2(490,50,false, true, REL, true);
			GyroTime_moveV2(400,50,true,false,REL);
			break;

		case BLUE:
			wait1Msec(3000);
			Gyro_TurnV2(42,15,REL);
			GyroTime_moveV2(1600,45,true,false,false);
			break;
		}
		break;


		//================================================
		// DEFENSE
		//================================================

	case 8:

		GyroTime_moveV2(1200,-50,true,false,REL);

		GyroSonar_moveV2(0, BACK, 100, 120, -55,true, true, CONSTANT);

		break;

		//================================================
		// OPPONENTS SIDE
		//================================================

	case 9:
		for (int i=0;i<360;i++)
		{SetDrive(i,50);
			wait1Msec(10);
		}
		StopRobot();
		wait1Msec(30000);
		break;

		//================================================
		// HIGH LIGHT
		//================================================

	case 10:
		for (int i=0;i<360;i++)
		{SetDrive(i,50);
			wait1Msec(10);
		}
		StopRobot();
		wait1Msec(30000);
		break;

		//================================================
		// HIGH LIGHT OPPONENTS
		//================================================

	case 11:
		for (int i=0;i<360;i++)
		{SetDrive(i,50);
			wait1Msec(10);
		}
		StopRobot();
		wait1Msec(30000);
		break;

		//================================================
		// ROBOT EVADE
		//================================================

	case 12:
		for (int i=0;i<360;i++)
		{SetDrive(i,50);
			wait1Msec(10);
		}
		StopRobot();
		wait1Msec(30000);
		break;
	}
}
