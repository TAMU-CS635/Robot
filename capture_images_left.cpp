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
    chdir("/home/netbook/Downloads/Capture_Images_Left");
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
        Mat frame;
        
        // If we couldn't grab a frame... quit
        if(!capture.read(frame))
            break;
        imshow("video", frame);
        const string file = to_string(n) + ".jpg";
        imwrite( file, frame );
        n++;
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
