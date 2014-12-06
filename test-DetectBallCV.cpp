#include "DetectBallCV.cpp"

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
    clock_t start;
    double duration;
    start = clock();
		
		int numCandidates;

    // main function
    detectBall(src, candidateArray, NAIVE, &numCandidates);

    //Compute time and print
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    printCandidates(candidateArray);
    cout<<NAIVE<<" Total Time: "<< duration <<'\n';
    //////////////////////////////////////////////////////////////////////
    
    //////////////////////////////// MAIN //////////////////////////////
    // initialize clock
    start = clock();

    // main function
    detectBall(src, candidateArray, OPEN_CV, &numCandidates);

    //Compute time
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    printCandidates(candidateArray);
    cout<<OPEN_CV<<" Total Time: "<< duration <<'\n';
    //////////////////////////////////////////////////////////////////////


    //image will not appear without this waitKey() command
    if(showImage)
        waitKey(0);

    return 0;
}


