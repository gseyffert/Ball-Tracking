#include "opencv2/opencv.hpp"
#include "BallTracking.h"

using namespace cv;

int MAX_CANDIDATES = 10;

bool showImage = false; 

int main(int argc, char** argv)
{
    if (argc != 2) { // check arguments
        printf("Incorrect Arguments: Specify a Source Video\n"); 
        return -1;
    }

    VideoCapture cap(argv[1]); // open the specified video

    if (showImage) namedWindow("edges",1);

    if(!cap.isOpened()) { // check if we succeeded
        printf("Video File %s not found!\n", argv[1]);
        return -1;
    }

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
      	detectBall(currFrame, candidateArray, OPEN_CV, &numCandidates);

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

        printf("%d: %d CANDS\n", i, numCandidates);

		totalCandidates += frameList[i].numCandidates;
        i++; 
    }
    
    LEN_VIDEO = i;

	node* graph = composeGraph(frameList, LEN_VIDEO);

	LinkedList<node*>* selectedCandidates = shortestPath(graph, totalCandidates);

	node* tempNode;
	candidate* tempCandidate;

    printf("Selected Candidates\n-------------------\n");

	for (int i = 0; i < LEN_VIDEO; i++) {
            printf("%d: ",i);
			tempNode = (*selectedCandidates).getNode(i)->item;	
			tempCandidate = tempNode->cand;
			printf("(%d, %d, %d, %f)\n", tempCandidate->x, tempCandidate->y, tempCandidate->radius, tempCandidate->probability);
	}

    visualize(selectedCandidates, argv[1]);

	freeGraph(frameList, graph, LEN_VIDEO);

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
