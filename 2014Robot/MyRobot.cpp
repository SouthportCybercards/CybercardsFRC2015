#include "WPILib.h"
#include <iostream.h>
#include <iomanip.h>
#include "MyRobot.h"
#include "RobotSettings.h"
#include "RobotState.h"

using namespace std;

/**
 * The 2014 CyberCards FRC robot code
 */ 
class OurRobotDemo : public SimpleRobot
{
	//declaring talon motor drivers
	Talon motor1;
	Talon motor2;
	Talon motor3;
	Talon motor4;
	Talon motor5;
	Talon motor6;
	
	//Arm intake, hold, and fire motors TODO SHOULD BE TALONS!!!!
	Talon leftArmIntakeMotor;
	Talon leftArmHoldMotor;
	Talon rightArmIntakeMotor;
	Talon rightArmHoldMotor;
	
	//Arm angle talon motor drivers
	Talon leftArmAngleMotor;
	Talon rightArmAngleMotor;
	
	//declaring joysticks
	Joystick leftDriveStick; 
	Joystick rightDriveStick;
	Joystick Operator_Control_Stick;
	
	//Potentiometer
	AnalogChannel armPotentiometer1;
	//AnalogChannel armPotentiometer2;
	
	//limit Switches
	DigitalInput limitSwitch1;
	DigitalInput limitSwitch2;
	
    int16_t potentiometerValueRight;
    //int16_t potentiometerValueLeft;
	float manualModeThrottlePosition;
    float manualModeArmPosition;
    bool inPosition;
    bool ballCaughtLimitSwitch1;
    bool ballCaughtLimitSwitch2;
    RobotStates_t robotState;
	int ejectBallTimer;
	
	//
    bool returnToHome;
    float homePositionInOhms;
    bool isAutonomous;
	
public:
	/**
	 * Constructor for our robot class
	 * @note This constructor uses the initializer notation to efficiently 
	 *  initialize the class.
	 *  First number in parenthesis is the sidecar the motor is on
	 *  Second number in parenthesis is the port on the sidecar
	 */
	OurRobotDemo():		
		motor1(1, 1),
		motor2(1, 2),
		motor3(1, 3),
		motor4(1, 4),
		motor5(1, 5),
		motor6(1, 6),
		leftArmIntakeMotor(2, 1),
		leftArmHoldMotor(2, 2),
		rightArmIntakeMotor(2, 4),
		rightArmHoldMotor(2, 3),
		leftArmAngleMotor(2, 5),
		rightArmAngleMotor(2, 6),
		leftDriveStick(1),
		rightDriveStick(2),
		Operator_Control_Stick(3),
		armPotentiometer1(1),
		//armPotentiometer2(2),
		limitSwitch1(2, 1),
		limitSwitch2(2, 2),
		//VALUES FROM HERE DOWN!!!! values in parenthesis are the initial value upon power up
    	potentiometerValueRight(kPotInitValue),
    	//potentiometerValueLeft(kPotInitValue),
    	manualModeThrottlePosition(0.0),
    	manualModeArmPosition(0.0),
    	inPosition(false),
    	ballCaughtLimitSwitch1(true),
    	ballCaughtLimitSwitch2(true),
    	robotState(kManualMode),
		ejectBallTimer(0)
		{
			//GetWatchdog().SetExpiration(0.1); // Must feed watchdog every 100mS
		
			//possRobotStates = {new RobotState("kBlock", K_BLOCK_ARM_ANGLE, K_BLOCK_OUT_SPD, K_BLOCK_IN_SPD, K_BLOCK_BALL_CAUGHT), 
			//		new RobotState("kFloorLoad")};
		
			//Default to homing
			returnToHome = true;
			
			//Read the current home position
			///NOTE - this requires that the robot start with the arms up in the positon to home to.
			homePositionInOhms = armPotentiometer1.GetValue();
			
			isAutonomous = false;
		}
	
	/**
	 * This code is run in the Autonomous mode
	 */
	void Autonomous()
	{
		isAutonomous = true;
		returnToHome = true;
		
		//GetWatchdog().SetEnabled(false);
		int i = 0;
		float AutomSpeed = 0.5; //drive motors half speed
		int AutomDuration = 100; //TODO make sure 40 ms is long enough
		
		//TODO remove after next match
		//Wait(3);
		
		//resetting counter
		i = 0;
		
		//TODO - double the autonomous length
		while(i < 2*AutomDuration)
		{
			//When we get to half way thru the time, stop monving, but continue control on the arms
			if(i >= AutomDuration){
				AutomSpeed = 0.0;
			}
			
			//TODO 
			potentiometerValueRight = armPotentiometer1.GetValue();
			ReturnHomeControl_REMOVE_ASAP();
			
			//Drive left motors at half speed
			motor1.Set(AutomSpeed);
			motor3.Set(AutomSpeed);
			motor5.Set(AutomSpeed);
			
			//Drive right motors at half speed
			motor2.Set(-1 * AutomSpeed);
			motor4.Set(-1 * AutomSpeed);
			motor6.Set(-1 * AutomSpeed);
			
			Wait(.01);
			i++;
			
			cout << "AUTO: potValue: " << potentiometerValueRight << ", Home Pos: " << homePositionInOhms << ", Return: " << returnToHome << endl;
						
		}
		
		//Drive left 
		//motor1.Set(0.0);
		//motor3.Set(0.0);
		//motor5.Set(0.0);
		
		//Drive right motors at half speed
		//motor2.Set(0.0);
		//motor4.Set(0.0);
		//motor6.Set(0.0);
		
		//TODO
		//while(1){
		//	ReturnHomeControl_REMOVE_ASAP();
		//}
		
	}
	
	/** Function to retrieve the user inputs
	 * Inputs (For all user input modes):
	 * * Potentiometers 
	 * * Limit switches
	 * * the requested state for the robot (from the joystick buttons)
	 * For manual mode only:
	 * * The intake and ejection wheel speeds - based on the operator joystick throttle
	 * * The arm position - based on the operator joystick Y axis  
	 */
    void GetUserInputs(RobotStates_t &requestedState)
    {
    	float throttleRaw = 0.0;
        //Read potentiometer goes from -5 to 28
        potentiometerValueRight = armPotentiometer1.GetValue();
        //potentiometerValueLeft = armPotentiometer2.GetValue();
        ballCaughtLimitSwitch1 = limitSwitch1.Get(); //TODO 0 is no ball 1 is ball 
        //Read Joystick state buttons
        requestedState = ReadJoystickButtons();
        throttleRaw = DeadZone(Operator_Control_Stick.GetTwist(), EJWHEEL_MM_DEADZONE_VAL);
        manualModeThrottlePosition = (throttleRaw * -1); //invert throttle value to match the joystick
		manualModeArmPosition = DeadZone(Operator_Control_Stick.GetY(), ARM_MM_DEADZONE_VAL);
		
    }

    /**
     * Function to control and actuate all us er inputs related to the drive base
     * * Left Joystick - the Y axis will control the left drive base motors forward or back.
     * * Right Joystick - the Y axis will control the right drive base motors forward or back.
     */
    void ManualDriveBaseActuation()
    {
        //get joystick inputs with GetY and filter 
        float lefty = DeadZone(leftDriveStick.GetY(), DRIVE_MM_DEADZONE_VAL);
        float righty = DeadZone(rightDriveStick.GetY(), DRIVE_MM_DEADZONE_VAL);
        //float arm = DeadZone(Operator_Control_Stick.GetY());
        //driving left side inverted
        motor1.Set(-1 * lefty);
        motor3.Set(-1 * lefty);
        motor5.Set(-1 * lefty);
        //driving right Side
        motor2.Set(righty);
        motor4.Set(righty);
        motor6.Set(righty);
    }
    /**
     * Function to determine the proper motor response to the current state
     */
    void StateBasedOutput()
    {
        /* State Machine where outputs are being driven */
        switch (robotState){
			case kManualMode:
				inPosition = true;
				
				//drive arm angle motors based on joystick position
				leftArmAngleMotor.Set(manualModeArmPosition * kArmRotateSpeed);
				rightArmAngleMotor.Set(-1 * manualModeArmPosition * kArmRotateSpeed);				
				
				//drive arm inner wheels based on throttle and trigger press
				if((manualModeThrottlePosition > 0) && (Operator_Control_Stick.GetRawButton(kEject_Ball_Button) == 1)){
					leftArmHoldMotor.Set(kLeftSideArmMotorDirectionOut * kHoldMotorSpeedOut);
					rightArmHoldMotor.Set(kRightSideArmMotorDirectionOut * kHoldMotorSpeedOut);
					//drive arm outer wheels based on joystick "throttle"
					leftArmIntakeMotor.Set(manualModeThrottlePosition * kLaunchMotorSpeed);
					rightArmIntakeMotor.Set(-1 * manualModeThrottlePosition * kLaunchMotorSpeed);
				}else if(manualModeThrottlePosition < 0){
					leftArmHoldMotor.Set(kLeftSideArmMotorDirectionIn * kIntakeMotorSpeed);
					rightArmHoldMotor.Set(kRightSideArmMotorDirectionIn * kIntakeMotorSpeed);
					//drive arm outer wheels based on joystick "throttle"
					leftArmIntakeMotor.Set(manualModeThrottlePosition * kIntakeMotorSpeed);
					rightArmIntakeMotor.Set(-1 * manualModeThrottlePosition * kIntakeMotorSpeed);
				}else{
					leftArmHoldMotor.Set(kLeftSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
					rightArmHoldMotor.Set(kRightSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
					//drive arm outer wheels based on joystick "throttle"
					leftArmIntakeMotor.Set(manualModeThrottlePosition * kLaunchMotorSpeed);
					rightArmIntakeMotor.Set(-1 * manualModeThrottlePosition * kLaunchMotorSpeed);
				}
				break;
			case kBlock:
				inPosition = MoveArmToSetPoint(kBlockPosition, potentiometerValueRight);
				leftArmIntakeMotor.Set(kMotorStopped);
				rightArmIntakeMotor.Set(kMotorStopped);
				leftArmHoldMotor.Set(kMotorStopped);
				rightArmHoldMotor.Set(kMotorStopped);
				break;
			case kFloorLoad:
				inPosition = MoveArmToSetPoint(kFloorLoadPosition, potentiometerValueRight);
				leftArmIntakeMotor.Set(kLeftSideArmMotorDirectionIn * kIntakeMotorSpeed);
				rightArmIntakeMotor.Set(kRightSideArmMotorDirectionIn * kIntakeMotorSpeed);
				leftArmHoldMotor.Set(kLeftSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
				rightArmHoldMotor.Set(kRightSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
				break;
			case kCatch:
				inPosition = MoveArmToSetPoint(kCatch_Position, potentiometerValueRight);
				leftArmIntakeMotor.Set(kLeftSideArmMotorDirectionIn * kIntakeMotorSpeed);
				rightArmIntakeMotor.Set(kRightSideArmMotorDirectionIn * kIntakeMotorSpeed);
				leftArmHoldMotor.Set(kLeftSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
				rightArmHoldMotor.Set(kRightSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
				break;
			case kCarry:
				inPosition = MoveArmToSetPoint(kCarry_Position, potentiometerValueRight);
				leftArmIntakeMotor.Set(kMotorStopped);
				rightArmIntakeMotor.Set(kMotorStopped);
				leftArmHoldMotor.Set(kLeftSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
				rightArmHoldMotor.Set(kRightSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
				break;
			case kPrepForHighGoal:
				inPosition = MoveArmToSetPoint(kHighGoalPosition, potentiometerValueRight);
				//wind up motors
				leftArmIntakeMotor.Set(kLeftSideArmMotorDirectionOut * kLaunchMotorSpeed);
				rightArmIntakeMotor.Set(kRightSideArmMotorDirectionOut * kLaunchMotorSpeed);
				leftArmHoldMotor.Set(kLeftSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
				rightArmHoldMotor.Set(kRightSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
				break;
			case kPrepForLowGoal:
				inPosition = MoveArmToSetPoint(kLow_Goal_Position, potentiometerValueRight);
				//if wheels are wound up to right speed
				leftArmIntakeMotor.Set(kLeftSideArmMotorDirectionOut * kPassMotorSpeed);
				rightArmIntakeMotor.Set(kRightSideArmMotorDirectionOut * kPassMotorSpeed);
				leftArmHoldMotor.Set(kLeftSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
				rightArmHoldMotor.Set(kRightSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
				break;
			case kPrepForHighPass:
				inPosition = MoveArmToSetPoint(kHighPassPosition, potentiometerValueRight);
				//if wheels are wound up to right speed
				leftArmIntakeMotor.Set(kLeftSideArmMotorDirectionOut * kLaunchMotorSpeed);
				rightArmIntakeMotor.Set(kRightSideArmMotorDirectionOut * kLaunchMotorSpeed);
				leftArmHoldMotor.Set(kLeftSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
				rightArmHoldMotor.Set(kRightSideArmMotorDirectionIn * kHoldMotorHoldSpeed);
				break;
			case kEjectBall:
				inPosition = MoveArmToSetPoint(kHighPassPosition, potentiometerValueRight);
				//move holding wheels forward for time
				leftArmIntakeMotor.Set(kLeftSideArmMotorDirectionOut * kLaunchMotorSpeed);
				rightArmIntakeMotor.Set(kRightSideArmMotorDirectionOut * kLaunchMotorSpeed);
				leftArmHoldMotor.Set(kLeftSideArmMotorDirectionOut * kHoldMotorSpeedOut);
				rightArmHoldMotor.Set(kRightSideArmMotorDirectionOut * kHoldMotorSpeedOut);
				break;
			}
    }

    /**
	 * Main function to run during teleop mode
	 * Performs the following
	 *  * Initializes the motor drive to run safely
	 *  * Controls the motors with arcade steering.
	 */
    void OperatorControl()
    {
        //GetWatchdog().SetEnabled(true);
    	
    	isAutonomous = false;
    	
        //Local declarations		
        RobotStates_t requestedState = kManualMode;
        robotState = kManualMode;
        homePositionInOhms = armPotentiometer1.GetValue();
        
        //runs the code in this section over and over forever
        while(IsOperatorControl() && !IsDisabled()){
        	
            //GetWatchdog().Feed();
			
        	//Get inputs from Joystick and sensors
            GetUserInputs(requestedState);
			
			//Transition to state, control function
			robotState = StateTransitionControl(requestedState);
			
			//set arm motor outputs based on state 
			StateBasedOutput();
			
			//get drive base joysticks and drive motors on base
			ManualDriveBaseActuation();
			
			//TODO - this is basically hijacking / defining arm state
			// This should be incorporated into the default state machine ASAP
			ResetHomePosition();
			ReturnHomeControl_REMOVE_ASAP();
			
			//printing values out to the console
			//if(!IsDisabled()){
			//	cout << "State requested: " << robotState << ", Pot voltage: " << potentiometerValueRight << ", Switch: " << ballCaughtLimitSwitch1 << endl;
			//}
			//DEBUG
			cout << "TELEOP: throttle: " << manualModeThrottlePosition << ", potValue: " << potentiometerValueRight << ", Home Pos: " << homePositionInOhms << ", Return: " << returnToHome << endl;
			
			//wait delay in main loop for motor update
			Wait(0.01);
			
		}
	}
	
    //
    void ResetHomePosition(void){
    	
    	if(Operator_Control_Stick.GetRawButton(HOME_RESET_BUTTON) == true){
    		homePositionInOhms = armPotentiometer1.GetValue();
		}
    	
    }
    
    //Jenky as hell
    void ReturnHomeControl_REMOVE_ASAP(void){
    	
    	if(isAutonomous == false){
			//This is jenky as hell
			if(Operator_Control_Stick.GetRawButton(HOME_BUTTON) == true){
				returnToHome = true;
			}
			
			//TODO - perhaps add a debounce counter to this?
			
			//Conditionally return to home
			if(returnToHome == true){
				MoveArmToSetPoint(homePositionInOhms, potentiometerValueRight);
			}
			
			//Determine if stop control to home
			if(manualModeArmPosition != 0){
				returnToHome = false;
			}
    	} else {
    		
    		//Conditionally return to home
			if(returnToHome == true){
				MoveArmToSetPoint(homePositionInOhms, potentiometerValueRight);
			}
    		
    	}
    	
    }
    
	/**
	 * Runs during test mode
	 */
	void Test() {

	}
	/* Function MoveArmToSetPoint
	 * moves the arm to a position on the arc
	 * 
	 */
	bool MoveArmToSetPoint(float setPoint, int16_t currentPosition){
		
		//local variables
		//float setPoint = 0;
		
		//TODO - may need to scale the rotation speed based on how close we are to the setpoint
		
		//convert from degrees to potentiometer value
		//setPoint = ArmAngleLookUp1D(setPointDegrees);
		//determining how to drive motors
		if(currentPosition > (setPoint + kArmPositionTolerance)){
			leftArmAngleMotor.Set(-1 * ARM_HOME_ROTATE_DIR * kArmRotateSpeed);
			rightArmAngleMotor.Set(ARM_HOME_ROTATE_DIR * kArmRotateSpeed);
			return(false);
		}else if(currentPosition < (setPoint - kArmPositionTolerance)){
			leftArmAngleMotor.Set(ARM_HOME_ROTATE_DIR * kArmRotateSpeed);
			rightArmAngleMotor.Set(-1 * ARM_HOME_ROTATE_DIR * kArmRotateSpeed);
			return(false);
		}else{
			leftArmAngleMotor.Set(kMotorStopped);
			rightArmAngleMotor.Set(kMotorStopped);
			return(true);
		}
	}
	/* Function ReadJoystickButtons
	 * runs through all joystick buttons and reads them
	 * 
	 */
	RobotStates_t ReadJoystickButtons(void){
		
		RobotStates_t request = kManualMode;
		
		/* interpret joystick state buttons
		 *
		 */
		if(Operator_Control_Stick.GetRawButton(kManual_Mode_Button)){
			request = kManualMode;
		}else if(Operator_Control_Stick.GetRawButton(kPrep_Low_Goal_Button)){
			request = kPrepForLowGoal;
		}else if(Operator_Control_Stick.GetRawButton(kPrep_HighPassButton)){
			request = kPrepForHighPass; 
		}else if(Operator_Control_Stick.GetRawButton(kPrep_High_Goal_Button)){
			request = kPrepForHighGoal;
		}else if(Operator_Control_Stick.GetRawButton(kCarryButton)){
			request = kCarry;
		}else if(Operator_Control_Stick.GetRawButton(kFloorLoadButton)){
			request = kFloorLoad;
		}else if(Operator_Control_Stick.GetRawButton(kCatch_Button)){
			request = kCatch;
		}
		return request;
	}
	
	/* maps the degree angle of the arms to the potentiometer value
	 * Input: degree measurement
	 * Output: potentiometer value
	 */
	float ArmAngleLookUp1D(float angleInDegrees){
//		
		//local variables
//		int i = 0;
//		int kPotentiometerDegreeTable [9] = {-30, 0, 45, 60, 90, 120, 135, 180, 210};
//		int kPotentiometerVoltageTable [9] = {6, 8, 13, 14, 16, 19, 21, 24, 26};
		float outputValue = kMinimumAngle;
//		
		if(angleInDegrees <= kMinimumAngle){
			outputValue = kMinimumAngle;
		}else if(angleInDegrees >= kMaximumAngle){
			outputValue = kMaximumAngle;
		}else{
			outputValue = (((1.0/15.0) * angleInDegrees) + 8.0); //120
		}
//		
//		for(i = 0; i < kSizeOfTable; i++){
//			if(angleInDegrees <= kPotentiometerDegreeTable[i]){
//				outputValue = kPotentiometerVoltageTable[i];
//			}else if(angleInDegrees < kPotentiometerDegreeTable[i+1]){
//				//outputValue = (((1/15) * kPotentiometerVoltageTable[i]) + 8);
//				(kPotentiometerDegreeTable[i+1] - kPotentiometerDegreeTable[i])/(kPotentiometerVoltageTable[i+1] - kPotentiometerVoltageTable[i]); 
//			}
//		}
//		if(i == kSizeOfTable){
//			outputValue = kPotentiometerVoltageTable[kSizeOfTable - 1];
//		}
//		
		return outputValue;
	}
	
	RobotStates_t StateTransitionControl(RobotStates_t requestedState){
	
		//DEBUG - shortcutting all the state transitions.  Default to manual mode ONLY.
		requestedState = kManualMode;
		
		//get trigger button from joystick
		bool Eject_Ball_Button_Pressed = Operator_Control_Stick.GetRawButton(kEject_Ball_Button);
		RobotStates_t goToState = robotState;
		
		
		//determine the next state to go to
		if(requestedState == kManualMode){
			goToState = kManualMode;
		}else{
			switch (robotState){
			case kManualMode:
				if(ballCaughtLimitSwitch1 == kLimitSwitchPressed){//have a ball
					goToState = kCarry;
				}else if(requestedState == kFloorLoad){
					goToState = kFloorLoad;
				}else if(requestedState == kCatch){
					goToState = kCatch;
				}else if(requestedState == kBlock){
					goToState = kBlock;
				}
				break;
			case kBlock:
				if(requestedState == kFloorLoad){goToState = kFloorLoad;}
				else if(requestedState == kCatch){goToState = kCatch;}
				break;
			case kFloorLoad:
				if((requestedState == kCarry) && (ballCaughtLimitSwitch1 == kLimitSwitchPressed)){//touch sensors pressed
					goToState = kCarry;
				}else if((requestedState == kBlock) && (ballCaughtLimitSwitch1 != kLimitSwitchPressed)){
					goToState = kBlock;
				}
				break;
			case kCatch:
				if((requestedState == kFloorLoad) && (ballCaughtLimitSwitch1 != kLimitSwitchPressed)){//if no ball
					goToState = kFloorLoad;
				}else if((requestedState == kBlock) && (ballCaughtLimitSwitch1 != kLimitSwitchPressed)){//if no ball
					goToState = kBlock;
				}else if(ballCaughtLimitSwitch1 == kLimitSwitchPressed){//ball caught
					goToState = kCarry;
				}
				break;
			case kCarry:
				if(requestedState == kPrepForHighGoal){goToState = kPrepForHighGoal;}
				else if(requestedState == kPrepForLowGoal){goToState = kPrepForLowGoal;}
				else if(requestedState == kPrepForHighPass){goToState = kPrepForHighPass;}
				break;
			case kPrepForHighGoal:
				if((requestedState == kEjectBall) && (Eject_Ball_Button_Pressed == true) && (inPosition == true)){
					goToState = kEjectBall;
				}else if(requestedState == kCarry){
					goToState = kCarry;
				}
				break;
			case kPrepForLowGoal:
				if((requestedState == kEjectBall) && (Eject_Ball_Button_Pressed == true) && (inPosition == true)){
					goToState = kEjectBall;
				}else if(requestedState == kCarry){
					goToState = kCarry;
				}
				break;
			case kPrepForHighPass:
				if((requestedState == kEjectBall) && (Eject_Ball_Button_Pressed == true) && (inPosition == true)){
					goToState = kEjectBall;
				}else if(requestedState == kCarry){
					goToState = kCarry;
				}
				break;
			case kEjectBall:
				if((ballCaughtLimitSwitch1 != kLimitSwitchPressed) && (ejectBallTimer >= kTimeToEjectBall)){//ball released and time limit
					goToState = kBlock;
					ejectBallTimer = 0; //reset ejectBallTimer
				}else{
					ejectBallTimer++;
				}
				break;
			}
		}
		return goToState;
	}
	
	/*Function: DeadZone
	 * filters joystick inputs
	 * so that they will not return any
	 * value lower than .05% so motors
	 * don't drift.
	 */ 
	float DeadZone(float stick, float deadZoneVal){
		//if stick value near zero filter
		if((stick <= deadZoneVal) && (stick >= (-1 * deadZoneVal))){
			return 0;
		}
		else{
			if(deadZoneVal > 0)
			{
				return (stick - deadZoneVal);
			}else if(deadZoneVal < 0)
			{
				return (stick + deadZoneVal);
			}else{
				return 0;
			}
		}
	}

}; 

START_ROBOT_CLASS(OurRobotDemo);

//Heyy :)Got to go, it's 7, love you bro bro <3
