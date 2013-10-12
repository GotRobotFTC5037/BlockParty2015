#pragma config(Hubs,  S1, HTMotor,  HTServo,  none,     none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     HTGYRO,         sensorAnalogActive)
#pragma config(Motor,  motorC,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  mtr_S1_C1_1,     right_motor,   tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     left_motor,    tmotorTetrix, openLoop, encoder)
#pragma config(Servo,  srvo_S1_C2_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_6,    servo6,               tServoNone)
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
#include "lib/abs_point_turn.h"
#include "lib/math_utils.h"
#include "lib/abs_gyro_cal.h"
//#include "JoystickDriver.c"
#include "lib/abs_create_mem.h"
#include "lib/abs_initialize_system.h"
#include "lib/abs_drive.h"

//========================================
// Main program
//========================================

task main ()
{
	//waitforstart();
  //abs_initialize_system();
  //abs_point_turn(gyro_degrees(360),CLOCKWISE,50);
	//abs_point_turn(gyro_degrees(180),COUNTERCLOCKWISE,80);
	abs_drive(BACKWARDS, ENCODER, 3600, 50);
}
