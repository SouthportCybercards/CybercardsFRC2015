#include "WPILib.h"
#include <iostream>

class Robot: public IterativeRobot
{
	RobotDrive robotDrive; // robot drive system
	Joystick leftStick,rightStick;
	LiveWindow *lw;
	int autoLoopCounter;

public:
	//Class constructor
	// Initialize the robot drive to:
	// Drive base:
	// Left		Front	Right
	// +---------------------+
	// | PWM 0 			PWM 1|
	// | 		Robot		 |
	// | PWM 2 			PWM 3|
	// +---------------------+
	//			Back
	Robot() :
		robotDrive(0, 2, 1, 3),	// these must be initialized in the same order
		leftStick(0),
		rightStick(1),				// as they are declared above.
		lw(NULL),
		autoLoopCounter(0)
	{
		robotDrive.SetExpiration(0.1);
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
			robotDrive.Drive(-0.5, 0.0); 	// drive forwards half speed
			autoLoopCounter++;
		} else if(autoLoopCounter >= 100) {
			 //Check if we've completed 100 loops (approximately 2 seconds)
			robotDrive.Drive(0.5, 0.0); 	// drive backwards half speed
			autoLoopCounter++;
		} //else {
			//robotDrive.Drive(0.0, 0.0); 	// stop robot
		//}
	}

	void TeleopInit()
	{

	}

	void TeleopPeriodic()
	{
		//Handle robot control
		MainRobotControlLoop();

		//TODO - Handle logging, etc.
	}

	//Main control loop function
	void MainRobotControlLoop(void){

		//Handle the drive base control
		DriveBaseControl();

		//Handle the main lift
		//InnerLiftControl();

		//Handle the outer lift
		//OuterLiftControl();
	}

	//Drive base control
	void DriveBaseControl(void){

		//Local declarations
		float driveThreshold = 0.005;

		//Get the x-axis of the joystick
		float yAxis1 = leftStick.GetY();
		float yAxis2 = rightStick.GetY();

		//std::cout << "yAxisVal: " << yAxisVal << std::endl;

		//Convert input to [-1.0, 1.0] range
		//Don't need to - already in the correct range

		//Drive the drive motors when any input is within +-driveThreshold of 0.0
		//NOTE - currently this doesn't scale up the input from 0.0 after the deadband region -- it just uses the raw value.
		if(yAxis1 >= driveThreshold || yAxis2 >= driveThreshold || yAxis1 <= -driveThreshold || yAxis2 <= -driveThreshold )
		{
			robotDrive.TankDrive(leftStick,rightStick); 	// drive Forwards
		} else {
			robotDrive.TankDrive(0.0, 0.0); 	// stop robot
		}

	}

	//Inner lift control
	void InnerLiftControl(void){

		//This is where we write code to control the inner lift

	}

	//Outer lift control
	void OuterLiftControl(void){

		//This is where we write code to control the outer lift

	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
