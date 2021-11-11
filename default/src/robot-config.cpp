#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
controller Controller1 = controller(primary);

inertial inertialSensor = inertial(PORT7);

motor frontLeft = motor(PORT8, ratio36_1, true);
motor frontRight = motor(PORT3, ratio36_1, false);

motor backLeft = motor(PORT10, ratio36_1, true);
motor backRight = motor(PORT1, ratio36_1, false);

motor middleLeft = motor(PORT9, ratio36_1, true);
motor middleRight = motor(PORT2, ratio36_1, false);

motor lift = motor(PORT5, ratio36_1, false);
motor fourbar = motor(PORT6, ratio36_1, false);

motor_group leftSide = motor_group(frontLeft, middleLeft, backLeft);
motor_group rightSide = motor_group(frontRight, middleRight, backRight);

smartdrive robotDrivetrain = smartdrive(leftSide, rightSide, inertialSensor, 299.24, 320, 40, mm, 1);

digital_out clamp = digital_out(Brain.ThreeWirePort.H);



// VEXcode generated functions
// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}