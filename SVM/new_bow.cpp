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

const int num_samples = 6000;
const int num_left_samples = 3404;
const int num_left_mid_samples = 2512;
const int num_mid_samples = 2500;
const int num_right_mid_samples = 2500;
const int num_right_samples = 2555;

const string left_dir = "/Users/khuongnguyen/Desktop/New_BOW/Capture_Images_Left";
const string left_mid_dir = "/Users/khuongnguyen/Desktop/New_BOW/Capture_Images_Left_Mid";
const string mid_dir = "/Users/khuongnguyen/Desktop/New_BOW/Capture_Images_Mid";
const string right_mid_dir = "/Users/khuongnguyen/Desktop/New_BOW/Capture_Images_Right_Mid";
const string right_dir = "/Users/khuongnguyen/Desktop/New_BOW/Capture_Images_Right";
const string dir = "/Users/khuongnguyen/Desktop/New_BOW";
const string mix_dir = "/Users/khuongnguyen/Desktop/New_BOW/Capture_Images";

void create_vocabulary(){
    //extract features
    Ptr<DescriptorExtractor> extractor = new SurfDescriptorExtractor();
    Mat training_descriptors(1,extractor->descriptorSize(),extractor->descriptorType());
    vector<KeyPoint> keypoints;
    SurfFeatureDetector detector(1000);
    chdir(mix_dir.c_str());
    for( int i = 0; i < num_samples; i++)
    {
        const string file = to_string(i) + ".jpg";
        Mat img = imread(file);
        detector.detect(img, keypoints);
        Mat descriptors;
        extractor->compute(img, keypoints, descriptors);
        training_descriptors.push_back(descriptors);
    }
    //build vocabulary
    BOWKMeansTrainer bowtrainer(1000);
    bowtrainer.add(training_descriptors);
    Mat vocabulary = bowtrainer.cluster();
    
    chdir(dir.c_str());
    FileStorage fs("vocabulary.xml", FileStorage::WRITE);
    fs << "vocabulary" << vocabulary;
    fs.release();
}

//map<string,CvSVM>  train_svm(){
void train_svm(){
    vector<KeyPoint> keypoints;
    vector<string> classes_names;
    Mat response_hist;
    Mat img;
    map<string,Mat> classes_training_data;
    
    SurfFeatureDetector detector(1000);
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
    Ptr<DescriptorExtractor> extractor = new SurfDescriptorExtractor();
    BOWImgDescriptorExtractor bowide( extractor, matcher );
    
    cout << "read vocabulary form file"<<endl;
    Mat vocabulary;
    FileStorage fs("vocabulary.xml", FileStorage::READ);
    fs["vocabulary"] >> vocabulary;
    fs.release();
    
    if( vocabulary.empty())
        cout << "vocabulary is empty" << endl;
    
    bowide.setVocabulary(vocabulary);
    
    //left
    chdir(left_dir.c_str());
    for( int i = 0; i < num_left_samples; i++)
    {
        const string file = to_string(i) + ".jpg";
        img = imread(file);
        detector.detect(img,keypoints);
        bowide.compute(img, keypoints, response_hist);
        
        if(classes_training_data.count("left") == 0) { //not yet created...
            classes_training_data["left"].create(0,response_hist.cols,response_hist.type());
            classes_names.push_back("left");
        }
        classes_training_data["left"].push_back(response_hist);
    }
    
    //left_mid
    chdir(left_mid_dir.c_str());
    for( int i = 0; i < num_left_mid_samples; i++)
    {
        const string file = to_string(i) + ".jpg";
        img = imread(file);
        detector.detect(img,keypoints);
        bowide.compute(img, keypoints, response_hist);
        
        if(classes_training_data.count("left_mid") == 0) { //not yet created...
            classes_training_data["left_mid"].create(0,response_hist.cols,response_hist.type());
            classes_names.push_back("left_mid");
        }
        classes_training_data["left_mid"].push_back(response_hist);
        
    }
    //mid
    chdir(mid_dir.c_str());
    for( int i = 0; i < num_mid_samples; i++)
    {
        const string file = to_string(i) + ".jpg";
        img = imread(file);
        detector.detect(img,keypoints);
        bowide.compute(img, keypoints, response_hist);
        
        if(classes_training_data.count("mid") == 0) { //not yet created...
            classes_training_data["mid"].create(0,response_hist.cols,response_hist.type());
            classes_names.push_back("mid");
        }
        classes_training_data["mid"].push_back(response_hist);
        
    }
    //right_mid
    chdir(right_mid_dir.c_str());
    for( int i = 0; i < num_right_mid_samples; i++)
    {
        const string file = to_string(i) + ".jpg";
        img = imread(file);
        detector.detect(img,keypoints);
        bowide.compute(img, keypoints, response_hist);
        
        if(classes_training_data.count("right_mid") == 0) { //not yet created...
            classes_training_data["right_mid"].create(0,response_hist.cols,response_hist.type());
            classes_names.push_back("right_mid");
        }
        classes_training_data["right_mid"].push_back(response_hist);
        
    }
    //right
    chdir(right_dir.c_str());
    for( int i = 0; i < num_right_samples; i++)
    {
        const string file = to_string(i) + ".jpg";
        img = imread(file);
        detector.detect(img,keypoints);
        bowide.compute(img, keypoints, response_hist);
        if(classes_training_data.count("right") == 0) { //not yet created...
            classes_training_data["right"].create(0,response_hist.cols,response_hist.type());
            classes_names.push_back("right");
        }
        classes_training_data["right"].push_back(response_hist);
    }
    chdir(dir.c_str());
    
    for (int i=0;i<classes_names.size();i++) {
        string class_ = classes_names[i];
        
        Mat samples(0,response_hist.cols,response_hist.type());
        Mat labels(0,1,CV_32FC1);
        
        //copy class samples and label
        cout << "adding " << classes_training_data[class_].rows << " positive" << endl;
        samples.push_back(classes_training_data[class_]);
        Mat class_label = Mat::ones(classes_training_data[class_].rows, 1, CV_32FC1);
        labels.push_back(class_label);
        
        //copy rest samples and label
        for (map<string,Mat>::iterator it1 = classes_training_data.begin(); it1 != classes_training_data.end(); ++it1) {
            string not_class_ = (*it1).first;
            if(not_class_.compare(class_)==0) continue; //skip class itself
            samples.push_back(classes_training_data[not_class_]);
            class_label = Mat::zeros(classes_training_data[not_class_].rows, 1, CV_32FC1);
            labels.push_back(class_label);
        }
        
        cout << "Train.." << endl;
        //        Mat samples_32f; samples.convertTo(samples_32f, CV_32F);
        if(samples.rows == 0) continue; //phantom class?!
        //        CvSVM classifier;
        //        classifier.train(samples_32f,labels);
        // svm parameters
        CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_EPS, 1000, FLT_EPSILON);
        CvSVMParams svm_param = CvSVMParams( CvSVM::C_SVC, CvSVM::RBF, 10.0, 8.0, 1.0, 10.0, 0.5, 0.1, NULL, criteria);
        
        // train svm
        SVM classifier;
        classifier.train(samples, labels, Mat(), Mat(), svm_param);
        //do something with the classifier, like saving it to file
        const string lib = "learned_lib_" + class_ + ".xml";
        classifier.save(lib.c_str());
    }
}

int main(){
    create_vocabulary();
    train_svm();
    return 0;
}

