#include "WPILib.h"
#include <iostream>

class Robot: public IterativeRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stick,stick2;
	LiveWindow *lw;
	int autoLoopCounter;

public:
	Robot() :
		myRobot(0, 2, 1, 3),	// these must be initialized in the same order
		stick(0),
		stick2(1),				// as they are declared above.
		lw(NULL),
		autoLoopCounter(0)
	{
		myRobot.SetExpiration(0.1);
	}

private:
	void RobotInit()
	{
		lw = LiveWindow::GetInstance();
	}

	void AutonomousInit()
	{
		autoLoopCounter = 0;
	}

	void AutonomousPeriodic()
	{
		if(autoLoopCounter < 100) //Check if we've completed 100 loops (approximately 2 seconds)
		{
			myRobot.Drive(-0.5, 0.0); 	// drive forwards half speed
			autoLoopCounter++;
		} else if(autoLoopCounter >= 100) {
			 //Check if we've completed 100 loops (approximately 2 seconds)
			myRobot.Drive(0.5, 0.0); 	// drive backwards half speed
			autoLoopCounter++;
		} //else {
			//myRobot.Drive(0.0, 0.0); 	// stop robot
		//}
	}

	void TeleopInit()
	{

	}

	void TeleopPeriodic()
	{
		//Local declarations
		float driveThreshold = 0.005;

		//Get the x-axis of the joystick
		float yAxis1 = stick.GetY();
		float yAxis2 = stick2.GetY();

		//std::cout << "yAxisVal: " << yAxisVal << std::endl;

		//Convert input to [-1.0, 1.0] range
		//Don't need to - already in the correct range

		//Drive the drive motors when any input is within +-driveThreshold of 0.0
		//NOTE - currently this doesn't scale up the input from 0.0 after the deadband region -- it just uses the raw value.
		if(yAxis1 >= driveThreshold || yAxis2 >= driveThreshold || yAxis1 <= -driveThreshold || yAxis2 <= -driveThreshold )
		{
			myRobot.TankDrive(stick,stick2); 	// drive Forwards
		} else {
			myRobot.TankDrive(0.0, 0.0); 	// stop robot
		}

	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
