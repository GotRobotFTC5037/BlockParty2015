/**
*
*  @file abs_s2_mission_execute.h
*
*  @brief runs the missions from the starting point S2
*
*  @param None n/a
*
*  @return returns nothing
*
*  @copyright Copyright 2013, Got Robot? FTC Team 5037
*
*/

#ifndef ABS_S2_MISSION_EXECUTE_H
#define ABS_S2_MISSION_EXECUTE_H

#include "abs_drive.h"
#include "abs_turn.h"
#include "abs_stop_robot.h"
#include "abs_end_ramp.h"
#include "abs_log.h"
#include "abs_get_angle_sensor_val.h"

void abs_s2_mission_execute()
{
	switch(g_mission_number)
	{
	case 0:
		g_screen_state = S_ANGLE_SHOW;
		abs_drive(FORWARD, E_ANGLE, /*distance in cm*/600, 50, true, GYRO);
		break;

	case 1:
		dist_record=true;
		abs_drive(BACKWARD, E_IR_DETECT, 3, 40, true, GYRO);
		g_shift_due_to_ir = true;
		if(abs_get_angle_sensor_val(RELATIVE_BPU) < 38)
		{
			dist_record = true;
			//g_shift_due_to_ir = false;
			abs_drive(BACKWARD, E_ANGLE, 40 - abs_get_angle_sensor_val(RELATIVE_BPU), 40, true, GYRO);
		}
		//Only use g_shift_due_to_ir when shifted due the the ir detecting
		PlayTone(200,20);
		wait1Msec(1000);
		if(g_shift_due_to_ir)
		{
			abs_drive(FORWARD, E_ANGLE, /*distance in cm*/6, 50, true, GYRO);
		}
		wait1Msec(500);
		break;

	case 2:
		dist_record=true;
		abs_drive(BACKWARD, E_ANGLE, /*distance in cm*/40, 50, true, GYRO);
		break;

	case 3:
		dist_record=true;
		abs_drive(BACKWARD, E_ANGLE, /*distance in cm*/65, 50, true, GYRO);
		break;

	case 4:
		dist_record=true;
		abs_drive(BACKWARD, E_ANGLE, /*distance in cm*/115, 50, true, GYRO);
		break;

	case 5:
		dist_record=true;
		abs_drive(BACKWARD, E_ANGLE, /*distance in cm*/140, 50, true, GYRO);
		wait1Msec(2000);
		break;

	case 6:
		abs_turn(CLOCKWISE, POINT, TURN, 75, 60);
		wait1Msec(200);
		abs_drive(FORWARD, E_ANGLE, 190, 50, true, GYRO);
		abs_turn(COUNTERCLOCKWISE, POINT, TURN, 75, 60);
		abs_drive(FORWARD, E_ANGLE, 80, 50, true, GYRO);
		break;

	case 7:
		abs_turn(COUNTERCLOCKWISE, POINT, TURN, 98, 60);
		wait1Msec(200);
		abs_drive(FORWARD, E_ANGLE, 87, 50, true, GYRO);
		motor[block_lift_motor] = 40;
		motor[block_lift_motor2] = 40;
		abs_turn(CLOCKWISE, POINT, TURN, 103, 60);
		motor[block_lift_motor] = 0;
		motor[block_lift_motor2] = 0;
		abs_drive(FORWARD, E_ANGLE, 80, 50, true, GYRO);
		break;
	}
	abs_log(__FILE__,"abdd up",2,g_abdd_up,0,0);
	servo[abdd] = g_abdd_up;
	StartTask (abs_calibrate_light);
	wait1Msec(2000);
	servo[abdd] = g_abdd_down;
	abs_log(__FILE__,"abdd down",2,g_abdd_down,0,0);

	wait1Msec(g_end_delay * DELAY_MULTIPLICATION_FACTOR);
	wait1Msec(100);
}

#endif /* !ABS_S2_MISSION_EXECUTE_H */
