#include "opencv2/opencv.hpp"
#include "BallTracking.h"

using namespace cv;

int main(int argc, char** argv)
{
    if (argc != 2) { // check arguments
        printf("Incorrect Arguments\n"); 
        return -1;
    }

    VideoCapture cap(argv[1]); // open the specified video

    if(!cap.isOpened()) { // check if we succeeded
        printf("Video File not found!\n");
        return -1;
    }

    Mat currFrame;
    cap >> currFrame; // load the first frame of the source video

    while(currFrame != NULL)
    {
        imshow(currFrame);

        // call detectBall 
        // detectBall(currFrame, [thresholds]);

        cap >> currFrame; // get a new frame from source video
    }


    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}