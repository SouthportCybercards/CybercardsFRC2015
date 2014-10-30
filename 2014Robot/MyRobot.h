//Header file for MyRobot.cpp

#ifndef __MYROBOTH__
#define __MYROBOTH__

//initialization values
#define kPotInitValue 0

//robot states types
typedef enum
{
	kManualMode,
	kBlock,
	kFloorLoad,
	kCatch,
	kCarry,
	kPrepForHighGoal,
	kPrepForLowGoal,
	kPrepForHighPass,
	kEjectBall
} RobotStates_t; //maybe add kPrepForLowPass

#endif
