#ifndef MOVETOCENTERBEHAVIOR_H_
#define MOVETOCENTERBEHAVIOR_H_

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/ml/ml.hpp>
#include "Create.h"
#include <unistd.h>

class MoveToCenterBehavior {
    private:
        Create create;
        int imgWidth;
        int imgHeight;
        int imgArea;

    public:
        MoveToCenterBehavior(Create create);

        cv::Mat get_test_matrix();
        void drive_to_center(int testid);
};
#endif
