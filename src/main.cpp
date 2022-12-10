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
// Drivetrain           drivetrain    1, 2, 4, 21, 5  
// indexer              motor         6               
// intake               motor         7               
// flywheel             motor_group   19, 20          
// Optical              optical       10              
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"
#include "vex_global.h"

using namespace vex;

competition Competition;                                // A global instance of competition

void pre_auton(void) {
  vexcodeInit();                                        // Initializing Robot Configuration.
}


int detectColor(){                                      // FUNCTION:  Detects the color on the underside of the roller
  int sHue = Optical.hue();

  if ( 330 <= sHue || sHue < 30){                       // Red, resulting int the roller being blue
    return 1;
  }
  else if ( 210 <= sHue && sHue <= 270 ){               // Blue, resulting int the roller being red
    return 2;
  }
  else {                                                // Unknown
    return 0;
  }
}


void roller(int colour){                                // FUNCTION:  Turns the roller to the desired color   INPUT: 1 (Blue) or 2 (Red) 
  Optical.setLightPower(50, pct);                         // Set LED to 100% Brightness
  Optical.setLight(ledState::on);                         // LED helps with color detection
  while (1) {
    if (detectColor() != colour){                         // If color is undesired retry
      intake.spinFor(forward, 60, degrees);
      Controller1.Screen.print(detectColor());
    }
    else {                                                // Else return
      Controller1.Screen.print(detectColor());
      break;
    }
  }
  intake.spinFor(reverse, 30, degrees);

  Optical.setLight(ledState::off);
}

void shoot(){                                           // FUNCTION:  Shoots the 2 preloaded disks 
  flywheel.spin(reverse);                                 // Start flywheel
  wait(3, seconds);                                       // Wait for it to spin up

  indexer.spinFor(reverse, 800, degrees);                 // Shoot 4 times (2x per disk) in case the indexer fails
  wait(2, seconds);
  indexer.spinFor(reverse, 800, degrees);
  wait(2, seconds);
  indexer.spinFor(reverse, 800, degrees);
  wait(2, seconds);
  indexer.spinFor(reverse, 800, degrees);
  wait(2, seconds);
 
  indexer.stop();                                         // Stop everything assuming the shot is finished
  flywheel.stop();
}


// void expansion(){                                    // FUNCTION:  Triggers the expansion    NOTE: Unused until the parts arrive
//   pneum.set(true);
//   wait(1.5, seconds);
//   pneum.set(false);
// }


void autonomous(void) {                                 // FUNCTION:  The 15s auto before the match
  const int robotPos = 2;                                 // 1 = NEXT TO roller;      2 = IN FRONT of roller;
  const int colour = 2;                                   // 1 = Results in BLUE;     2 = Results in RED;
  const int partnerAuto = 1;                              // 0 = Partner has NO auto; 1 = Partner has auto;

  intake.setVelocity(20, percent);                        // Initial Config
  flywheel.setVelocity(50, percent); 
  indexer.setVelocity(100, percent);
  Drivetrain.setTurnVelocity(5,percent);

  if (robotPos == 1){                                     // PSOITION:  NEXT TO
    flywheel.setVelocity(89, percent);                      // Config
    Drivetrain.setTurnVelocity(5, percent);

    Drivetrain.driveFor(reverse, 8, inches);                // Drive up to roller
    Drivetrain.turnToRotation(90 - 4.5, degrees);           // Error correction of -4.5
    Drivetrain.setDriveVelocity(15, percent);
    Drivetrain.driveFor(reverse, 3.5, inches);

    roller(colour);                                         // Turn the roller

    Drivetrain.driveFor(forward, 1.5, inches);              // Drive away from roller

    Drivetrain.turnToRotation(87 - 4, degrees);             // Turn towards hoop; error correction of -4 degrees
    shoot();                                                // Shoot preload

    if (partnerAuto == 0) {                                 // TODO: If the other team has no auto
      // Drivetrain.turnToRotation(45 + 10, degrees);
      // Drivetrain.driveFor(30, inches);
    }

  }
  else if (robotPos == 2) {                               // POSITION:  IN FRONT
    flywheel.setVelocity(86, percent);                      // Config

    Drivetrain.driveFor(reverse, 1, inches);                // Drive Up to Roller

    roller(colour);                                         // Turn the roller

    Drivetrain.driveFor(forward, 2, inches);                // Drive away from roller

    Drivetrain.turnToHeading(343, degrees);                 // Turn towards hoop
    shoot();                                                // Shoot preload

    if (partnerAuto == 0) {                                 // TODO: If the other team has no auto
    }
  }
}


void usercontrol(void) {                                // FUNCTION:  Usercontrol
  while (1) {

    // START Flywheel
    if (Controller1.ButtonL1.pressing()){                 // (L1) : [Flywheel] High Shot
      flywheel.spin(reverse,85,velocityUnits::pct);
      flywheel.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonR2.pressing()){            // (R2) : [Flywheel] Medium Shot
      flywheel.spin(reverse,75,velocityUnits::pct);
      flywheel.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonA.pressing()){             // (A)  : [Flywheel] Slow Shot
      flywheel.spin(reverse,65,velocityUnits::pct);
      flywheel.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonL2.pressing()){            // (L2) : [Flywheel] Back to storage
      flywheel.spin(forward,85,velocityUnits::pct);
      flywheel.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonLeft.pressing()){          // (<)  : [Flywheel] Stop
      flywheel.stop(brakeType::coast);
    }
    // END Flywheel

    // START Indexer
    if (Controller1.ButtonUp.pressing()){                 // (^)  : [Indexer] Disk to Flywheel
      indexer.spin(reverse,90,velocityUnits::pct);
      indexer.setStopping(brakeType::coast);
    }

    else if(Controller1.ButtonR1.pressing()){             // (R1) : [Indexer] (1) Disk to Flywheel
      indexer.setVelocity(100,percent);
      indexer.spinFor(reverse, 800, degrees, false);
    }

    else if (Controller1.ButtonDown.pressing()){          // (V)  : [Indexer] Disk to Storage
      indexer.spin(forward,90,velocityUnits::pct);
      indexer.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonRight.pressing()){         // (>)  : [Indexer] Stop
      indexer.stop(brakeType::coast);
    }
    // END Indexer

    // START Intake
    if (Controller1.ButtonB.pressing()){                  // (B)  : [Intake] Disk to Field
      intake.spin(reverse,100,velocityUnits::pct);
      intake.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonX.pressing()){             // (X)  : [Intake] Disk to Storage
      intake.spin(forward,100,velocityUnits::pct);
      intake.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonY.pressing()){             // (Y)  : [Intake] Stop
      intake.stop(brakeType::hold);
    }
    // END Intake

    wait(20, msec);                                       // Sleep the task for a short amount of time to prevent wasted resources.
  }
}


int main() {                                            // FUNCTION:  Main     NOTE: DON'T screw with this
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);
  pre_auton();
 
  while (true) {
    wait(100, msec);
  }
}