#include "opencv2/opencv.hpp"
#include "BallTracking.h"

using namespace cv;

int LEN_VIDEO = 300; 

bool showImage = true; 

int main(int argc, char** argv)
{
    if (argc != 2) { // check arguments
        printf("Incorrect Arguments\n"); 
        return -1;
    }

    VideoCapture cap(argv[1]); // open the specified video

    if (showImage) namedWindow("edges",1);

    if(!cap.isOpened()) { // check if we succeeded
        printf("Video File not found!\n");
        return -1;
    }

    Mat currFrame;
    cap >> currFrame; // load the first frame of the source video

    frame* frameList = (frame*) malloc(sizeof(frame) * LEN_VIDEO);

    int i = 0;

    while(!currFrame.empty())
    {
        if (showImage) {
            imshow("edges", currFrame);
            waitKey(10);
        }

        // call detectBall 
        // detectBall(currFrame, [thresholds]);

        cap >> currFrame; // get a new frame from source video

        frameList[i]->numCandidates = 3;
        frameList[i]->candidateList = NULL;
        frameList[i]->nodes = NULL;

        i++; 
    }



    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
