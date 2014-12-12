#include "SearchBehavior.h"
#include "MoveToCenterBehavior.h"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

int maxRampCounter = 20;

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
    int testid = 0;
    CvSVM svm;
    printf("loading learned_lib\n");
    //Load learned library
    svm.load("learned_lib");
    
    //Initialize IRobot Create
    Create create;
    create.lights(0, 0, 0);

    create.calibrate_ir();

    Create::ir_values ir = create.read_ir();

    // instantiate search behavior
    // boolean is for showing the window
    SearchBehavior searchBehavior(create, true);

    // instantiate move to center behavior
    MoveToCenterBehavior moveToCenterBehavior(create);

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
        // moveToCenterBehavior.drive_to_center(testid);
        printf("testid: %d\n", testid);
        break;
    }

    return 0;
}
