#pragma config(Hubs,  S1, HTMotor,  none,     none,     none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     motorRight,    tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     motorLeft,     tmotorTetrix, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "joystickdriver.c"
task main()
{
	while(true)
	{
		if(abs(joystick.joy1_y1)>10)motor[motorRight] = joystick.joy1_y1;
		else motor[motorRight] = 0;
		if(abs(joystick.joy1_y2)>10)motor[motorLeft] = joystick.joy1_y2;
		else motor[motorLeft] = 0;
	}
}
