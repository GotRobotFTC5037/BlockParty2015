#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTServo)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     SENSOR_MUX,     sensorI2CCustom)
#pragma config(Sensor, S3,     HTGYRO,         sensorI2CCustom)
#pragma config(Sensor, S4,     angle_sensor,   sensorI2CCustom)
#pragma config(Motor,  mtr_S1_C1_1,     jolly_roger,   tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     block_lift_motor2, tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S1_C2_1,     block_lift_motor, tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     sky_hook,      tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     right_motor,   tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C3_2,     left_motor,    tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C4_1,    grabber_right,        tServoStandard)
#pragma config(Servo,  srvo_S1_C4_2,    grabber_left,         tServoStandard)
#pragma config(Servo,  srvo_S1_C4_3,    roger_slide,          tServoContinuousRotation)
#pragma config(Servo,  srvo_S1_C4_4,    EOPD_servo,           tServoStandard)
#pragma config(Servo,  srvo_S1_C4_5,    ground_arm,           tServoStandard)
#pragma config(Servo,  srvo_S1_C4_6,    abdd,                 tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/**
*
*  @file auto.c
*
*  @brief The automatic program for the robot.
*
*  @copyright Copyright 2013, Got Robot? FTC Team 5037
*
*/

/*Includes*/

//-----------------------
// sensor/mux/joystick includes
//-----------------------

#include "lib/compile_flags.h"

#include "joystickdriver.c"
#include "lib/xander/hitechnic-sensormux.h"
#include "lib/xander/lego-light.h"
#include "lib/xander/hitechnic-irseeker-v2.h"

#if MOCK_GYRO == 1
  #include "rcunit/mock/event_manager.h"
  #include "rcunit/mock/rc_mock_gyro.h"
#else
  #include "lib/xander/hitechnic-gyro.h"
#endif

#include "lib/xander/hitechnic-angle.h"
#include "lib/xander/hitechnic-accelerometer.h"
#include "lib/xander/hitechnic-eopd.h"

//-----------------------
// Custom include
//-----------------------

#include "lib/global_variables.h"
#include "lib/abs_initialize.h"
#include "lib/abs_cscreen.h"
#include "lib/abs_s1_mission_execute.h"
#include "lib/abs_s2_mission_execute.h"
#include "lib/abs_s3_mission_execute.h"
#include "lib/abs_s4_mission_execute.h"
#include "lib/abs_s5_mission_execute.h"
#include "lib/abs_dlog.h"
#include "lib/abs_stay_on_ramp.h"
#include "lib/abs_end_ramp.h"
#include "lib/abs_reset_heading.h"


//========================================
// Main program
//========================================
task main()
{
	Delete(LogFileName, LogIoResult);																	//open the datalog file so it is excisable
	OpenWrite(LogFileHandle, LogIoResult, LogFileName, LogFileSize);	//throughout the program

	abs_dlog(__FILE__ ,"Program start"," Start time:", nPgmTime);			//write the first entry starting the datalog

	abs_initialize();
	abs_reset_heading(RELATIVE_CONSTANT);

	switch(g_start_point)
	{
	case 1:
		abs_s1_mission_execute();		//missions for starting position 1
		break;
	case 2:
		abs_s2_mission_execute();		//missions for starting position 2
		break;
	case 3:
		abs_s3_mission_execute();		//missions for starting position 3
		break;
	case 4:
		abs_s4_mission_execute();		//missions for starting position 4
		break;
	case 5:
		abs_s5_mission_execute();		//missions for starting position 5
		break;
	}

	/** TODO:  see if this can be deleted */
	abs_reset_heading(CONSTANT);					//reset the constant value for the end mission section of the program

	switch(g_end_point)
	{
	case 1:												//case 1: compleat start
		wait1Msec(2000);
		servo[abdd] = g_abdd_down;
		abs_stop_robot();
		break;
	case 2:												//case 2&3: end point is on the ramp
	case 3:
		abs_end_ramp(2000);
		break;
	case 4:
	case 5:
	case 6:
	break;

	default:											//error case: if this is executed something went wrong with the auto selection
		abs_dlog(__FILE__,"Invalid Ramp Option");
		break;
	}

	abs_dlog(__FILE__ ,"end auto", "End time:", nPgmTime);
	Close(LogFileHandle, LogIoResult);			//close the datalogging
	LogData=false;

	if(g_stay_on_ramp) abs_hold_position();	//if config selected robot resists being pushed
}
