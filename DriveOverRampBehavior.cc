#include "DriveOverRampBehavior.h"

DriveOverRampBehavior::DriveOverRampBehavior(Create create, bool showWindow, int initial_ir) {
  this -> create = create;
  this -> showWindow = showWindow;
  this -> velocity = -0.15;
  this -> maxRampCounter = 15;
  this -> initial_ir = initial_ir;
}

void DriveOverRampBehavior::go() {
  Create::ir_values ir;

  // drive forward for 4 seconds
  create.move(velocity);
  usleep(4000000);

  // stop
  this -> create.motor_raw(0,0);
  usleep(10);

  // turn 180
  this -> create.motor_raw(0,-2.0);
  usleep(1100000);

  // stop
  this -> create.motor_raw(0,0);
  usleep(10);

  time_t startTime = time(NULL);
  time_t currentTime = 0;
  double differenceTime = 0;

  // on the ramp
  while(differenceTime < 12) {
    ir = create.read_ir();
    if(ir.left < 200) {
      // turn right
      create.motor_raw(-this -> velocity, -1.0);
      usleep(10);
    } else if(ir.right < 200) {
      // turn left
      create.motor_raw(-this -> velocity, 1.0);
      usleep(10);
    } else {
      create.move(-this -> velocity);
      usleep(10);
    }
    currentTime = time(NULL);
    differenceTime = difftime(currentTime,startTime);
    std::cout << "differenceTime: " << differenceTime << std::endl;
  }
  // stop
  this -> create.motor_raw(0,0);
  usleep(100);
}
