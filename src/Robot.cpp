#include "WPILib.h"
#include "Joystick.h"
#include <iostream>

class Robot: public IterativeRobot
{
	RobotDrive robotDrive; // robot drive system
	Joystick leftStick, rightStick;
	LiveWindow *lw;
	int autoLoopCounter;

public:
	//Class constructor
	// Initialize the robot drive to:
	// Drive base:
	// Left		Front	Right
	// +---------------------+
	// | PWM 9 			PWM 7|
	// | 		Robot		 |
	// | PWM 8 			PWM 6|
	// +---------------------+
	//			Back
	Robot() :
		robotDrive(9, 8, 7, 6),	// these must be initialized in the same order
		leftStick(0),
		rightStick(1),// as they are declared above.
		lw(NULL),
		autoLoopCounter(0)
	{
		robotDrive.SetExpiration(0.1);
		robotDrive.SetInvertedMotor(robotDrive.kFrontLeftMotor,false);
		robotDrive.SetInvertedMotor(robotDrive.kFrontRightMotor,false);
		robotDrive.SetInvertedMotor(robotDrive.kRearLeftMotor,false);
		robotDrive.SetInvertedMotor(robotDrive.kRearRightMotor,false);
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
	//	if(AutoIO.Get() == true){
		//	if(AutoSelect.Get() == false){
			//	if(autoLoopCounter < 50) //Check if we've completed 500 loops (approximately 10 seconds)---time 168 or 50
			//	{
				//	robotDrive.Drive(0.5, 0); 	// drive forwards half speed--(0.03 for 160 0 for 50
				//	autoLoopCounter++;
			//	} //else if((autoLoopCounter >= 100) && (autoLoopCounter < 200)) {
					 //Check if we've completed 100 loops (approximately 2 seconds)
					//robotDrive.Drive(0.5, 0.0); 	// drive backwards half speed
					//autoLoopCounter++;
				//}
			//	else {
				//	robotDrive.Drive(0.0, 0.0); 	// stop robot
			//	}
		//	}else{
			//	if(autoLoopCounter < 134) //Check if we've completed 168 loops
			//	{
			//		robotDrive.Drive(0.5, 0.0); 	// drive forwards half speed with no curve correction
				//	autoLoopCounter++;
			//	} //else if((autoLoopCounter >= 100) && (autoLoopCounter < 200)) {
					 //Check if we've completed 100 loops (approximately 2 seconds)
					//robotDrive.Drive(0.5, 0.0); 	// drive backwards half speed
					//autoLoopCounter++;
				//}
			//	else {
				//	robotDrive.Drive(0.0, 0.0); 	// stop robot
			//	}
		//	}
	//	}else{
			//intentionally blank
		//}
		//std::cout << "AutoIO: " << AutoIO.Get() << "AutoSelect: "<< AutoSelect.Get() << std::endl;
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
	}

	//Drive base control
	void DriveBaseControl(void){

		//Local declarations
		float driveThreshold = 0.005;

		//Get the y-axis of the joystick

		float yAxis1 = 1 * leftStick.GetY();
		float yAxis2 = 1 * rightStick.GetY();

		//std::cout << "yAxisVal: " << yAxisVal << std::endl;

		//Convert input to [-1.0, 1.0] range
		//Don't need to - already in the correct range

		//Drive the drive motors when any input is within +-driveThreshold of 0.0
		//NOTE - currently this doesn't scale up the input from 0.0 after the deadband region -- it just uses the raw value.
		if(yAxis1 >= driveThreshold || yAxis2 >= driveThreshold || yAxis1 <= -driveThreshold || yAxis2 <= -driveThreshold )
		{
			robotDrive.TankDrive(-yAxis1,-yAxis2); 	// drive Forwards
		} else {
			robotDrive.TankDrive(0.0, 0.0); 	// stop robot
		}

	}

	
}

	void TestPeriodic()
	{
		lw->Run();
	}
};


START_ROBOT_CLASS(Robot);

