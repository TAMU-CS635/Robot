#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <opencv2/ml/ml.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

using namespace cv;
using namespace std;

const int num_test_left_samples = 1000;
const int num_test_left_mid_samples = 1000;
const int num_test_mid_samples = 1000;
const int num_test_right_mid_samples = 1000;
const int num_test_right_samples = 1000;

const string test_left_dir = "/Users/khuongnguyen/Desktop/New_BOW/Test/test_left";
const string test_left_mid_dir = "/Users/khuongnguyen/Desktop/New_BOW/Test/test_left_mid";
const string test_mid_dir = "/Users/khuongnguyen/Desktop/New_BOW/Test/test_mid";
const string test_right_mid_dir = "/Users/khuongnguyen/Desktop/New_BOW/Test/test_right_mid";
const string test_right_dir = "/Users/khuongnguyen/Desktop/New_BOW/Test/test_right";

int main(){
    vector<string> dir_list;
    dir_list.push_back(test_left_dir);
    dir_list.push_back(test_left_mid_dir);
    dir_list.push_back(test_mid_dir);
    dir_list.push_back(test_right_mid_dir);
    dir_list.push_back(test_right_dir);
    
    vector<int> num_test_samples_list;
    num_test_samples_list.push_back(num_test_left_samples);
    num_test_samples_list.push_back(num_test_left_mid_samples);
    num_test_samples_list.push_back(num_test_mid_samples);
    num_test_samples_list.push_back(num_test_right_mid_samples);
    num_test_samples_list.push_back(num_test_right_samples);
    
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
    
    for (int n = 0; n < dir_list.size(); n++) {
        chdir(dir_list[n].c_str());
        cout << "TEST " << dir_list[n] << " REGION" << endl;
        for(int i = 0; i < num_test_samples_list[n]; i++) {
            const string file = to_string(i) + ".jpg";
            Mat img = imread(file),response_hist;
            
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
            
            cout << "Image " << i << ": ";
            switch (min_index) {
                case 0:
                    cout << "On the left. ";
                    break;
                case 1:
                    cout << "On the left mid. ";
                    break;
                case 2:
                    cout << "Head on. ";
                    break;
                case 3:
                    cout << "On the right mid. ";
                    break;
                case 4:
                    cout << "On the right. ";
                    break;
                default:
                    break;
            }
            cout << "Region: " << min_index << endl;
        }
    }
    return 0;
}