#pragma config(Hubs,  S1, HTMotor,  none,     none,     none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     HTGYRO,         sensorAnalogActive)
#pragma config(Motor,  motorC,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  mtr_S1_C1_1,     right_motor,   tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     left_motor,    tmotorTetrix, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/**
 *
 *  Filename:  template.c (this should be the name of the function)
 *
 *  Description:  Enter a description of what this function does
 *
 *  Copyright 2013, Got Robot! FTC Team 5037
 *
 */

float drift;

#include "lib/abs_point_turn_to.h"
#include "lib/math_utils.h"
#include "lib/abs_gyro_cal.h"

//========================================
// Main program
//========================================

task main ()
{
	drift = abs_gyro_cal(8000);
  abs_point_turn_to(gyro_degrees(360),CLOCKWISE,50);
  abs_point_turn_to(gyro_degrees(180),COUNTERCLOCKWISE,80);
}
