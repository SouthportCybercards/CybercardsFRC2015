#include "WPILib.h"
#include "Joystick.h"
#include <iostream>

class Robot: public IterativeRobot
{
	RobotDrive RobotDriveBase; // robot drive system
	Joystick LeftStick, RightStick, LeftOpStick, RightOpStick;
	Talon LeftOuterLiftMotor;
	Talon RightOuterLiftMotor;
	Talon LeftElbowMotor;
	Talon RightElbowMotor;
	Talon LeftInnerLiftMotor;
	Talon RightInnerLiftMotor;
	DigitalInput InnerLiftZeroSensor;
	Encoder InnerLiftMotorEncoder;
	Encoder LeftElbowMotorEncoder;
	Encoder RightElbowMotorEncoder;
	DigitalInput LeftElbowZeroSensor;
	DigitalInput RightElbowZeroSensor;
	AnalogInput LeftOuterLiftZeroSensor;
	AnalogInput RightOuterLiftZeroSensor;
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
	const int leftElbowAutoButton = 1;
	const int rightElbowAutoButton = 1;
	const int leftElbowAutoCenterButton = 2;
	const int rightElbowAutoCenterButton = 2;


	//Inner Lift constants
	const int innerLiftEncoderValue = 10;
	const int maxLevel = 6;
	const int minLevel = 0;

	//z axis constant
	const float elbowThreshold = 0.3;

	//Class members
	bool innerLiftStateControl;
	int targetLevel;
	int innerLiftEncoderZeroValue;
	int targetInnerLiftEncoderValue;

	//Global variables
	bool leftAutoUp_prev = false;
	bool leftAutoDown_prev = false;
	bool rightAutoUp_prev = false;
	bool rightAutoDown_prev = false;


	bool leftOuterHome_prev = false;
	bool rightOuterHome_prev = false;
	bool leftOuter;
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
		RobotDriveBase(9, 8, 7, 6),	// these must be initialized in the same order
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
		InnerLiftMotorEncoder(1, 2, false, InnerLiftMotorEncoder.k4X),
		LeftElbowMotorEncoder(3, 4, false, LeftElbowMotorEncoder.k4X),
		RightElbowMotorEncoder(5, 6, false, RightElbowMotorEncoder.k4X),
		LeftElbowZeroSensor(7),
		RightElbowZeroSensor(8),
		LeftOuterLiftZeroSensor(0),
		RightOuterLiftZeroSensor(1),
		lw(NULL),
		autoLoopCounter(0)
	{
		RobotDriveBase.SetExpiration(0.1);
		RobotDriveBase.SetInvertedMotor(RobotDriveBase.kFrontLeftMotor,false);
		RobotDriveBase.SetInvertedMotor(RobotDriveBase.kFrontRightMotor,false);
		RobotDriveBase.SetInvertedMotor(RobotDriveBase.kRearLeftMotor,false);
		RobotDriveBase.SetInvertedMotor(RobotDriveBase.kRearRightMotor,false);

		//Default variables to initial known state
		innerLiftStateControl = true;
		targetLevel = 0;
		innerLiftEncoderZeroValue = 0; //TODO - is this a good initial value?
		targetInnerLiftEncoderValue = 0; //TODO - is this a good initial value?
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
			RobotDriveBase.Drive(-0.5, 0.0); 	// drive forwards half speed
			autoLoopCounter++;
		} else if(autoLoopCounter >= 100) {
			 //Check if we've completed 100 loops (approximately 2 seconds)
			RobotDriveBase.Drive(0.5, 0.0); 	// drive backwards half speed
			autoLoopCounter++;
		} //else {
			//RobotDriveBase.Drive(0.0, 0.0); 	// stop robot
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

		ElbowControl();
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
			RobotDriveBase.TankDrive(-yAxis1,-yAxis2); 	// drive Forwards
		} else {
			RobotDriveBase.TankDrive(0.0, 0.0); 	// stop robot
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

		//Testing Motor encoder
		//int currentInnerLiftEncoderValue = InnerLiftMotorEncoder.Get();
		//std::cout << "currentEncoderValue:"  << currentInnerLiftEncoderValue << std::endl;

		//Testing limit switch
		//bool innerLiftSwitchVal = InnerLiftZeroSensor.Get();
		//std::cout << "innerLiftSwitchVal: " << innerLiftSwitchVal << std::endl;
	}

	//Function to control the inner lift with automatic state control
	void InnerLiftAutoControl(void){

		//Local declarations
		//int targetLevel;
		//int targetInnerLiftEncoderValue;
		//int innerLiftEncoderZeroValue;

		//Get the current states of the encoder and limit switches.
		int currentInnerLiftEncoderValue = InnerLiftMotorEncoder.Get();
		bool leftAutoUp = LeftOpStick.GetRawButton(leftInnerAutoUpButton);
		bool leftAutoDown = LeftOpStick.GetRawButton(leftInnerAutoDownButton);
		bool rightAutoUp = RightOpStick.GetRawButton(rightInnerAutoUpButton);
		bool rightAutoDown = RightOpStick.GetRawButton(rightInnerAutoDownButton);
		bool innerZeroLimitSwitch = InnerLiftZeroSensor.Get();

		//If lift is zeroed - based on the limit switch - set the class member innerLiftEncoderZeroValue
		//	to record what the zeroed encoder values is.
		//TODO - perhaps this should be run regardless if in auto control or not.
		if(innerZeroLimitSwitch == false){ //false=limit switch pressed
			innerLiftEncoderZeroValue = currentInnerLiftEncoderValue;
		}

		//Determine what the target level is, based on user input
		//If either of the state up buttons is pressed, update the target level to move to.
		//TODO - should we debounce this?
		// For example, we may want to only increment as we see an entire - button press then button release cycle.
		if (((leftAutoUp == true) && (leftAutoUp_prev == false)) || ((rightAutoUp == true) && (rightAutoUp_prev == false))){
			if (targetLevel < maxLevel){
				targetLevel++;

			}
		}
		//If either of the state down buttons is pressed, update the target level to move to.
		//TODO - should we debounce this?
		else if (((leftAutoDown == true) && (leftAutoDown_prev == false)) || ((rightAutoDown == true) && (rightAutoDown_prev == false))){
			if(targetLevel > minLevel){
				targetLevel--;
			}
		}
		else{
			//Intentionally empty
		}

		//Determine the target encoder value, based on the target level
		targetInnerLiftEncoderValue = ((targetLevel * innerLiftEncoderValue) + innerLiftEncoderZeroValue);

		//Based on the target encoder value, handle motor commands.
		if ((currentInnerLiftEncoderValue < targetInnerLiftEncoderValue) && innerZeroLimitSwitch){
			LeftInnerLiftMotor.Set(innerLiftSpeed * leftInnerLiftUpDirection);
			RightInnerLiftMotor.Set(innerLiftSpeed * rightInnerLiftUpDirection);
		}
		else if ((currentInnerLiftEncoderValue > targetInnerLiftEncoderValue) && innerZeroLimitSwitch){
			LeftInnerLiftMotor.Set(innerLiftSpeed * leftInnerLiftUpDirection * -1);
			RightInnerLiftMotor.Set(innerLiftSpeed * rightInnerLiftUpDirection * -1);
		}
		else{
			LeftInnerLiftMotor.Set(0.0);
			RightInnerLiftMotor.Set(0.0);
		}
		std::cout << "leftAutoUp:"  << leftAutoUp << " tarVal: " << targetLevel << "leftAutoUp_prev: " << leftAutoUp_prev << std::endl;
		leftAutoUp_prev = leftAutoUp;
		leftAutoDown_prev = leftAutoDown;
		rightAutoUp_prev = rightAutoUp;
		rightAutoDown_prev = rightAutoDown;
	}

	//Outer lift control
	void OuterLiftControl(void){
		//local declarations- get POV input
		int leftPOV = LeftOpStick.GetPOV();
		int rightPOV = RightOpStick.GetPOV();


		// 			+--------------------+
		//		   /|     			     |\
		// 	      / | 		Robot		 | \
		// 	     /  |    			     |  \
		// 	    /   +--------------------+   \
		//	home position				home position


		// 		+---------------------+
		//		|\     			     /|
		// 	    | \		Robot		/ |
		// 	    |  \  			   /  |
		// 	    +---------------------+
		//			center position R/L
		bool leftOuterHome;
		bool rightOuterHome;

		float leftOuterZeroSensorValue = LeftOuterLiftZeroSensor.GetVoltage();
		float rightOuterZeroSensorValue = RightOuterLiftZeroSensor.GetVoltage();

		if (leftOuterZeroSensorValue > 2.5){
			leftOuterHome = false;
		}
		else{
			leftOuterHome = true;
		}

		if (rightOuterZeroSensorValue > 2.5){
			rightOuterHome = false;
		}
		else{
			rightOuterHome = true;
		}

		//left stick motor control
		if ((leftPOV == 315 || leftPOV == 0 || leftPOV == 45) && !leftOuterHome){
			LeftOuterLiftMotor.Set(leftArmSpeed);
		}
		else if (leftPOV == 135 || leftPOV == 180 || leftPOV == 225){
			LeftOuterLiftMotor.Set(leftArmSpeed * -1);
		}
		else{
			LeftOuterLiftMotor.Set(0.0);
		}

		//Right stick motor control
		if ((rightPOV == 315 || rightPOV == 0 || rightPOV == 45) && !rightOuterHome){
			RightOuterLiftMotor.Set(rightArmSpeed);
		}
		else if (rightPOV == 135 || rightPOV == 180 || rightPOV == 225){
			RightOuterLiftMotor.Set(rightArmSpeed * -1);
		}
		else{
			RightOuterLiftMotor.Set(0.0);
		}
	}

	void ElbowControl(void){
		float leftElbowValue = LeftOpStick.GetZ();
		float rightElbowValue = RightOpStick.GetZ();

		bool leftElbowAuto = LeftOpStick.GetRawButton(leftElbowAutoButton);
		bool rightElbowAuto = RightOpStick.GetRawButton(rightElbowAutoButton);

		int currentLeftElbowEncoderValue = LeftElbowMotorEncoder.Get();
		int currentRightElbowEncoderValue = RightElbowMotorEncoder.Get();

		int targetLeftElbowEncoderValue;
		int targetRightElbowEncoderValue;

		int leftElbowEncoderZeroValue;
		int rightElbowEncoderZeroValue;

		bool leftElbowZeroSwitch = LeftElbowZeroSensor.Get();
		bool rightElbowZeroSwitch = RightElbowZeroSensor.Get();

		bool leftHome;
		bool leftScore;
		bool leftManual;
		bool rightHome;
		bool rightScore;
		bool rightManual;

		if (leftElbowZeroSwitch){
			leftElbowEncoderZeroValue = currentLeftElbowEncoderValue;
		}

		if (rightElbowZeroSwitch){
			rightElbowEncoderZeroValue = currentLeftElbowEncoderValue;
		}

		if ((leftElbowValue >= elbowThreshold) && leftElbowAuto){
			leftHome = false;
			leftScore = true;
			leftManual = false;
		}
		else if ((leftElbowValue <= -elbowThreshold) && leftElbowAuto){
			leftHome = true;
			leftScore = false;
			leftManual = false;
		}
		else{
			leftHome = false;
			leftScore = false;
			leftManual = true;
		}

		if ((rightElbowValue >= elbowThreshold) && rightElbowAuto){
			rightHome = true;
			rightScore = false;
			rightManual = false;
		}
		else if ((rightElbowValue <= -elbowThreshold) && rightElbowAuto){
			rightHome = false;
			rightScore = true;
			rightManual = false;
		}
		else{
			rightHome = false;
			rightScore = false;
			rightManual = true;
		}

		if (rightHome && !rightElbowZeroSwitch){
			RightElbowMotor.Set(rightElbowValue * rightArmElbowInDirection);
		}
		//else if (rightScore && ()){

		//}
		else{
			RightElbowMotor.Set(0.0);
		}

		/*Elbow motor Controls
		if(rightElbowValue >= elbowThreshold || rightElbowValue <= -elbowThreshold ){

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
		*/

	}
	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
