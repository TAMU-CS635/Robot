#include "DriveOverRampBehavior.h"

DriveOverRampBehavior::DriveOverRampBehavior(Create *create) {
  this -> create = create;
  this -> velocity = -0.15;
}

void DriveOverRampBehavior::go() {
  Create::ir_values ir;

  // drive forward for 4 seconds
  create->move(velocity);
  usleep(4000000);

  // stop
  create->motor_raw(0,0);
  usleep(10);

  // turn 180
  create->motor_raw(0,-2.0);
  usleep(1050000);

  // stop
  create->motor_raw(0,0);
  usleep(10);

  time_t startTime = time(NULL);
  time_t currentTime = 0;
  double differenceTime = 0;

  // on the ramp
  while(differenceTime < 10.5) {
    ir = create->read_ir();
    if(ir.left < 200) {
      // turn right
      create->motor_raw(0, -1.0);
      usleep(10);
    } else if(ir.right < 200) {
      // turn left
      create->motor_raw(0, 1.0);
      usleep(10);
    } else {
      create->motor_raw(-this -> velocity, 0);
      usleep(10);
    }
    currentTime = time(NULL);
    differenceTime = difftime(currentTime,startTime);
    std::cout << "differenceTime: " << differenceTime << std::endl;
  }
  // stop
  create->motor_raw(0,0);
  usleep(1000000);
  
}
