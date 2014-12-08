#include "opencv2/opencv.hpp"
#include "BallTracking.h"
#include <time.h>
#include <sys/time.h>

using namespace cv;

int MAX_CANDIDATES = 10;

const int MODE = OPEN_CV;

bool showImage = false; 

double timestamp()
{
	struct timeval tv;
	gettimeofday (&tv, 0);
	return tv.tv_sec + 1e-6*tv.tv_usec;
}


int main(int argc, char** argv)
{
    if (argc != 2) { // check arguments
        printf("Incorrect Arguments: Specify a Source Video\n"); 
        return -1;
    }

    VideoCapture cap(argv[1]); // open the specified video

    if (showImage) namedWindow("edges",1);

    if(!cap.isOpened()) { // check if we succeeded
        return -1;
    }

    double t0 = timestamp();    
    
	int LEN_VIDEO = cap.get(CAP_PROP_FRAME_COUNT);

    printf("Loaded %d frame video\n", LEN_VIDEO);

    Mat currFrame;
    cap >> currFrame; // load the first frame of the source video

    frame* frameList = new frame[LEN_VIDEO];

    int i = 0;
	int totalCandidates = 0;
	int numCandidates;

    while(!currFrame.empty())
    {

        if (showImage) {
            imshow("edges", currFrame);
            waitKey(10);
        }

		// allocate candidate space
		candidate* candidateArray = (candidate*) malloc(sizeof(candidate) * MAX_CANDIDATES);	

        // call detectBall 
      	detectBall(currFrame, candidateArray, MODE, &numCandidates);

        cap >> currFrame; // get a new frame from source video

        if (numCandidates == 0) {
            if (i == 0) exit(1);
            //no candidates
            frameList[i] = frameList[i-1];
        } else {
            frameList[i].numCandidates = numCandidates;
            frameList[i].candidateList = candidateArray;
            frameList[i].nodes = NULL;
        }

        //printf("%d: %d potential candidates detected\n", i, numCandidates);

		totalCandidates += frameList[i].numCandidates;
        i++; 
    }
    
    double t_afterDetect = timestamp() - t0;

    LEN_VIDEO = i;

	node* graph = composeGraph(frameList, LEN_VIDEO);

    double t_afterCompose = timestamp() - t0;

	LinkedList<node*>* selectedCandidates = shortestPath(graph, totalCandidates);

    double t_afterPath = timestamp() - t0; 

	node* tempNode;
	candidate* tempCandidate;

    visualize(selectedCandidates, argv[1]);

    double t_afterVis = timestamp() - t0;

	freeGraph(frameList, graph, LEN_VIDEO);

    double t_afterFree = timestamp() - t0;

    printf("Execution Completed for %d frame video:\n", LEN_VIDEO);
    printf("Detect Ball   : %f elapsed ( %f %% )\n", t_afterDetect, 100*t_afterDetect/t_afterFree);
    printf("Compose Graph : %f elapsed ( %f %% )\n", t_afterCompose - t_afterDetect, 100*(t_afterCompose - t_afterDetect)/t_afterFree);
    printf("Shortest Path : %f elapsed ( %f %% ) \n", t_afterPath - t_afterCompose, 100*(t_afterPath - t_afterCompose)/t_afterFree);
    printf("Visualize     : %f elapsed ( %f %% ) \n", t_afterVis - t_afterPath, 100*(t_afterVis - t_afterPath)/t_afterFree);
    printf("Free Graph    : %f elapsed ( %f %% ) \n", t_afterFree - t_afterVis, 100*(t_afterFree - t_afterVis)/t_afterFree);
    printf("--------------------------------\n");
    printf("Total         : %f elapsed\n", t_afterFree);

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
