#ifndef DRIVEOVERRAMPBEHAVIOR_H_
#define DRIVEOVERRAMPBEHAVIOR_H_

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Create.h"
#include <unistd.h>
#include <time.h>

class DriveOverRampBehavior {
    private:
        Create create;
        float velocity;
        bool showWindow;
        int maxRampCounter;
        int initial_ir;

    public:
        DriveOverRampBehavior(Create create, bool showWindow, int initial_ir);

        void go();
};

#endif
