#include "DetectBall.cpp"
#include <sys/time.h>

int main(int argc, char* argv[])
{
    //Matrix to store each frame of the webcam feed
    Mat src;

    if (argc < 2)
    {
        cerr<<"No input image specified\n";
        return 0;
    }

    // create the main window, and attach the trackbars
    if(showImage)
        namedWindow( windowName, WINDOW_AUTOSIZE );
    if(showTrackbars){
        createTrackbar(cannyThresholdTrackbarName, windowName, &cannyThreshold,maxCannyThreshold);
        createTrackbar(accumulatorThresholdTrackbarName, windowName, &accumulatorThreshold, maxAccumulatorThreshold);
    }

    //read image to Mat
    src = imread(argv[1], 1);

    // make sure image is not empty
    if( src.empty() )
    {
        cerr<<"Invalid input image\n";
    }
    
    candidate *candidateArray = (candidate*)malloc(sizeof(candidate)*MAX_NUM_OBJECTS);


    //////////////////////////////// MAIN //////////////////////////////
    // initialize clock
    struct timeval start, end;
    float delta = 0;
    gettimeofday(&start, NULL);
		
	int numCandidates;

    // main function
    detectBall(src, candidateArray, NAIVE, &numCandidates);

    //Compute time and print
    gettimeofday(&end, NULL);
    delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
         end.tv_usec - start.tv_usec) / 1.e6;
    printCandidates(candidateArray);
    cout<<"Naive Total Time: "<< delta <<'\n';
    //////////////////////////////////////////////////////////////////////
    
    //////////////////////////////// MAIN //////////////////////////////
    // initialize clock
    gettimeofday(&start, NULL);

    // main function
    detectBall(src, candidateArray, OPEN_CV, &numCandidates);

    //Compute time
    gettimeofday(&end, NULL);
    delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
         end.tv_usec - start.tv_usec) / 1.e6;
    printCandidates(candidateArray);
    cout<<"Open CV Total Time: "<< delta <<'\n';
    //////////////////////////////////////////////////////////////////////
    
    //////////////////////////////// MAIN //////////////////////////////
    // initialize clock
    gettimeofday(&start, NULL);

    // main function
    detectBall(src, candidateArray, OPTIMIZED, &numCandidates);

    //Compute time
    gettimeofday(&end, NULL);
    delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
         end.tv_usec - start.tv_usec) / 1.e6;
    printCandidates(candidateArray);
    cout<<"Optimized Total Time: "<< delta <<'\n';
    //////////////////////////////////////////////////////////////////////

    //image will not appear without this waitKey() command
    if(showImage)
        waitKey(0);

    return 0;
}


