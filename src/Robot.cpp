#include "WPILib.h"
#include "Joystick.h"
#include <iostream>

class Robot: public IterativeRobot
{
	RobotDrive RobotDrive; // robot drive system
	Joystick LeftStick, RightStick, LeftOpStick, RightOpStick;
	Talon LeftOuterLiftMotor;
	Talon RightOuterLiftMotor;
	Talon LeftElbowMotor;
	Talon RightElbowMotor;
	Talon LeftInnerLiftMotor;
	Talon RightInnerLiftMotor;
	DigitalInput InnerLiftZeroSensor;
	Encoder InnerLiftMotorEncoder;
	LiveWindow *lw;
	int autoLoopCounter;

	//Motor Direction output constants
	const float leftArmSpeed = 1.0;
	const float rightArmSpeed = 1.0;
	const float leftElbowInSpeed = 1.0;
	const float rightElbowInSpeed = 1.0;
	const float innerLiftSpeed = 1.0;
	const float leftInnerLiftUpDirection = -1.0;
	const float rightInnerLiftUpDirection = 1.0;
	const float leftArmElbowInDirection = 1.0;
	const float rightArmElbowInDirection = 1.0;

	//Joystick button input constants
	const int leftInnerManualUpButton = 5;
	const int leftInnerManualDownButton = 3;
	const int rightInnerManualUpButton = 6;
	const int rightInnerManualDownButton = 4;

	const int leftInnerAutoUpButton = 6;
	const int leftInnerAutoDownButton = 4;
	const int rightInnerAutoUpButton = 5;
	const int rightInnerAutoDownButton = 3;

	//Inner Lift constants
	const int innerLiftEncoderValue = 10;
	const int maxLevel = 6;
	const int minLevel = 0;

	//Class members
	bool innerLiftStateControl;

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
		RobotDrive(9, 8, 7, 6),	// these must be initialized in the same order
		LeftStick(0),
		RightStick(1),// as they are declared above.
		LeftOpStick(2),
		RightOpStick(3),
		LeftOuterLiftMotor(0),
		RightOuterLiftMotor(1),
		LeftElbowMotor(2),
		RightElbowMotor(3),
		LeftInnerLiftMotor(4),
		RightInnerLiftMotor(5),
		InnerLiftZeroSensor(0),
		InnerLiftMotorEncoder(5, 6, false, InnerLiftMotorEncoder.k4X),
		lw(NULL),
		autoLoopCounter(0)
	{
		RobotDrive.SetExpiration(0.1);
		RobotDrive.SetInvertedMotor(RobotDrive.kFrontLeftMotor,false);
		RobotDrive.SetInvertedMotor(RobotDrive.kFrontRightMotor,false);
		RobotDrive.SetInvertedMotor(RobotDrive.kRearLeftMotor,false);
		RobotDrive.SetInvertedMotor(RobotDrive.kRearRightMotor,false);

		innerLiftStateControl = false;
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
			RobotDrive.Drive(-0.5, 0.0); 	// drive forwards half speed
			autoLoopCounter++;
		} else if(autoLoopCounter >= 100) {
			 //Check if we've completed 100 loops (approximately 2 seconds)
			RobotDrive.Drive(0.5, 0.0); 	// drive backwards half speed
			autoLoopCounter++;
		} //else {
			//RobotDrive.Drive(0.0, 0.0); 	// stop robot
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

		//Get all the user inputs from the joysticks
		//GetAllUserInput();

		//Based on the
		//DetermineRobotState();

		//Handle the drive base control
		DriveBaseControl();

		if(innerLiftStateControl == true){
			//Handle the main lift
			InnerLiftAutoControl();
		} else {
			InnerLiftManualControl();
		}

		//Handle the outer lift
		OuterLiftControl();
	}

	//Drive base control
	void DriveBaseControl(void){

		//Local declarations
		float driveThreshold = 0.005;

		//Get the y-axis of the joystick
		float yAxis1 = 0.75 * LeftStick.GetY();
		float yAxis2 = 0.72 * RightStick.GetY();

		//std::cout << "yAxisVal: " << yAxisVal << std::endl;

		//Convert input to [-1.0, 1.0] range
		//Don't need to - already in the correct range

		//Drive the drive motors when any input is within +-driveThreshold of 0.0
		//NOTE - currently this doesn't scale up the input from 0.0 after the deadband region -- it just uses the raw value.
		if(yAxis1 >= driveThreshold || yAxis2 >= driveThreshold || yAxis1 <= -driveThreshold || yAxis2 <= -driveThreshold )
		{
			RobotDrive.TankDrive(-yAxis1,-yAxis2); 	// drive Forwards
		} else {
			RobotDrive.TankDrive(0.0, 0.0); 	// stop robot
		}

	}

	//Inner lift control
	void InnerLiftManualControl(void){

		int leftManualUp = LeftOpStick.GetRawButton(leftInnerManualUpButton);
		int leftManualDown = LeftOpStick.GetRawButton(leftInnerManualDownButton);
		int rightManualUp = RightOpStick.GetRawButton(rightInnerManualUpButton);
		int rightManualDown = RightOpStick.GetRawButton(rightInnerManualDownButton);

		if (leftManualUp == true || rightManualUp == true){
			LeftInnerLiftMotor.Set(innerLiftSpeed * leftInnerLiftUpDirection);
			RightInnerLiftMotor.Set(innerLiftSpeed * rightInnerLiftUpDirection);
		}
		else if (leftManualDown == true || rightManualDown == true){
			LeftInnerLiftMotor.Set(innerLiftSpeed * leftInnerLiftUpDirection * -1);
			RightInnerLiftMotor.Set(innerLiftSpeed * rightInnerLiftUpDirection * -1);
		}
		else{
			LeftInnerLiftMotor.Set(0.0);
			RightInnerLiftMotor.Set(0.0);
		}

		//Testing limit switch
		//bool innerLiftSwitchVal = InnerLiftZeroSensor.Get();
		//std::cout << "innerLiftSwitchVal: " << innerLiftSwitchVal << std::endl;
	}

	void InnerLiftAutoControl(void){
		int targetLevel;
		int currentInnerLiftEncoderValue = InnerLiftMotorEncoder.Get();
		int targetInnerLiftEncoderValue;
		int leftAutoUp = LeftOpStick.GetRawButton(leftInnerAutoUpButton);
		int leftAutoDown = LeftOpStick.GetRawButton(leftInnerAutoDownButton);
		int rightAutoUp = RightOpStick.GetRawButton(rightInnerAutoUpButton);
		int rightAutoDown = RightOpStick.GetRawButton(rightInnerAutoDownButton);
		int InnerLiftEncoderZeroValue;

		if (leftAutoUp == true || rightAutoUp == true){
			if (targetLevel < maxLevel){
				targetLevel++;
			}
		}
		else if(leftAutoDown == true || rightAutoDown == true){
			if(targetLevel > minLevel){
				targetLevel--;
			}
		}
		else{}

		targetInnerLiftEncoderValue = (targetLevel * innerLiftEncoderValue);

		if ((currentInnerLiftEncoderValue < targetInnerLiftEncoderValue) && !InnerLiftEncoderZeroValue){
			LeftInnerLiftMotor.Set(innerLiftSpeed * leftInnerLiftUpDirection);
			RightInnerLiftMotor.Set(innerLiftSpeed * rightInnerLiftUpDirection);
		}
		else if ((currentInnerLiftEncoderValue > targetInnerLiftEncoderValue) && !InnerLiftEncoderZeroValue){
			LeftInnerLiftMotor.Set(innerLiftSpeed * leftInnerLiftUpDirection * -1);
			RightInnerLiftMotor.Set(innerLiftSpeed * rightInnerLiftUpDirection * -1);
		}
		else if (){

		}

		/*
		bool InnerLiftUp;
		bool InnerLiftDown;

		int TargetUpInnerLiftEncoderValue;
		int TargetDownInnerLiftEncoderValue;
		int CurrentInnerLiftEncoderValue = InnerLiftMotorEncoder.Get();
		int leftAutoUp = leftOpStick.GetRawButton(LeftInnerAutoUpButton);
		int leftAutoDown = leftOpStick.GetRawButton(LeftInnerAutoDownButton);
		int rightAutoUp = rightOpStick.GetRawButton(RightInnerAutoUpButton);
		int rightAutoDown = rightOpStick.GetRawButton(RightInnerAutoDownButton);

		TargetUpInnerLiftEncoderValue = (InnerLiftEncoderValue + InnerLiftEncoderZeroValue);
		TargetUpInnerLiftEncoderValue = (InnerLiftEncoderValue - InnerLiftEncoderZeroValue);

		if ((leftAutoUp == true || rightAutoUp == true) && !InnerLiftZeroSensor){
			if (CurrentInnerLiftEncoderValue < TargetUpInnerLiftEncoderValue){
				InnerLiftUp = true;
				InnerLiftDown = false;
			}
			else {
				InnerLiftUp = false;
				InnerLiftDown = false;
			}
		}
		else if ((leftAutoDown == true || rightAutoDown == true) && !InnerLiftZeroSensor){
			if(CurrentInnerLiftEncoderValue > 0){
				InnerLiftUp = false;
				InnerLiftDown = true;
			}
			else {
				InnerLiftUp = false;
				InnerLiftDown = false;
			}
		}
		else if (InnerLiftZeroSensor){
			InnerLiftUp = false;
			InnerLiftDown = false;
			InnerLiftEncoderZeroValue = InnerLiftMotorEncoder.Get();
		}
		else{}

		if (InnerLiftUp == true && InnerLiftDown == false){
			LeftInnerLiftMotor.Set(InnerLiftSpeed * LeftInnerLiftUpDirection);
			RightInnerLiftMotor.Set(InnerLiftSpeed * RightInnerLiftUpDirection);
		}
		else if (InnerLiftUp == false && InnerLiftDown == true){
			LeftInnerLiftMotor.Set(InnerLiftSpeed * LeftInnerLiftUpDirection * -1);
			RightInnerLiftMotor.Set(InnerLiftSpeed * RightInnerLiftUpDirection * -1);
		}
		else{
			LeftInnerLiftMotor.Set(0.0);
			RightInnerLiftMotor.Set(0.0);
		}
		*/
	}

	//Outer lift control
	void OuterLiftControl(void){
		//local declarations- get pov input
		int leftPOV = LeftOpStick.GetPOV();
		int rightPOV = RightOpStick.GetPOV();
		float rightElbowValue = RightOpStick.GetZ();
		float leftElbowValue = LeftOpStick.GetZ();

		//Local Declarations-- makes joystick input close to zero zero
		float elbowThreshold = 0.3;

		//left stick motor control
		if (leftPOV == 315 || leftPOV == 0 || leftPOV == 45){
			LeftOuterLiftMotor.Set(leftArmSpeed);
		}
		else if (leftPOV == 135 || leftPOV == 180 || leftPOV == 225){
			LeftOuterLiftMotor.Set(leftArmSpeed * -1);
		}
		else{
			LeftOuterLiftMotor.Set(0.0);
		}

		//Right stick motor control
		if (rightPOV == 315 || rightPOV == 0 || rightPOV == 45){
			RightOuterLiftMotor.Set(rightArmSpeed);
		}
		else if (rightPOV == 135 || rightPOV == 180 || rightPOV == 225){
			RightOuterLiftMotor.Set(rightArmSpeed * -1);
		}
		else{
			RightOuterLiftMotor.Set(0.0);
		}

		//Elbow motor Controls
		if(rightElbowValue >= elbowThreshold || rightElbowValue <= -elbowThreshold ){
			RightElbowMotor.Set(rightElbowValue * rightArmElbowInDirection);
		}
		else{
			RightElbowMotor.Set(0.0);
		}

		if(leftElbowValue >= elbowThreshold || leftElbowValue <= -elbowThreshold){
			LeftElbowMotor.Set(leftElbowValue * leftArmElbowInDirection);
		}
		else{
			LeftElbowMotor.Set(0.0);
		}
	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
