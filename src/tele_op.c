#pragma config(Sensor, S1,     GYRO,           sensorAnalogInactive)
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
#include "drivers/hitechnic-gyro.h"

task main()
{
while(true)
{

nxtDisplayTextLine(2, "Offset: %f", HTGYROstartCal(HTGYRO));
}
}
