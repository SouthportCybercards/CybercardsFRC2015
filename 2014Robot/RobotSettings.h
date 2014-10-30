//User settings MyRobot.cpp

#ifndef __ROBOTSETTINGSH__
#define __ROBOTSETTINGSH__

//potentiometer position values in terms of degrees
//kPotentiometerDegreeTable [9] = {-30, 0, 45, 60, 90, 120, 135, 180, 210};
//kPotentiometerVoltageTable [9] = {6, 8, 13, 14, 16, 19, 21, 24, 26};
#define kBlockPosition 90.0
#define kCatch_Position 90.0
#define kCarry_Position 90.0
#define kFloorLoadPosition 50.0
#define kHighGoalPosition 135.0
#define kLow_Goal_Position 90.0
#define kHighPassPosition 160.0
#define kMinimumAngle -30.0
#define kMaximumAngle 210.0

//tolerance deadzone for the potentiometer in volts
#define kArmPositionTolerance HOME_POS_DEADBAND //0.5

#define ARM_HOME_ROTATE_DIR -1

//size of the conversion table for the conversion between degrees and volts
#define kSizeOfTable 9
//3179466319 Mikes number
//Motor drive speeds (from -1 full power reverse to 1 full power forward)
#define kLaunchMotorSpeed 0.8
#define kPassMotorSpeed 0.5
#define kHoldMotorSpeedOut 1.0
#define kHoldMotorHoldSpeed 0.1
#define kIntakeMotorSpeed 0.6
#define kMotorStopped 0.0

//Joystick mapping buttons
#define kEject_Ball_Button 1
#define kCatch_Button 2
#define kManual_Mode_Button 6
#define kPrep_Low_Goal_Button 8
#define kPrep_HighPassButton 9
#define kPrep_High_Goal_Button 10
#define kCarryButton 11
#define kFloorLoadButton 12

//TODO
#define HOME_BUTTON 11
#define HOME_RESET_BUTTON 7
#define HOME_POS_IN_OHMS 750
#define HOME_POS_DEADBAND 2

//arm position rotate speed
#define kArmRotateSpeed 1

//arm motors direction definition
#define kLeftSideArmMotorDirectionIn 1
#define kLeftSideArmMotorDirectionOut -1
#define kRightSideArmMotorDirectionIn -1
#define kRightSideArmMotorDirectionOut 1

//the number of cycles(in milliseconds) to stay in eject ball state while shooting ball
#define kTimeToEjectBall 5000

//limit switch pressed definition
#define kLimitSwitchPressed 0

//ignore these values!!!!
#define K_BLOCK_ARM_ANGLE 90.0
#define K_BLOCK_OUT_SPD 0.0
#define K_BLOCK_IN_SPD 0.0
#define K_BLOCK_BALL_CAUGHT false

//deadzone values for various joystick inputs
#define DEFAULT_DEADZONE_VAL 0.75
#define ARM_MM_DEADZONE_VAL 0.1
#define DRIVE_MM_DEADZONE_VAL 0.2
#define EJWHEEL_MM_DEADZONE_VAL 0.2

#endif
