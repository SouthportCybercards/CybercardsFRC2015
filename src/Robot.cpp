#include "WPILib.h"
#include <iostream>

class Robot: public IterativeRobot
{

	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick
	LiveWindow *lw;
	int autoLoopCounter;

public:
	Robot() :
		myRobot(0, 1),	// these must be initialized in the same order
		stick(0),		// as they are declared above.
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
		//myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)

		//Get the x-axis of the joystick
		float yAxisVal = stick.GetY();
		std::cout << "yAxisVal: " << yAxisVal << std::endl;

		//Convert input to [-1.0, 1.0] range
		//Don't need to - already in the correct range

		//Control the motor with PWM0 value
		myRobot.Drive(yAxisVal, 0.0);
	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
