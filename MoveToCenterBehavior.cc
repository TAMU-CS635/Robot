#include "MoveToCenterBehavior.h"

using namespace cv;

MoveToCenterBehavior::MoveToCenterBehavior(Create create){
    this -> create = create;
    this -> imgWidth = 160;
    this -> imgHeight = 120;
    this -> imgArea = imgWidth*imgHeight;
}

Mat MoveToCenterBehavior::get_test_matrix() {
    Mat frame;
    Mat imgMat;

    VideoCapture capture = VideoCapture(1);

    // get the image
    while(!capture.read(frame))
        capture.read(frame);
  imgMat=imwrite("left.jpg",frame); 
    cvtColor(frame, imgMat, CV_BGR2GRAY);

    cv::resize(imgMat, imgMat, cv::Size(this -> imgWidth, this -> imgHeight), 0, 0, INTER_LINEAR);

    Mat test(1, this -> imgArea, CV_32FC1);

    int ii = 0;
    for(int i = 0; i < imgMat.rows; i++){
        for(int j = 0; j < imgMat.cols; j++){
            test.at<float>(0, ii++) = (imgMat.at<uchar>(i, j)/255.0);
        }
    }

    return test;
}

void MoveToCenterBehavior::drive_to_center(int testid){
    switch(testid){
        // left
        case 0:
            create.motor_raw(0,-2);
            usleep(400000);
            create.move(-1);
            usleep(1500000);
            create.motor_raw(0, 2);
            usleep(200000);
            create.move(-1);
            usleep(1500000);
            break;
        // left - mid
        case 1:
            create.motor_raw(0,-2);
            usleep(500000);
            create.move(-1);
            usleep(1500000);
            break;
        // right - mid
        case 3:
            create.motor_raw(0, 2);
            usleep(500000);
            create.move(-1);
            usleep(1700000);
            break;
        // right
        case 4:
            create.motor_raw(0, 2);
            usleep(400000);
            create.move(-1);
            usleep(1500000);
            create.motor_raw(0, -2);
            usleep(200000);
            create.move(-1);
            usleep(1500000);
            break;
    }
    create.motor_raw(0, 0);
    usleep(10);
}
