#ifndef SEARCHBEHAVIOR_H_
#define SEARCHBEHAVIOR_H_

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Create.h"
#include <unistd.h>

class SearchBehavior {
  private:
    Create create;

    float velocity;

    int minArea;
    int maxArea;

    bool showWindow;

  public:
    SearchBehavior(Create create, bool showWindow);

    void find();

    static IplImage* get_thresholded_image(IplImage* img);
    static float get_angular_speed(double moment10, double moment01, double area);

};
#endif
