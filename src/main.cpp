#include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv/cv.h>
//#include "opencv2/opencv.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"

using namespace std; 
 
#include "camera.h"
#include "detection.h"
#include "prediction.h"
int main(int argc, char **argv) {
    // camera_test(argc, argv);
    // detection_test(argc, argv);
    // detection_measure(argc, argv);
    // prediction_test(argc, argv);
    // prediction_measure(argc, argv);
    prediction_performance(argc, argv);
    return 0;
}
