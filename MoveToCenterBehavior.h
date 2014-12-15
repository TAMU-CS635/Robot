#ifndef MOVETOCENTERBEHAVIOR_H_
#define MOVETOCENTERBEHAVIOR_H_

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <opencv2/nonfree/features2d.hpp>
#include "opencv2/features2d/features2d.hpp"
#include <opencv2/ml/ml.hpp>
#include "Create.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace cv;

class MoveToCenterBehavior {
    private:
        Create *create;
        int imgWidth;
        int imgHeight;
        int imgArea;
        Mat vocabulary;

    public:
        MoveToCenterBehavior(Create *create, Mat vocabulary);
        
        cv::Mat GetThresholdedImage(Mat img);
        cv::Mat get_test_matrix();
        void drive_to_center(int testid);
};
#endif
