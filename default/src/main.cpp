/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Authors:      Ryan McLoughlin & David Barsoum                           */
/*    Created:      Mon September 27 2021                                     */
/*    Description:  Primary Software for Team 9421R                           */
/*                                                                            */
/*    Side Note: This program uses a competion template created by Ryan       */
/*               McLoughlin and Mr. Chesnais early on in 2020 and uses        */
/*               modified JPearman buttons functions.                         */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// frontLeft            motor         3               
// frontRight           motor         7               
// backLeft             motor         1               
// backRight            motor         9               
// Controller1          controller                    
// leftMiddle           motor         2               
// rightMiddle          motor         8               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"
#include <cmath>

using namespace vex;

// A global instance of competition
competition Competition;

// Capability choice for autonomous modes - these reflect which subsystem/capability is enabled 
int autonomousSelection = -1;

// false red, true blue - impacts distances and turn direction
bool allianceBlue = false;

//reverse drive and clamp booleans
volatile bool driveIsReversed = false;
bool clampState = true;

/*
James Pearman autoselect functions and definitions. These are modified for FHS and begin below.
*/

// collect data for on screen button and include off and on color feedback for button
// prc - instead of radio approach with one button on or off at a time
typedef struct _button
{
  int    xpos;
  int    ypos;
  int    width;
  int    height;
  bool   state;
  vex::color offColor;
  vex::color onColor;
  const char *label;
} button;

// Button array definitions for each software button. The purpose of each button data structure
// is defined above.  The array size can be extended, so you can have as many buttons as you
// wish as long as it fits.

//Custom buttons and colors here
button buttons[] =
{
  {15,  50, 90, 90,  false, 0x1D1F3F, 0x6C6C6D},
  {15,  145, 90, 90,  false, 0x1D1F3F, 0x6C6C6D},
  {135,  50, 90, 90,  false, 0x1D1F3F, 0x6C6C6D},
  {135,  145, 90, 90,  false, 0x1D1F3F, 0x6C6C6D},
  {255, 50, 90, 90,  false, 0x1D1F3F, 0x6C6C6D},
  {255, 145, 90, 90,  false, 0x1D1F3F, 0x6C6C6D},
  {375, 50, 90, 90,  false, 0x1D1F3F, 0x6C6C6D},
  {375, 145, 90, 90,  false, 0x1D1F3F, 0x6C6C6D}
};

// forward ref
void displayButtonControls(int index, bool pressed);

/*-----------------------------------------------------------------------------*/
/** @brief      Check if touch is inside button                                */
/*-----------------------------------------------------------------------------*/
int findButton(int16_t xpos, int16_t ypos)
{
  int nButtons = sizeof(buttons) / sizeof(button);
  for( int index=0;index < nButtons;index++) 
  {
    button *pButton = &buttons[ index ];
    if( xpos < pButton->xpos || xpos > (pButton->xpos + pButton->width) )
      continue;
    if( ypos < pButton->ypos || ypos > (pButton->ypos + pButton->height) )
      continue;
    return(index);
  }
  return (-1);
}

/*-----------------------------------------------------------------------------*/
/** @brief      Init button states                                             */
/*-----------------------------------------------------------------------------*/
void initButtons()
{
  int nButtons = sizeof(buttons) / sizeof(button);
  for(int index=0;index < nButtons;index++)
  {
    buttons[index].state = false;
  }
}

/*-----------------------------------------------------------------------------*/
/** @brief      Screen has been touched                                        */
/*-----------------------------------------------------------------------------*/
void userTouchCallbackPressed()
{
  int index;
  int xpos = Brain.Screen.xPosition();
  int ypos = Brain.Screen.yPosition();
  if((index = findButton( xpos, ypos)) >= 0)
  {
    displayButtonControls(index, true);
  }
}

/*-----------------------------------------------------------------------------*/
/** @brief      Screen has been (un)touched                                    */
/*-----------------------------------------------------------------------------*/
void userTouchCallbackReleased()
{
  int index;
  int xpos = Brain.Screen.xPosition();
  int ypos = Brain.Screen.yPosition();
  if((index = findButton(xpos, ypos)) >= 0)
  {
    // clear all buttons to false, ie. unselected
    //      initButtons();
    // now set this one as true
    if( buttons[index].state == true)
    {
    buttons[index].state = false;
    }
    else
    {
    buttons[index].state = true;
    }
    // save as auton selection
    autonomousSelection = index;
    displayButtonControls(index, false );
  }
}

/*-----------------------------------------------------------------------------*/
/** @brief      Draw all buttons                                               */
/*-----------------------------------------------------------------------------*/
void displayButtonControls(int index, bool pressed)
{
  vex::color c;
  Brain.Screen.setPenColor(vex::color(0xe0e0e0));
  for(int i=0;i<sizeof(buttons)/sizeof(button);i++)
  {
    if(buttons[i].state)
    {
      c = buttons[i].onColor;
    }
    else
    {
      c = buttons[i].offColor;
    } 
    Brain.Screen.setFillColor(c);
    // button fill
    if(i == index && pressed == true)
    {
      Brain.Screen.drawRectangle(buttons[i].xpos, buttons[i].ypos, buttons[i].width, buttons[i].height, c);
    }
    else
      Brain.Screen.drawRectangle(buttons[i].xpos, buttons[i].ypos, buttons[i].width, buttons[i].height);
    // outline
    Brain.Screen.drawRectangle(buttons[i].xpos, buttons[i].ypos, buttons[i].width, buttons[i].height, vex::color::transparent);
    // draw label
    if(buttons[i].label != NULL)
      Brain.Screen.printAt(buttons[i].xpos + 8, buttons[i].ypos + buttons[i].height - 8, buttons[i].label);
  }
}
/*
James Pearman autoselect functions and definitions which have been modified for FHS end here.
*/

void driveSetVelocity(double speed)
{
 leftSide.setVelocity(speed, pct);
 rightSide.setVelocity(speed, pct);
}

void driveRotateFor(double numRevolutions)
{
 leftSide.rotateFor(numRevolutions, rev, false);
 rightSide.rotateFor(numRevolutions, rev, false);
}


void driveDistance(double distance)
{
   driveRotateFor(distance * 0.059);
}

int getTrueDegreeVal(int inertialDegreeeVal)
{
  if (inertialSensor.heading() < 360 && inertialSensor.heading() > 180)
  {
    return inertialSensor.heading() - 360;
  }
  else 
  {
    return inertialSensor.heading();
  }
  
}

//new pid function
void driveForward(double dist, double speedAsPct)
{
  int trueVal = getTrueDegreeVal(inertialSensor.heading());

  leftSide.setVelocity(speedAsPct, pct);
  rightSide.setVelocity(speedAsPct, pct);

  leftSide.spinFor(dist, rev);
  rightSide.spinFor(dist, rev);

  while (leftSide.isSpinning() == true)
  {
    trueVal = getTrueDegreeVal(inertialSensor.heading());
    rightSide.setVelocity(leftSide.velocity(percent) + 2.7 * trueVal, percent);
    wait(10, msec);
  }
}

void driveTurnFor(double degrees)
{
 leftSide.rotateFor(-degrees, rev, false);
 rightSide.rotateFor(degrees, rev, false);
}

void clampToggle()
{
  clamp.set(clampState);
}

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton(void)
{
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit(); 

  inertialSensor.calibrate();
  waitUntil(inertialSensor.isCalibrating() == false);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous(void)
{
  
  driveSetVelocity(100);
  lift.setVelocity(100, pct);
  lift.rotateFor(700, degrees);
  driveDistance(50);
  wait(2, sec);
  lift.spinFor(reverse, 200, degrees);
  //driveDistance(-40);
  driveDistance(-46);
  wait(2, sec);
  /*
  lift.spinFor(fwd, 200, degrees);
  driveDistance(-5);
  */
  
  robotDrivetrain.turnFor(50, degrees);
  //robotDrivetrain.turnFor(90, degrees);

}
  
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
 

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void usercontrol(void)
{
  int leftStickVal = 0;
  int rightStickVal = 0;
  lift.setVelocity(100, pct);
  fourbar.setVelocity(100, pct);

  while (true)
  {
    // Driving
    leftStickVal = Controller1.Axis3.position(percentUnits::pct);
    rightStickVal = Controller1.Axis2.position(percentUnits::pct);

    leftStickVal = pow(leftStickVal, 3) / 10000;
    rightStickVal = pow(rightStickVal, 3) / 10000;

    /*
      This fixes the reverse button issue, but why? 
      Controller1.ButtonB.pressed(switchDirection);
      Controller1.ButtonDown.pressed(switchDirection);
    */

    if (Controller1.ButtonB.pressing())
    {
      driveIsReversed = !driveIsReversed;
    }

    if (driveIsReversed)
    {
      leftSide.spin(reverse, rightStickVal, volt);
      rightSide.spin(reverse, leftStickVal, volt);
    }
    else
    {
      leftSide.spin(fwd, leftStickVal, volt);
      rightSide.spin(fwd, rightStickVal, volt);
    }

    
    if (Controller1.ButtonR1.pressing())
    {
      lift.spin(reverse);
    }
    else if (Controller1.ButtonR2.pressing())
    {
      lift.spin(forward);
    }
    else
    {
      lift.stop(hold);
    }


    if (Controller1.ButtonL1.pressing())
    {
      fourbar.spin(forward);
    }
    else if (Controller1.ButtonL2.pressing())
    {
      fourbar.spin(reverse);
    }
    else
    {
      fourbar.stop(hold);
    }

    Controller1.ButtonA.pressed(clampToggle);
    clampState = !clampState;

  }
    wait(20, msec);
}

// Main will set up the competition functions and callbacks for autonomous and driver control perious
int main()
{
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);
  double lbFtTorque = 12.0*(leftSide.torque(InLb) + rightSide.torque(InLb));

  // register events
  Brain.Screen.pressed(userTouchCallbackPressed);
  Brain.Screen.released(userTouchCallbackReleased);

  // Run the pre-autonomous function.
  pre_auton();
  Brain.Screen.setFillColor(white);
  Brain.Screen.drawRectangle(0, 0, 700, 400);
  Brain.Screen.drawImageFromFile("FlyerRobotics.png", 35, 0);
  displayButtonControls(0, false);
  //Brain.Screen.print(lbFtTorque);
}
