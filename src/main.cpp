/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Competition Template                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// Drivetrain           drivetrain    1, 2, 4, 21     
// indexer              motor         6               
// intake               motor         7               
// flywheel             motor_group   19, 20          
// Optical              optical       10              
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

competition Competition;                                // A global instance of competition

void pre_auton(void) {
  vexcodeInit();                                        // Initializing Robot Configuration.
}

int detectColor(){
  int sHue = Optical.hue();

  if ( 330 <= sHue || sHue < 30){                       // Red
    return 1;
  }
  else if ( 210 <= sHue && sHue <= 270 ){               // Blue
    return 2;
  }
  else {                                                // ???
    return 0;
  }
}

void roller(int colour){
  Optical.setLight(ledState::on);                       // LED helps with color detection
  while (1) {
    if (detectColor() != colour){                       // If color is undesired retry
      intake.spinFor(forward, 60, degrees);
      Controller1.Screen.print(detectColor());
    }
    else {                                              // Else return
      Controller1.Screen.print(detectColor());
      break;
    }
  }
  Optical.setLight(ledState::off);
}

void shoot(){
  flywheel.spin(reverse);                               // Start flywheel
  wait(2, seconds);

  indexer.spin(reverse);                                // Start indexer
  wait(10, seconds);

  indexer.stop();                                       // Stop everything assuming the shot is finished
  flywheel.stop();
}


void autonomous(void) {
  const int robotPos = 2;                               // 1 = in front of roller;  2 = next to roller
  const int colour = 1;                                 // 1 = results in blue;     2 = results in red

  intake.setVelocity(20, percent);                      // Initial Config
  flywheel.setVelocity(50, percent); 
  indexer.setVelocity(100, percent);
  Drivetrain.setTurnVelocity(5,percent);

  if (robotPos == 1) {
    flywheel.setVelocity(50, percent);                  // Adjust for low shot

    Drivetrain.driveFor(reverse, 1, inches);            // Drive Up to Roller

    roller(colour);                                     // Turn the roller

    Drivetrain.driveFor(forward, 3, inches);            // Drive away from roller

    Drivetrain.turnFor(right, 35, degrees, false);      // Shoot preload
    wait(2, seconds);
    shoot();
  }

  if (robotPos == 2) {
    flywheel.setVelocity(80, percent);                  // Adjust for low shot

    Drivetrain.driveFor(reverse, 7, inches);            // Drive up to roller
    Drivetrain.turnFor(right, 38, degrees, false);
    wait(2.5, seconds);
    Drivetrain.setDriveVelocity(15, percent);
    Drivetrain.driveFor(reverse, 3.5, inches);

    roller(colour);                                     // Turn the roller

    Drivetrain.driveFor(forward, 3.5, inches, false);   // Drive away from roller
    wait(1, seconds);

    Drivetrain.turnFor(left, 50, degrees);              // Shoot preload
    shoot();
  }
}


void usercontrol(void) {
  while (1) {
    flywheel.setVelocity(100, percent);                 // Set motors to a high velocity
    indexer.setVelocity(90, percent);
    intake.setVelocity(100, percent); 

    flywheel.setStopping(brakeType::coast);             // Set the beakeType to coast (Makes buttons act like a toggle)
    indexer.setStopping(brakeType::coast);
    intake.setStopping(brakeType::coast);

    if (Controller1.ButtonLeft.pressing()){             // (<) : Stop Flywheel
      flywheel.stop();
    }

    else if (Controller1.ButtonRight.pressing()){       // (>) : Stop Indexer
      indexer.stop();
    }

    else if (Controller1.ButtonY.pressing()){           // (Y) : Stop Intake
      intake.stop();
    }

    wait(20, msec);                                     // Sleep the task for a short amount of time to prevent wasted resources.
  }
}


int main() {                                            // DON'T screw with this 
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);
  pre_auton();

  while (true) {
    wait(100, msec);
  }
}
