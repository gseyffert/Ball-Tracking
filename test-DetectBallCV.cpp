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
    int num;

    // main function
    detectBall(src, candidateArray);

    //Compute time
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    //////////////////////////////////////////////////////////////////////



    //print candidates
    printCandidates(candidateArray);
    cout<<"Total Time: "<< duration <<'\n';

    return 0;
}


