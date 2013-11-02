#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTServo)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     GYRO_MUX,       sensorI2CCustom)
#pragma config(Sensor, S3,     SENSOR_MUX,     sensorI2CCustom)
#pragma config(Sensor, S4,     ANGLE_SENSOR,   sensorI2CCustom)
#pragma config(Motor,  motorB,          block_grabber_left, tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorC,          block_grabber_right, tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C1_1,     left_motor,    tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     right_motor,   tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S1_C2_1,     motorF,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     motorG,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     motorH,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     motorI,        tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C4_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/**
*d
*  @file tele_op.c
*
*  @brief The tele_op program for the robot
*-----------------
*  @copyright Copyright 2013, Got Robot? FTC Team 5037
*
*/

/*Includes*/

#include "JoystickDriver.c"

#include "lib/abs_teleop_utils.h"
#include "lib/abs_joystick_drive.h"
#include "lib/abs_joystick_gunner.h"

//========================================
// Main program
//========================================

task main ()
{
	while(true)
	{
		abs_joystick_drive(LINEAR);
		abs_joystick_gunner();
	}
}
