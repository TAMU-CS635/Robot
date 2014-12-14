#include "SearchBehavior.h"
#include "MoveToCenterBehavior.h"
#include "DriveOverRampBehavior.h"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

int main()
{
//    int testid = 0;
//
//    CvSVM svm;
//    printf("loading learned_lib\n");
//    //Load learned library
//    svm.load("learned_lib");
    
    int region_id;
    // load vocabulary data
    Mat vocabulary;
    FileStorage fs( "vocabulary.xml", FileStorage::READ);
    fs["vocabulary"] >> vocabulary;
    fs.release();
    if( vocabulary.empty()  ) return 1;

    //Load learned library
    CvSVM svm_left; svm_left.load("learned_lib_left.xml");
    CvSVM svm_left_mid; svm_left_mid.load("learned_lib_left_mid.xml");
    CvSVM svm_mid; svm_mid.load("learned_lib_mid.xml");
    CvSVM svm_right_mid; svm_right_mid.load("learned_lib_right_mid.xml");
    CvSVM svm_right; svm_right.load("learned_lib_right.xml");

    //Initialize IRobot Create
    Create create;
    create.lights(0, 0, 0);

    create.calibrate_ir();

    // instantiate search behavior
    // boolean is for showing the window
    SearchBehavior searchBehavior(create, true);

    // instantiate move to center behavior
    MoveToCenterBehavior moveToCenterBehavior(create, vocabulary);

    DriveOverRampBehavior driveOverRampBehavior(create, true);
    
    // matrix for the test
    Mat response_hist;

    // An infinite loop
    while(region_id!=2)

    { 
	    // find the ramp and drive up to it
        searchBehavior.find();
	    // get the test matrix
	    response_hist = moveToCenterBehavior.get_test_matrix();
        
        //compare list to find id region
        vector<double> region_list;
        
        //predict region
        region_list.push_back(svm_left.predict(response_hist,true));
        region_list.push_back(svm_left_mid.predict(response_hist,true));
        region_list.push_back(svm_mid.predict(response_hist,true));
        region_list.push_back(svm_right_mid.predict(response_hist,true));
        region_list.push_back(svm_right.predict(response_hist,true));
        
        //find the index of the minimum value, which is the region id
        region_id = std::min_element(region_list.begin(), region_list.end()) - region_list.begin();

	    // move based on region id
        //moveToCenterBehavior.drive_to_center(region_id);
        break;
        printf("testid: %d\n", region_id);
    }
    //driveOverRampBehavior.go();

    return 0;
}
