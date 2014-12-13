#ifndef DRIVEOVERRAMPBEHAVIOR_H_
#define DRIVEOVERRAMPBEHAVIOR_H_

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Create.h"
#include <unistd.h>

class DriveOverRampBehavior {
    private:
        Create create;
        float velocity;
        bool showWindow;
        int maxRampCounter;

    public:
        DriveOverRampBehavior(Create create, bool showWindow);

        void go();
};

#endif
