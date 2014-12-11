#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Create.h"

class SearchBehavior {
  private:
    Create create;

    float velocity = -0.1;

    int minArea = 5000;
    int maxArea = 120000;

    bool showWindow;

  public:
    SearchBehavior();

    SearchBehavior(Create create);

    void find();

    static IplImage* GetThresholdedImage(IplImage* img);
    static float get_angular_speed(double moment10, double moment01, double area);

}
