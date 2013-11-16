/**
*
*  @file abs_joystick_drive.h
*
*  @brief The header file that handles the joystick motor control
*
*  @copyright Copyright 2013, Got Robot? FTC Team 5037
*
*/
#ifndef ABS_JOYSTICK_GUNNER_H
#define ABS_JOYSTICK_GUNNER_H

/** macros */
//=======================================
// joystick drive
//=======================================
task abs_joystick_gunner()
{
	while(true)
	{
		if(abs(joystick.joy2_y1) > 10) 	motor[block_grabber_left] = joystick.joy2_y1*100/127;
		else motor[block_grabber_left] = 0;

		if(abs(joystick.joy2_y2) > 10) motor[block_grabber_right] = joystick.joy2_y2*100/127;
		else motor[block_grabber_right] = 0;

		//if(joy2Btn(4))
		//{
		//	motor(flag_motor)=75;
		//}

		//else if(joy2Btn(2))
		//{
		//	motor(flag_motor)=-75;
		//}

		//else motor(flag_motor)=0;

		//if(joystick.joy1_TopHat == 0)
		//	motor(lifter_motor)=LIFT_SPEED_UP;
		//if(joystick.joy1_TopHat == 4)
		//	motor(lifter_motor)=LIFT_SPEED_DOWN;

		int grabber_position;

		//if(joy2Btn(8) && grabber_position < 3)
		//{
		//	grabber_position++;
		//	while(joy2Btn(8));
		//}
		//if(joy2Btn(7) && grabber_position > 1)
		//{
		//	grabber_position--;
		//	while(joy2Btn(7));
		//}
		if(joy2Btn(1) || joy1Btn(1)) grabber_position = GRABBER_OPEN;
		if(joy2Btn(2) || joy1Btn(2)) grabber_position = GRABBER_MID;
		if(joy2Btn(3) || joy1Btn(3)) grabber_position = GRABBER_CLOSE;

		if(joy2Btn(5)) grabber_position = GRABBER_POS_LEFT_OPEN;
		else if(joy2Btn(7)) grabber_position = GRABBER_POS_LEFT_CLOSE;
		if(joy2Btn(6)) grabber_position = GRABBER_POS_RIGHT_OPEN;
		else if(joy2Btn(8)) grabber_position = GRABBER_POS_RIGHT_CLOSE;

		switch(grabber_position)
		{
		case GRABBER_OPEN:
			servo[grabber_left] = GRABBER_LEFT_OPEN;
			servo[grabber_right] = GRABBER_RIGHT_OPEN;
			break;
		case GRABBER_MID:
			servo[grabber_left] = GRABBER_LEFT_MID;
			servo[grabber_right] = GRABBER_RIGHT_MID;
			break;
		case GRABBER_CLOSE:
			servo[grabber_left] = GRABBER_LEFT_CLOSE;
			servo[grabber_right] = GRABBER_RIGHT_CLOSE;
			break;
		case GRABBER_POS_LEFT_CLOSE:
			servo[grabber_left] = GRABBER_LEFT_CLOSE;
			break;
		case GRABBER_POS_LEFT_OPEN:
			servo[grabber_left] = GRABBER_LEFT_OPEN;
			break;
		case GRABBER_POS_RIGHT_CLOSE:
			servo[grabber_right] = GRABBER_RIGHT_CLOSE;
			break;
		case GRABBER_POS_RIGHT_OPEN:
			servo[grabber_right] = GRABBER_RIGHT_OPEN;
			break;
		}
	}
}

#endif /* !ABS_JOYSTICK_DRIVE_H */
