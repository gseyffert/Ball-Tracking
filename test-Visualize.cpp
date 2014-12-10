#include "Visualize.cpp"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr<<"No input video specified\n";
        return 0;
    }

    //read image to Mat
    LinkedList<node*>* o;

    VideoCapture cap(argv[1]); // open the specified video

    if(!cap.isOpened()) { // check if we succeeded
        printf("Video File not found!\n");
        return 0;
    }
    int LEN_VIDEO = cap.get(CAP_PROP_FRAME_COUNT);
    
    //////////////////////////////// MAIN //////////////////////////////
    // initialize clock
    clock_t start;
    double duration;
    start = clock();

    // main function
    visualize(0, argv[1], LEN_VIDEO);

    //Compute time and print
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout<<"Total Time: "<< duration <<'\n';
    //////////////////////////////////////////////////////////////////////

    return 0;
}