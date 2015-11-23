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
	Talon LeftInnerLiftMotor;
	Talon RightInnerLiftMotor;
	DigitalInput AutoIO;
	DigitalInput AutoSelect;
	LiveWindow *lw;
	int autoLoopCounter;

	//Motor Direction output constants
	float LeftArmUpSpeed = 0.3;
	float RightArmUpSpeed = 0.3;
	float LeftElbowInSpeed = 1.0;
	float RightElbowInSpeed = 1.0;
	float InnerLiftSpeed = 0.4;
	float LeftInnerLiftButtonUpDirection = 1.0;
	float RightInnerLiftButtonUpDirection = -1.0;
	float LeftInnerLiftYAxisUpDirection = -1.0;
	float RightInnerLiftYAxisUpDirection = 1.0;
	float LeftArmElbowInDirection = 1.0;
	float RightArmElbowInDirection = 1.0;

	//Joystick button input constants
	int LeftInnerManualUpButton = 5;
	int LeftInnerManualDownButton = 3;
	int RightInnerManualUpButton = 6;
	int RightInnerManualDownButton = 4;

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
		leftOpStick(2),
		rightOpStick(3),
		LeftOuterLiftMotor(0),
		RightOuterLiftMotor(1),
		LeftElbowMotor(2),
		RightElbowMotor(3),
		LeftInnerLiftMotor(4),
		RightInnerLiftMotor(5),
		AutoIO(9),
		AutoSelect(8),
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
		if(AutoIO.Get() == true){
			if(AutoSelect.Get() == false){
				if(autoLoopCounter < 50) //Check if we've completed 500 loops (approximately 10 seconds)---time 168 or 50
				{
					robotDrive.Drive(0.5, 0); 	// drive forwards half speed--(0.03 for 160 0 for 50
					autoLoopCounter++;
				} //else if((autoLoopCounter >= 100) && (autoLoopCounter < 200)) {
					 //Check if we've completed 100 loops (approximately 2 seconds)
					//robotDrive.Drive(0.5, 0.0); 	// drive backwards half speed
					//autoLoopCounter++;
				//}
				else {
					robotDrive.Drive(0.0, 0.0); 	// stop robot
				}
			}else{
				if(autoLoopCounter < 134) //Check if we've completed 168 loops
				{
					robotDrive.Drive(0.5, 0.0); 	// drive forwards half speed with no curve correction
					autoLoopCounter++;
				} //else if((autoLoopCounter >= 100) && (autoLoopCounter < 200)) {
					 //Check if we've completed 100 loops (approximately 2 seconds)
					//robotDrive.Drive(0.5, 0.0); 	// drive backwards half speed
					//autoLoopCounter++;
				//}
				else {
					robotDrive.Drive(0.0, 0.0); 	// stop robot
				}
			}
		}else{
			//intentionally blank
		}
		std::cout << "AutoIO: " << AutoIO.Get() << "AutoSelect: "<< AutoSelect.Get() << std::endl;
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
		InnerLiftControl();

		//Handle the outer lift
		OuterLiftControl();
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

	//Inner lift control
	void InnerLiftControl(void){
		int leftManualUp = leftOpStick.GetRawButton(LeftInnerManualUpButton);
		int leftManualDown = leftOpStick.GetRawButton(LeftInnerManualDownButton);
		int rightManualUp = rightOpStick.GetRawButton(RightInnerManualUpButton);
		int rightManualDown = rightOpStick.GetRawButton(RightInnerManualDownButton);

		//Outer if else is for y axis on stick control
		float liftThreshold = 0.1;

		//Get the y-axis of the joystick
		float yAxisA = leftOpStick.GetY();
		//float yAxisB = 0.72 * rightOpStick.GetY();
		if (yAxisA >= liftThreshold || yAxisA <= -liftThreshold ){
			LeftInnerLiftMotor.Set(LeftInnerLiftYAxisUpDirection * yAxisA);
			RightInnerLiftMotor.Set(RightInnerLiftYAxisUpDirection * yAxisA);
		} else {
			//this inner if else structure describes the button driven control
			if (leftManualUp == true || rightManualUp == true){
				LeftInnerLiftMotor.Set(InnerLiftSpeed * LeftInnerLiftButtonUpDirection);
				RightInnerLiftMotor.Set(InnerLiftSpeed * RightInnerLiftButtonUpDirection);
			}
			else if (leftManualDown == true || rightManualDown == true){
				LeftInnerLiftMotor.Set(InnerLiftSpeed * LeftInnerLiftButtonUpDirection * -1);
				RightInnerLiftMotor.Set(InnerLiftSpeed * RightInnerLiftButtonUpDirection * -1);
			}
			else{
				LeftInnerLiftMotor.Set(0.0);
				RightInnerLiftMotor.Set(0.0);
			}
		}
	}
	//Outer lift control
	void OuterLiftControl(void){
		//local declarations- get pov input
		//int leftPOV = leftOpStick.GetPOV();
		//int rightPOV = rightOpStick.GetPOV();
		////float rightElbowValue = rightOpStick.GetZ();
		///float leftElbowValue = leftOpStick.GetZ();
		////float rightVertical = rightOpStick.GetY();
		////float leftElbowValue = leftOpStick.GetZ();

		//Local Declarations-- makes joystick input close to zero zero
		////float elbowThreshold = 0.55;

		//left stick motor control
		//if (leftPOV == 315 || leftPOV == 0 || leftPOV == 45){
			//LeftOuterLiftMotor.Set(LeftArmUpSpeed);
		//}
		//else if (leftPOV == 135 || leftPOV == 180 || leftPOV == 225){
			//LeftOuterLiftMotor.Set(LeftArmUpSpeed * -1);
		//}
		//else{
			//LeftOuterLiftMotor.Set(0.0);
		//}

		//Right stick motor control
		//if (rightPOV == 315 || rightPOV == 0 || rightPOV == 45){
//		if (rightVertical >= elbowThreshold ){
//			RightOuterLiftMotor.Set(RightArmUpSpeed);
//		}
//		else if (rightVertical <= -elbowThreshold){
//			RightOuterLiftMotor.Set(-RightArmUpSpeed);
//		}
//		//else if (rightPOV == 135 || rightPOV == 180 || rightPOV == 225){
//			//RightOuterLiftMotor.Set(RightArmUpSpeed * -1);
//		//}
//		else{
			////RightOuterLiftMotor.Set(0.0);
		//}

		//Elbow motor Controls
//		if(rightElbowValue >= elbowThreshold || rightElbowValue <= -elbowThreshold ){
//			RightElbowMotor.Set(rightElbowValue * RightArmElbowInDirection * RightElbowInSpeed);
//		}
//		else{
//			RightElbowMotor.Set(0.0);
//		}
//
//		if(leftElbowValue >= elbowThreshold || leftElbowValue <= -elbowThreshold ){ //tote stopper
//			LeftElbowMotor.Set(leftElbowValue * LeftArmElbowInDirection * LeftElbowInSpeed);
//		}
//		else{
//			LeftElbowMotor.Set(0.0);
//		}
//
}

	void TestPeriodic()
	{
		lw->Run();
	}
};


START_ROBOT_CLASS(Robot);