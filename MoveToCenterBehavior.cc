#include "MoveToCenterBehavior.h"

using namespace std;

MoveToCenterBehavior::MoveToCenterBehavior(Create create, Mat vocabulary){
    this -> create = create;
    this -> imgWidth = 160;
    this -> imgHeight = 120;
    this -> imgArea = imgWidth*imgHeight;
    this -> vocabulary = vocabulary;
}

Mat MoveToCenterBehavior::get_test_matrix() {
   
    SurfFeatureDetector detector(1000);
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
    Ptr<DescriptorExtractor> extractor = new SurfDescriptorExtractor();
    BOWImgDescriptorExtractor bowide( extractor, matcher );
    
    bowide.setVocabulary( this -> vocabulary );
    
    Mat frame, response_hist;
//    Mat imgMat;

    VideoCapture capture = VideoCapture(1);

    // get the image
    while(!capture.read(frame))
        capture.read(frame);

    vector<KeyPoint> keypoints;
    detector.detect(frame,keypoints);
    bowide.compute(frame, keypoints, response_hist);

    return response_hist;
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
