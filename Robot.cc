#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/ml/ml.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "Create.h"
#include <unistd.h>
#include <libplayerc/playerc.h>
#include "SearchBehavior.h"

using namespace cv;

int maxRampCounter = 20;

void get_to_mid_from_left(Create create){
    create.motor_raw(0,-2);
    usleep(400000);
    create.move(-1);
    usleep(1500000);
    create.motor_raw(0,0);
    usleep(10);

    create.motor_raw(0,2);
    usleep(200000);
    create.move(-1);
    usleep(1500000);
    create.motor_raw(0,0);
    usleep(10);
}

void get_to_mid_from_left_mid(Create create){
    create.motor_raw(0,-2);
    usleep(800000);
    create.move(-1);
    usleep(3000000);
    create.motor_raw(0,0);
    usleep(10);
    
}

void get_to_mid_from_right(Create create){
    create.motor_raw(0,2);
    usleep(400000);
    create.move(-1);
    usleep(1500000);
    create.motor_raw(0,0);
    usleep(10);

    create.motor_raw(0,-2);
    usleep(200000);
    create.move(-1);
    usleep(1500000);
    create.motor_raw(0,0);
    usleep(10);
}

void get_to_mid_from_right_mid(Create create){
create.motor_raw(0,2);
    usleep(800000);
    create.move(-1);
    usleep(3000000);
    create.motor_raw(0,0);
    usleep(10);
    
}

void run_over_ramp(Create create, Create::ir_values ir, int rampCounter){
    // driving over the ramp
    printf("Go over ramp activated");
    rampCounter = 0;
    while(true){
        ir = create.read_ir();
        if (ir.fleft < 1150 || ir.fright < 1150){
            rampCounter++;
        } else {
            rampCounter = 0;
        }
        
        if (rampCounter > maxRampCounter){
            create.motor_raw(0,0);
            usleep(10);
            break;
        }
        create.move(-0.1);
        usleep(10);
        if (ir.left < 600){
            create.motor_raw(-0.1, 0.5);
            usleep(100000);
        }
        else if (ir.right < 600){
            create.motor_raw(-0.1, -0.5);
            usleep(100000);
        }
    }
}

int main()
{
    //Initialize learning machine
    int img_area = 320*240;
    Mat img_mat;
    int ii = 0;
    CvSVM svm;
    printf("loading learned_lib\n");
    //Load learned library
    svm.load("learned_lib");
    
    //Initialize IRobot Create
    Create create;
    create.lights(0, 0, 0);

    create.calibrate_ir();

    Create::ir_values ir = create.read_ir();

    // Initialize capturing live feed from the camera
    CvCapture* capture = 0;

    // Will hold a frame captured from the camera
    IplImage* frame = 0;

    //set test id
     int testid = 0;

    // instantiate search behavior
    SearchBehavior searchBehavior(create, true);

    // An infinite loop
    while(testid!=2)
    { 
        searchBehavior.find();

        int testidArray[5] = {0};
        // Initialize capturing live feed from the camera device 1
        VideoCapture cap = VideoCapture(1);
        // Couldn't get a device? Throw an error and quit
        if(!cap.isOpened())
        {
            printf("Could not initialize capturing...");
            return -1;
        }
        
        Mat fram;
        for(int imageCounter = 0; imageCounter < 5; imageCounter++){
            printf("before getting image\n");
            // try to get an image
            while(!cap.read(fram))
                cap.read(fram);

            printf("after getting image\n");
            // convert to gray scale
            cvtColor( fram, img_mat, CV_BGR2GRAY );
            //resize the image
            cv::resize(img_mat, img_mat, cv::Size(320,240), 0, 0, INTER_LINEAR);
            // perform SVM test
            Mat test(1,img_area,CV_32FC1);
            
            ii = 0; // Current column in training_mat
            for (int i = 0; i<img_mat.rows; i++) {
                for (int j = 0; j < img_mat.cols; j++) {
                    test.at<float>(0,ii++) = img_mat.at<uchar>(i,j);
                }
            }
            testidArray[imageCounter] = svm.predict(test);
            printf("testid: %d \n", testid);

            // move a little
            create.move(-0.2);
            usleep(300000);
            create.motor_raw(0,0);
            usleep(10);
        }
        // release image so no memory leak
        //cvReleaseImage(&frame);

        // driving over the ramp
        //run_over_ramp(create, ir, rampCounter);
        int mostOftenArray[5] = {0};

        for(int i = 0; i < 5; i++){
            if (testidArray[i] == i)
                mostOftenArray[i]++;
        }
        int largest = 0;
        int testid;

        for(int i = 0; i < 5; i++){
            if (mostOftenArray[i] > largest)
                testid = i;
        }

        if(testid==0)
            get_to_mid_from_left(create);
        else
        if(testid==1)
            get_to_mid_from_left_mid(create);
        else
        if(testid==3)
            get_to_mid_from_right_mid(create);
        else
        if(testid==4)
            get_to_mid_from_right(create);
    
        //cvReleaseCapture(&capture);
    }
    
    create.motor_raw(0,0);
    usleep(10);
    return 0;
}
