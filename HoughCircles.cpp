#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>

using namespace std;
using namespace cv;

//max number of objects to be detected in frame
const size_t MAX_NUM_OBJECTS=3;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//minimum and maximum object radius
const int MIN_OBJECT_RADIUS = 10;
const int MAX_OBJECT_RADIUS = 25;
//names that will appear at the top of each window
const String windowName = "Original Image";
const String trackbarWindowName = "Trackbars";
const std::string cannyThresholdTrackbarName = "Canny threshold";
const std::string accumulatorThresholdTrackbarName = "Accumulator Threshold";


const int cannyThresholdInitialValue = 180;
const int accumulatorThresholdInitialValue = 150; //30
const int maxAccumulatorThreshold = 200;
const int maxCannyThreshold = 255;

void on_trackbar( int, void* )
{//This function gets called whenever a
    // trackbar position is changed

}

String intToString(int number){
    stringstream ss;
    ss << number;
    return ss.str();
}

void HoughDetection(Mat& src_gray, Mat& src, int cannyThreshold, int accumulatorThreshold, int imageNumber)
{
    // will hold the results of the detection
    std::vector<Vec3f> circles;
    // runs the actual detection
    HoughCircles( src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows/8, cannyThreshold, accumulatorThreshold, MIN_OBJECT_RADIUS, MAX_OBJECT_RADIUS );

    // clone the colour, input image for displaying purposes
    int size = std::min((int)circles.size(), 2);

    for(int i = 0; i < size; i++ )
    {
        int x = cvRound(circles[i][0]);
        int y = cvRound(circles[i][1]);
        int radius = cvRound(circles[i][2]);
        printf("Image: %d X: %d Y: %d Radius: %d \n", imageNumber, x, y, radius);
        Point center(x, y);
        // circle center
        circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
    }
}


int main(int argc, char* argv[])
{
    //Matrix to store each frame of the webcam feed
    Mat src, src_gray;

    if (argc < 2)
    {
        std::cerr<<"No input image specified\n";
        return -1;
    }

    //declare and initialize both parameters that are subjects to change
    int cannyThreshold = cannyThresholdInitialValue;
    int accumulatorThreshold = accumulatorThresholdInitialValue;

    // create the main window, and attach the trackbars
    namedWindow( windowName, WINDOW_AUTOSIZE );
    createTrackbar(cannyThresholdTrackbarName, windowName, &cannyThreshold,maxCannyThreshold);
    createTrackbar(accumulatorThresholdTrackbarName, windowName, &accumulatorThreshold, maxAccumulatorThreshold);


    //all of our operations will be performed within this loop
    while(1){        

        for (int i =200; i <= 299 ; i++){
        
            //get next image name
            char buffer [50];
            int n, a=5, b=3;
            sprintf (buffer, "putts/%03d.jpg", i);
            // printf ("%s\n",buffer);

            //store image to matrix     
            src = imread( buffer, 1 );

            // make sure image is not empty
            if( src.empty() )
            {
                std::cerr<<"Invalid input image\n";
                return -1;
            }

            // Convert it to gray
            cvtColor( src, src_gray, COLOR_BGR2GRAY );

            // Reduce the noise so we avoid false circle detection
            GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );

            // those paramaters cannot be =0
            // so we must check here
            cannyThreshold = std::max(cannyThreshold, 1);
            accumulatorThreshold = std::max(accumulatorThreshold, 1);

            //runs the detection, and update the display
            HoughDetection(src_gray, src, cannyThreshold, accumulatorThreshold, i);

            imshow(windowName,src);
            // imshow(windowName + "2",src_gray);
            
            //image will not appear without this waitKey() command
            waitKey(1);
        }
    }
    return 0;
}
