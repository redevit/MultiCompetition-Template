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
#include "vex_global.h"

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
  Optical.setLightPower(100, pct);                      // Set LED to 100% Brightness
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
  intake.spinFor(reverse, 30, degrees);

  Optical.setLight(ledState::off);
}

void shoot(){
  flywheel.spin(reverse);                               // Start flywheel
  wait(4, seconds);

  indexer.spinFor(reverse, 800, degrees);
  wait(2, seconds);
  indexer.spinFor(reverse, 800, degrees);
  wait(2, seconds);
  

  indexer.stop();                                       // Stop everything assuming the shot is finished
  flywheel.stop();
}


void autonomous(void) {
  const int robotPos = 1;                               // 1 = in front of roller;  2 = next to roller
  const int colour = 2;                                 // 1 = results in blue;     2 = results in red
  const int partnerAuto = 0;                            // 0 = Partner has no auto; 1 = Partner has auto

  intake.setVelocity(20, percent);                      // Initial Config
  flywheel.setVelocity(50, percent); 
  indexer.setVelocity(100, percent);
  Drivetrain.setTurnVelocity(5,percent);

  if (robotPos == 1) {
    flywheel.setVelocity(100, percent);                  // Adjust for low shot

    Drivetrain.driveFor(reverse, 1, inches);            // Drive Up to Roller

    roller(colour);                                     // Turn the roller

    Drivetrain.driveFor(forward, 2, inches);            // Drive away from roller

    Drivetrain.turnFor(left, 19, degrees, false);       // Shoot preload
    wait(2, seconds);
    shoot();

    if (partnerAuto == 0) {
      return;
    } 
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

    if (partnerAuto == 0){
      return; 
    }
  }
}


void usercontrol(void) {
  while (1) {
    // START Flywheel
    if (Controller1.ButtonL1.pressing()){               // (L1) : [Flywheel] Shoot
      flywheel.spin(reverse,100,velocityUnits::pct);
      flywheel.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonL2.pressing()){          // (L2) : [Flywheel] Back to storage
      flywheel.spin(forward,100,velocityUnits::pct);
      flywheel.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonLeft.pressing()){        // (<)  : [Flywheel] Stop
      flywheel.stop(brakeType::coast);
    }
    // END Flywheel

    // START Indexer
    if (Controller1.ButtonUp.pressing()){               // (^)  : [Indexer] Disk to Flywheel
      indexer.spin(reverse,90,velocityUnits::pct);
      indexer.setStopping(brakeType::coast);
    }

    else if(Controller1.ButtonR1.pressing()){           // (R1) : [Indexer] (1) Disk to Flywheel
      indexer.setVelocity(100,percent);
      indexer.spinFor(reverse, 800, degrees);
    }

    else if (Controller1.ButtonDown.pressing()){        // (V)  : [Indexer] Disk to Storage
      indexer.spin(forward,90,velocityUnits::pct);
      indexer.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonRight.pressing()){       // (>)  : [Indexer] Stop
      indexer.stop(brakeType::coast);
    }
    // END Indexer

    // START Intake    
    if (Controller1.ButtonB.pressing()){                // (B)  : [Intake] Disk to Field
      intake.spin(reverse,100,velocityUnits::pct);
      intake.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonX.pressing()){           // (X)  : [Intake] Disk to Storage
      intake.spin(forward,100,velocityUnits::pct);
      intake.setStopping(brakeType::coast);
    }

    else if (Controller1.ButtonY.pressing()){           // (Y)  : [Intake] Stop
      intake.stop(brakeType::hold);
    }
    // END Intake

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
