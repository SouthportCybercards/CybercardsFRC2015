#include "RobotState.h"

//Class to describe a possible robot state
class RobotState
{
	
public:
	string stateName;
	uint16_t armAngle;
	uint16_t outerWheelSpeed;
	uint16_t innerWheelSpeed;
	bool ballCaught;

	//Constructor - just populate the state name
	RobotState(string inStateName):
		stateName(inStateName),
		armAngle(0),
		outerWheelSpeed(0),
		innerWheelSpeed(0),
		ballCaught(true)
	{
		//Intentionally Blank
	}
	
	//Constructor - populate all the class elements
	RobotState(string inStateName, uint16_t inArmAngle, uint16_t inOutWheelSpd, uint16_t inInWheelSpd, bool inBallCaught):
		stateName(inStateName),
		armAngle(inArmAngle),
		outerWheelSpeed(inOutWheelSpd),
		innerWheelSpeed(inInWheelSpd),
		ballCaught(inBallCaught)
	{
		//Intentionally Blank
	}
};
