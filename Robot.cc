#include "SearchBehavior.h"
#include "MoveToCenterBehavior.h"
#include "DriveOverRampBehavior.h"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

int main()
{
    int testid = 0;

    CvSVM svm;
    printf("loading learned_lib\n");
    //Load learned library
    svm.load("learned_lib");

    //Initialize IRobot Create
    Create create;
    create.lights(0, 0, 0);

    create.calibrate_ir();

    // instantiate search behavior
    // boolean is for showing the window
    SearchBehavior searchBehavior(create, true);

    // instantiate move to center behavior
    MoveToCenterBehavior moveToCenterBehavior(create);

    DriveOverRampBehavior driveOverRampBehavior(create, true);

    // matrix for the test
    Mat test;

    // An infinite loop
    while(testid!=2)

    { 
	    // find the ramp and drive up to it
        searchBehavior.find();
	    // get the test matrix
	    test = moveToCenterBehavior.get_test_matrix();
	    // predict the testid
	    testid = svm.predict(test);
	    // move based on test id
        moveToCenterBehavior.drive_to_center(testid);
        printf("testid: %d\n", testid);
    }
    driveOverRampBehavior.go();

    return 0;
}
