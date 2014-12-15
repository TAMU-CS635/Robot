#include "SearchBehavior.h"


SearchBehavior::SearchBehavior(Create *create, bool showWindow) {
  this -> create = create;
  this -> showWindow = showWindow;
  this -> velocity = -0.1;
  this -> minArea = 1900;
  this -> maxArea = 113000;
  //this -> maxArea = 115000;
}

IplImage* SearchBehavior::get_thresholded_image(IplImage* img) {
    // Convert the image into an HSV image
    IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
    cvCvtColor(img, imgHSV, CV_BGR2HSV);
    IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);
    //cvInRangeS(imgHSV, cvScalar(30, 125, 125), cvScalar(35, 255, 255), imgThreshed);
    cvInRangeS(imgHSV, cvScalar(30, 125, 150), cvScalar(33, 255, 255), imgThreshed);
    cvReleaseImage(&imgHSV);
    return imgThreshed;
}

float SearchBehavior::get_angular_speed(double moment10, double moment01, double area) {
  int posX = moment10/area;
  int posY = moment01/area;

  float angularSpeed = (float)(320 - posX) / 1600.0;

  return angularSpeed;
}



void SearchBehavior::find() {
  double moment10 = 0;
  double moment01 = 0;
  double area = 0;

  float angularSpeed = 0;

  int areaCounter = 0;

  // variable to hold the fram
  IplImage* frame = 0;
  // initialize the camera
  CvCapture* capture = 0;
  capture = cvCaptureFromCAM(2);

  // create a window
  if(this -> showWindow)
    cvNamedWindow("thresh");

  while(areaCounter < 25){
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

    if(area > this -> minArea){
      angularSpeed = SearchBehavior::get_angular_speed(moment10, moment01, area);
      create->motor_raw(0, angularSpeed*2);
      usleep(10);
      areaCounter++;
    } else {
      create->motor_raw(0, 0.38);
      usleep(10);
    }

    // display blob image in window
    if(this -> showWindow){
      cvShowImage("thresh", imgYellowThresh);

      // Wait for a keypress
      // just gives time for window to show
      cvWaitKey(10);
    }
  }
  create->motor_raw(0, 0);
  usleep(10);

  areaCounter = 0;

  // loop until area is greater than maxArea 3 times in a row
  while(areaCounter < 4){
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
    std::cout << "Area: " << area << std::endl;

    // calculate angular speed
    angularSpeed = SearchBehavior::get_angular_speed(moment10, moment01, area);

    // decide action based on area of the blob
    if(area > this -> minArea && area < this -> maxArea){
      // put motor command
      create->motor_raw(this -> velocity, angularSpeed);
      usleep(10);
      // reset area counter
      areaCounter = 0;
    } else if(area >= this -> maxArea && area < (this -> maxArea + 20000)) {
      // area is above the threshold
      areaCounter++;
    } else if(area >= (this -> maxArea + 20000)) {
      create->motor_raw(-this -> velocity, angularSpeed);
      usleep(10);
    } else {
      // spin
      create->motor_raw(0, 0.38);
      usleep(10);
      // reset area counter
      areaCounter = 0;
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
  // stop
  create->motor_raw(0,0);
  usleep(10);

  // release the camera
  cvReleaseCapture(&capture);
}
