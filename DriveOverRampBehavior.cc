#include "DriveOverRampBehavior.h"
#include "SearchBehavior.h"

DriveOverRampBehavior::DriveOverRampBehavior(Create create, bool showWindow) {
  this -> create = create;
  this -> showWindow = showWindow;
  this -> velocity = -0.1;
  this -> maxRampCounter = 20;
}

void DriveOverRampBehavior::go() {
  double moment10 = 0;
  double moment01 = 0;
  double area = 0;

  bool overRamp = false;

  float angularSpeed = 0;

  int rampCounter = 0;

  Create::ir_values ir;

  // variable to hold the fram
  IplImage* frame = 0;
  // initialize the camera
  CvCapture* capture = 0;
  capture = cvCaptureFromCAM(1);

  // create a window
  if(this -> showWindow)
    cvNamedWindow("thresh");

  // loop until area is greater than maxArea 3 times in a row
  while(rampCounter < this -> maxRampCounter){
    frame = cvQueryFrame(capture);

    // Holds the yellow thresholded image (yellow = white, rest = black)
    IplImage* imgYellowThresh = SearchBehavior::get_thresholded_image(frame);

    //Smooth the binary image using Gaussian kernel
    cvSmooth(imgYellowThresh, imgYellowThresh, CV_GAUSSIAN,3,3);

    // Calculate the moments to estimate the position of the ball
    CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
    cvMoments(imgYellowThresh, moments, 1);

    moment10 = cvGetSpatialMoment(moments, 1, 0);
    moment01 = cvGetSpatialMoment(moments, 0, 1);

    area = cvGetSpatialMoment(moments, 0, 0);

    // calculate angular speed
    angularSpeed = SearchBehavior::get_angular_speed(moment10, moment01, area);
    // put motor command
    this -> create.motor_raw(this -> velocity, angularSpeed);
    usleep(10);

    ir = create.read_ir();
    if (ir.fleft > 1300 || ir.fright > 1300){
        rampCounter++;
    } else {
        rampCounter = 0;
    }

    // display blob image in window
    if(this -> showWindow){
      cvShowImage("thresh", imgYellowThresh);

      // Wait for a keypress
      // just gives time for window to show
      cvWaitKey(10);
    }

    // avoid memory leaks
    cvReleaseImage(&imgYellowThresh);
  }
  rampCounter = 0;
  // stop
  this -> create.motor_raw(0,0);
  usleep(10);

  // turn 180
  this -> create.motor_raw(0,-2.0);
  usleep(1100000);

  // stop
  this -> create.motor_raw(0,0);
  usleep(10);

  // on the ramp
  while(rampCounter < (this -> maxRampCounter + 10)) {
    ir = create.read_ir();
    if(ir.left < 200) {
      // turn right
      create.motor_raw(-this -> velocity, -2.0);
      usleep(10);
    } else if(ir.right < 200) {
      // turn left
      create.motor_raw(-this -> velocity, 2.0);
      usleep(10);
    } else {
      create.move(-this -> velocity);
      usleep(10);
      if(ir.fleft < 1150 || ir.fright < 1150){
        rampCounter++;
      } else {
        rampCounter = 0;
      }
    }
  }
  // stop
  this -> create.motor_raw(0,0);
  usleep(10);

  // release the camera
  cvReleaseCapture(&capture);
}
