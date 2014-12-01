#include "opencv2/opencv.hpp"
#include "BallTracking.h"

using namespace cv;

int MAX_CANDIDATES = 10;

bool showImage = true; 

int main(int argc, char** argv)
{
    if (argc != 2) { // check arguments
        printf("Incorrect Arguments: Specify a Source Video\n"); 
        return -1;
    }

    VideoCapture cap(argv[1]); // open the specified video

    if (showImage) namedWindow("edges",1);

    if(!cap.isOpened()) { // check if we succeeded
        printf("Video File not found!\n");
        return -1;
    }

		int LEN_VIDEO = cap.get(CAP_PROP_FRAME_COUNT);

    Mat currFrame;
    cap >> currFrame; // load the first frame of the source video
		

    frame* frameList = new frame[LEN_VIDEO];

    int i = 0;

    while(!currFrame.empty())
    {
        if (showImage) {
            imshow("edges", currFrame);
            waitKey(10);
        }

				// allocate candidate space
				candidate* candidateArray = (candidate*) malloc(sizeof(candidate) * MAX_CANDIDATES);	
				
        // call detectBall 
      	detectBall(currFrame, candidateArray);

        cap >> currFrame; // get a new frame from source video

        frameList[i].numCandidates = 3;
        frameList[i].candidateList = candidateArray;
        frameList[i].nodes = NULL;

        i++; 
    }

		composeGraph(frameList, LEN_VIDEO);


    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
