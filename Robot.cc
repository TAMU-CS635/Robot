#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/ml/ml.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "Create.h"
#include <unistd.h>
#include <libplayerc/playerc.h>

using namespace cv;

float velocity = 0.1;
float old_area = 0;
// number of ir readings in a row to test for ramp
int maxRampCounter = 20;

IplImage* GetThresholdedImage(IplImage* img)
{
    // Convert the image into an HSV image
    IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
    cvCvtColor(img, imgHSV, CV_BGR2HSV);
    IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);
    cvInRangeS(imgHSV, cvScalar(20, 100, 100), cvScalar(30, 255, 255), imgThreshed);
    cvReleaseImage(&imgHSV);
    return imgThreshed;
}

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
        create.move(-velocity);
        usleep(10);
        if (ir.left < 600){
            create.motor_raw(velocity, 0.5);
            usleep(100000);
        }
        else if (ir.right < 600){
            create.motor_raw(velocity, -0.5);
            usleep(100000);
        }
    }
}

int main()
{
    //Initialize learning machine
    int img_area = 160*120;
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
    

    

    // The window we'll be using
    cvNamedWindow("thresh");

    int rampCounter = 0;
    int areaCounter = 0;

    // Will hold a frame captured from the camera
    IplImage* frame = 0;

    //set test id
     int testid = 0;

    // An infinite loop
    while(testid!=2)
         
    {
    capture = cvCaptureFromCAM(1);
    // Couldn't get a device? Throw an error and quit
    if(!capture)
    {
        printf("Could not initialize capturing...");
        return -1;
    }
    areaCounter=0;
    while(true){

        frame = cvQueryFrame(capture);
        // If we couldn't grab a frame... quit
        if(!frame){
            printf("No Frame \n");
            break;
        }

        // Holds the yellow thresholded image (yellow = white, rest = black)
        IplImage* imgYellowThresh = GetThresholdedImage(frame);
        
        //Smooth the binary image using Gaussian kernel
        cvSmooth(imgYellowThresh, imgYellowThresh, CV_GAUSSIAN,3,3);
        
        // Calculate the moments to estimate the position of the ball
        CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
        cvMoments(imgYellowThresh, moments, 1);
        
        // The actual moment values
        static double moment10 = 0;
        static double moment01 = 0;
        moment10 = cvGetSpatialMoment(moments, 1, 0);
        moment01 = cvGetSpatialMoment(moments, 0, 1);
        double area = 0;
        area = cvGetCentralMoment(moments, 0, 0);
        //printf("area %f\n", area);
        //printf("area counter %d \n",areaCounter);
        if(area > 5000 && area < 120000){
            // Holding the current position of the object
            static int posX = 0;
            static int posY = 0;
        
            posX = moment10/area;
            posY = moment01/area;

            static float angularSpeed = 0;
            angularSpeed = (float)(320 - posX) / (float)(1600);

            create.motor_raw(-velocity, angularSpeed);
            usleep(10);
            areaCounter = 0;
        }
        else if(area >= 120000){
            areaCounter++;
        }
        else{
            create.motor_raw(0,0.38);
            usleep(10);
            areaCounter = 0;
        }

        if (areaCounter > 3){
            cvReleaseImage(&imgYellowThresh);
            break;
        }

        cvShowImage("thresh", imgYellowThresh);
        
        // Wait for a keypress
        int c = cvWaitKey(10);
        if(c!=-1)
        {
            cvReleaseImage(&imgYellowThresh);
            // If pressed, break out of the loop
            break;
        }
        // Release the thresholded image+moments... we need no memory leaks.. please
        cvReleaseImage(&imgYellowThresh);
    
    }
    // We're done using the camera. Other applications can now use it
        
    create.motor_raw(0,0);
    usleep(10);

    int testidArray[5] = {0};

    chdir("/home/netbook/Desktop/Process_Data");
    for(int imageCounter = 0; imageCounter < 5; imageCounter++){
        printf("before getting image\n");
        // try to get an image
        frame = cvQueryFrame(capture);
        const string file = std::to_string(imageCounter) + ".jpg";
        cvSaveImage(file, frame);
        img_mat = imread(file,1); // I used 0 for greyscale
        printf("after getting image\n");
        
        // convert to gray scale
        cvtColor( img_mat, img_mat, CV_BGR2GRAY );
        //resize the image
        GaussianBlur(img_mat, img_mat, Size(7,7), 1.5, 1.5);
        Canny(img_mat, img_mat, 0, 30, 3);
        resize(img_mat, img_mat, Size(160, 120), 0, 0, INTER_LINEAR);
        
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
        printf("id 0\n");
        //get_to_mid_from_left(create);
    else
    if(testid==1)
        printf("id 1\n");
        //get_to_mid_from_left_mid(create);
    else
    if(testid==3)
        printf("id 3\n");
        //get_to_mid_from_right_mid(create);
    else
    if(testid==4)
        printf("id 4\n");
        //get_to_mid_from_right(create);
    
    cvReleaseCapture(&capture);
    }
    
    create.motor_raw(0,0);
    usleep(10);
    return 0;
}
