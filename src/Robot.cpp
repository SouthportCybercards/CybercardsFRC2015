#include "WPILib.h"
#include "Joystick.h"
#include <iostream>

class Robot: public IterativeRobot
{
	RobotDrive robotDrive; // robot drive system
	Joystick leftStick, rightStick, leftOpStick, rightOpStick;
	Talon LeftOuterLiftMotor;
	Talon RightOuterLiftMotor;
	Talon LeftElbowMotor;
	Talon RightElbowMotor;
	//Talon InnerLiftMotor1;
	//Talon InnerLiftMotor2;
	LiveWindow *lw;
	int autoLoopCounter;

	//Motor Direction output constants
	float LeftArmUpSpeed = 1.0;
	float RightArmUpSpeed = 1.0;
	float CenterLiftUpSpeed = 1.0;
	float LeftElbowInSpeed = 1.0;
	float RightElbowInSpeed = 1.0;

	//Joystick button input constants
	int InnerLiftUpDirection = 0;
	int InnerLiftDownDirection = 1;
	int LeftLiftUpButton = 6;
	int LeftLiftDownButton = 4;
	int RightLiftUpButton = 5;
	int RightLiftDownButton = 3;
	int LeftArmElbowInDirection = 1;
	int RightArmElbowInDirection = 1;

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
		//robotDrive(0, 2, 1, 3),	// these must be initialized in the same order
		robotDrive(9, 8, 7, 6),	// these must be initialized in the same order
		leftStick(0),
		rightStick(1),// as they are declared above.
		leftOpStick(2),
		rightOpStick(3),
		LeftOuterLiftMotor(0),
		RightOuterLiftMotor(1),
		LeftElbowMotor(2),
		RightElbowMotor(3),
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
		OuterLiftControl();
	}

	//Drive base control
	void DriveBaseControl(void){

		//Local declarations
		float driveThreshold = 0.005;

		//Get the y-axis of the joystick
		float yAxis1 = 0.75 * leftStick.GetY();
		float yAxis2 = 0.72 * rightStick.GetY();

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

	//Inner lift control
	void InnerLiftControl(void){

		//This is where we write code to control the inner lift

		//Read the lift control inputs (buttons on the xbox controller - which buttons?)
		//void XboxController;"A Button, B Button, X Button, Y Button";

		//bool xBoxInput1Button = xBoxController.GetButton();

		//Based on the input, control the motor

	}


	//Outer lift control
	void OuterLiftControl(void){

		//This is where we write code to control the outer lift
		int leftUpPressed = leftOpStick.GetRawButton(LeftLiftUpButton);
		int leftDownPressed = leftOpStick.GetRawButton(LeftLiftDownButton);
		int rightUpPressed = rightOpStick.GetRawButton(RightLiftUpButton);
		int rightDownPressed = rightOpStick.GetRawButton(RightLiftDownButton);
		float rightElbowValue = rightOpStick.GetZ();
		float leftElbowValue = leftOpStick.GetZ();

		//Local Declarations-- makes joystick input close to zero zero
		float elbowThreshold = 0.005;

		if (leftUpPressed == true){
			LeftOuterLiftMotor.Set(LeftArmUpSpeed);
		}
		else if (leftDownPressed == true){
			LeftOuterLiftMotor.Set(LeftArmUpSpeed * -1);
		}
		else{
			LeftOuterLiftMotor.Set(0.0);
		}

		if (rightUpPressed == true){
			RightOuterLiftMotor.Set(RightArmUpSpeed);
		}
		else if (rightDownPressed == true){
			RightOuterLiftMotor.Set(RightArmUpSpeed * -1);
		}
		else{
			RightOuterLiftMotor.Set(0.0);
		}

		if(rightElbowValue >= elbowThreshold || rightElbowValue <= -elbowThreshold ){
			RightElbowMotor.Set(rightElbowValue);
		}
		else{
			RightElbowMotor.Set(0.0);
		}

		if(leftElbowValue >= elbowThreshold || leftElbowValue <= -elbowThreshold){
			LeftElbowMotor.Set(leftElbowValue);
		}
		else{
			LeftElbowMotor.Set(0.0);
		}
		//...
	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
