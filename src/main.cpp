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

// A global instance of competition
competition Competition;

void pre_auton(void) {
  // Initializing Robot Configuration.
  vexcodeInit();                                
}

int detectColor(){
  int sHue = Optical.hue();

  if ( 330 <= sHue || sHue < 30){               // red
    return 1;
  }
  else if ( 210 <= sHue && sHue <= 270 ){       // blue
    return 2;
  }
  else {                                        // ???
    return 0;
  }
}

void roller(int colour){
  Optical.setLight(ledState::on);               // LED helps with color detection
  while (1) {
    if (detectColor() != colour){               // If color is undesired retry
      intake.spinFor(forward, 60, degrees);
      Controller1.Screen.print(detectColor());
    }
    else {                                      // Else return
      Controller1.Screen.print(detectColor());
      break;
    }
  }
  Optical.setLight(ledState::off);
}

void shoot(){
  // Start flywheel
  flywheel.spin(reverse);
  wait(2, seconds);

  // Start indexer
  indexer.spin(reverse);
  wait(10, seconds);

  // Stop everything assuming the shot is finished
  indexer.stop();
  flywheel.stop();
}


void autonomous(void) {
  const int rbposition = 2;                   // 1 = in front of roller;  2 = next to roller
  const int colour = 1;                       // 1 = results in blue;     2 = results in red

  // Initial Config
  intake.setVelocity(20, percent);
  flywheel.setVelocity(50, percent); 
  indexer.setVelocity(100, percent);
  Drivetrain.setTurnVelocity(5,percent);

  if (rbposition == 1) {
    // Adjust for low shot
    flywheel.setVelocity(50, percent); 

    // Drive Up to Roller
    Drivetrain.driveFor(reverse, 1, inches);

    // Turn the roller
    roller(colour);

    // Drive away from roller
    Drivetrain.driveFor(forward, 3, inches);

    // Shoot preload
    Drivetrain.turnFor(right, 35, degrees, false);
    wait(2, seconds);
    shoot();
  }

  if (rbposition == 2) {
    // Adjust for low shot
    flywheel.setVelocity(80, percent); 

    // Drive up to roller
    Drivetrain.driveFor(reverse, 7, inches);
    Drivetrain.turnFor(right, 38, degrees, false);
    wait(2.5, seconds);
    Drivetrain.setDriveVelocity(15, percent);
    Drivetrain.driveFor(reverse, 3.5, inches);

    // Turn the roller
    roller(colour);

    // Drive away from roller
    Drivetrain.driveFor(forward, 3.5, inches, false);

    // Shoot preload
    wait(1, seconds);
    Drivetrain.turnFor(left, 50, degrees);
    shoot();
  }
}


void usercontrol(void) {
  // User control code here, inside the loop
  while (1) {
    // START Flywheel
    if (Controller1.ButtonL1.pressing()){
      flywheel.spin(reverse,100,velocityUnits::pct);
      flywheel.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonL2.pressing()){
      flywheel.spin(forward,100,velocityUnits::pct);
      flywheel.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonLeft.pressing()){
      flywheel.stop(brakeType::coast);
    }
    // END Flywheel

    // START Indexer
    if (Controller1.ButtonUp.pressing()){
      indexer.spin(reverse,90,velocityUnits::pct);
      indexer.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonDown.pressing()){
      indexer.spin(forward,90,velocityUnits::pct);
      indexer.setStopping(brakeType::coast);
    }
  
    else if (Controller1.ButtonRight.pressing()){
      indexer.stop(brakeType::coast);
    }
    // END Indexer

    // START Intake    
    if (Controller1.ButtonB.pressing()){
      intake.spin(reverse,100,velocityUnits::pct);
      intake.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonX.pressing()){
      intake.spin(forward,100,velocityUnits::pct);
      intake.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonY.pressing()){
      intake.stop(brakeType::hold);
    }
    // END Intake

    wait(20, msec); // Sleep the task for a short amount of time to prevent wasted resources.
  }
}


// Set up the competition functions and callbacks.
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
