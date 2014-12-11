#include "opencv2/opencv.hpp"
#include "BallTracking.h"
#include <time.h>
#include <sys/time.h>
#include <cstring>
#include <omp.h>

using namespace cv;

int MAX_CANDIDATES = 10;
int THREAD_NUM = 2;

const int MODE = OPTIMIZED;

#define showImage 0

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

    string filename = argv[1];

    VideoCapture cap(filename); // open the specified video

    if (showImage) namedWindow("vid",1);

    if(!cap.isOpened()) { // check if we succeeded
        return -1;
    }

    double t0 = timestamp();    
    
	int LEN_VIDEO = cap.get(CAP_PROP_FRAME_COUNT);

    printf("Loaded %d frame video\n", LEN_VIDEO);

	Mat* videoBuffer = new Mat[LEN_VIDEO];
    Mat currFrame;
    cap >> currFrame; // load the first frame of the source video

	int i = 0;


    while(!currFrame.empty())
    {

        if (showImage) {
            imshow("vid", currFrame);
            waitKey(10);
        }
		videoBuffer[i] = currFrame;
        cap >> currFrame; // get a new frame from source video    
        i++;
	}

	LEN_VIDEO = i;

    frame* frameList = new frame[LEN_VIDEO];

	int totalCandidates = 0;
	int numCandidates;

	#pragma omp parallel for num_threads(THREAD_NUM) schedule(dynamic) private(numCandidates, i, currFrame) reduction(+:totalCandidates)
	for(i = 0; i < LEN_VIDEO; i++){
		currFrame = videoBuffer[i]; 

		// allocate candidate space
		candidate* candidateArray = (candidate*) malloc(sizeof(candidate) * MAX_CANDIDATES);	

        // call detectBall
      	detectBall(currFrame, candidateArray, MODE, &numCandidates);

        if (numCandidates == 0) {
            printf("No candidates @ %d\n", i);
            if (i == 0) exit(1);
            //no candidates
            frameList[i] = frameList[i-1];
        } else {
            // printf("Candidates @ %d\n", i);
            frameList[i].numCandidates = numCandidates;
            frameList[i].candidateList = candidateArray;
            frameList[i].nodes = NULL;
        }
        //printf("%d: %d potential candidates detected\n", i, numCandidates);

    	totalCandidates += frameList[i].numCandidates;
    }
    
    double t_afterDetect = timestamp() - t0;

	node* graph = composeGraph(frameList, LEN_VIDEO);

    double t_afterCompose = timestamp() - t0;
	LinkedList<node*>* selectedCandidates = shortestPath(graph, totalCandidates);

    double t_afterPath = timestamp() - t0; 

	node* tempNode;
	candidate* tempCandidate;

    visualize(selectedCandidates, filename, LEN_VIDEO);

    double t_afterVis = timestamp() - t0;

    runffmpeg("output/%d.jpg", "tracked.mp4", "1", "720x400", "24");

    double t_afterFfmpeg = timestamp() - t0;

	freeGraph(frameList, graph, LEN_VIDEO-1);

    double t_afterFree = timestamp() - t0;

    printf("With video %s. And DetectBall MODE == OPEN_CV , and threads num = %d\n", argv[1], THREAD_NUM);
    printf("Execution Completed for %d frame video:\n", LEN_VIDEO);
    printf("Detect Ball   : %f elapsed ( %f %% )\n", t_afterDetect, 100*t_afterDetect/t_afterFree);
    printf("Compose Graph : %f elapsed ( %f %% )\n", t_afterCompose - t_afterDetect, 100*(t_afterCompose - t_afterDetect)/t_afterFree);
    printf("Shortest Path : %f elapsed ( %f %% ) \n", t_afterPath - t_afterCompose, 100*(t_afterPath - t_afterCompose)/t_afterFree);
    printf("Visualize     : %f elapsed ( %f %% ) \n", t_afterVis - t_afterPath, 100*(t_afterVis - t_afterPath)/t_afterFree);
    printf("ffmpeg        : %f elapsed ( %f %% ) \n", t_afterFfmpeg - t_afterVis, 100*(t_afterFfmpeg - t_afterVis)/t_afterFree);
    printf("Free Graph    : %f elapsed ( %f %% ) \n", t_afterFree - t_afterFfmpeg, 100*(t_afterFree - t_afterFfmpeg)/t_afterFree);
    printf("--------------------------------\n");
    printf("Total         : %f elapsed\n", t_afterFree);

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
