#include "MoveToCenterBehavior.h"

using namespace std;
int i = 0;

Mat MoveToCenterBehavior::GetThresholdedImage(Mat img)
{
    // Convert the image into an HSV image
    Mat imgHSV = Mat(img.size(), 8, 3);
    cvtColor(img, imgHSV, CV_BGR2HSV);
    Mat imgThreshed = Mat(img.size(), 8, 1);
    cv::inRange(imgHSV, Scalar(20, 100, 100), Scalar(30, 255, 255), imgThreshed);
    return imgThreshed;
}

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
    //frame = GetThresholdedImage(frame);
    const string file = "image" + std::to_string(i) + ".jpg";
    i++;
    imwrite(file, frame);
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
            usleep(500000);
            create.move(-1);
            usleep(2200000);
            create.motor_raw(0, 2);
            usleep(350000);
            create.move(-1);
            usleep(1200000);
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
            usleep(1500000);
            break;
        // right
        case 4:
            create.motor_raw(0, 2);
            usleep(500000);
            create.move(-1);
            usleep(2200000);
            create.motor_raw(0, -2);
            usleep(350000);
            create.move(-1);
            usleep(1200000);
            break;
    }
    create.motor_raw(0, 0);
    usleep(10);
}
