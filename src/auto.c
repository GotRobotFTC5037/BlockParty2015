#pragma config(Hubs,  S1, HTMotor,  none,     none,     none)
#pragma config(Sensor, S2,     HTGYRO,         sensorAnalogActive)
#pragma config(Motor,  motorC,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  mtr_S1_C1_1,     right_motor,   tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     left_motor,    tmotorTetrix, openLoop, reversed)
#pragma systemFile
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

#include "lib/abs_global.h"
#include "lib/abs_point_turn_to.h"
#include "lib/math_utils.h"
#include "lib/abs_gyro_cal.h"
#include "JoystickDriver.c"
#include "lib/abs_create_mem.h"
#include "lib/abs_initialise.h"

//========================================
// Main program
//========================================

task main ()
{
	//waitforstart();
	abs_initialise(CALIBRATION_TIME);
	abs_point_turn_to(gyro_degrees(360),CLOCKWISE,50);
	abs_point_turn_to(gyro_degrees(180),COUNTERCLOCKWISE,80);
}
