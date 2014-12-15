#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

using namespace cv;
using namespace std;

int main()
{
    SurfFeatureDetector detector(1000);
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
    Ptr<DescriptorExtractor> extractor = new SurfDescriptorExtractor();
    BOWImgDescriptorExtractor bowide( extractor, matcher );
    
    // load vocabulary data
    Mat vocabulary;
    FileStorage fs( "vocabulary.xml", FileStorage::READ);
    fs["vocabulary"] >> vocabulary;
    fs.release();
    if( vocabulary.empty()  ) return 1;
    
    bowide.setVocabulary( vocabulary );
    
    CvSVM svm_left; svm_left.load("learned_lib_left.xml");
    CvSVM svm_left_mid; svm_left_mid.load("learned_lib_left_mid.xml");
    CvSVM svm_mid; svm_mid.load("learned_lib_mid.xml");
    CvSVM svm_right_mid; svm_right_mid.load("learned_lib_right_mid.xml");
    CvSVM svm_right; svm_right.load("learned_lib_right.xml");
    
    chdir("/home/netbook/Desktop/Capture_Right_Mid");
    // Initialize capturing live feed from the camera device 1
    VideoCapture capture = VideoCapture(1);
    // Couldn't get a device? Throw an error and quit
    if(!capture.isOpened())
    {
        printf("Could not initialize capturing...");
        return -1;
    }
    int n = 0;
    // An infinite loop
    while(true)
    {
        // Will hold a frame captured from the camera
        Mat img;
        
        // If we couldn't grab a frame... quit
        if(!capture.read(frame))
            break;
        imshow("video", img);
        vector<KeyPoint> keypoints;
        detector.detect(img,keypoints);
        bowide.compute(img, keypoints, response_hist);
        
        vector<double> region_list;
        region_list.push_back(svm_left.predict(response_hist,true));
        region_list.push_back(svm_left_mid.predict(response_hist,true));
        region_list.push_back(svm_mid.predict(response_hist,true));
        region_list.push_back(svm_right_mid.predict(response_hist,true));
        region_list.push_back(svm_right.predict(response_hist,true));
        
        long int min_index = std::min_element(region_list.begin(), region_list.end()) - region_list.begin();
        
        if(min_index != 3){
            const string file = to_string(n) + ".jpg";
            imwrite( file, frame );
            n++;
        }
        
        // Wait for a keypress
        int c = cvWaitKey(10);
        if(c!=-1)
        {
            // If pressed, break out of the loop
            break;
        }
    }
    
    return 0;
}
