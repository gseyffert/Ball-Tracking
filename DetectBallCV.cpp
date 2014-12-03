#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <cstdio>
#include <ctime>
//#include <omp.h>
#include "BallTracking.h"

using namespace std;
using namespace cv;

//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=3;
//minimum and maximum object radius
const int MIN_OBJECT_RADIUS = 10;
const int MAX_OBJECT_RADIUS = 25;
//names that will appear at the top of each window
const String windowName = "Original Image";
const String trackbarWindowName = "Trackbars";
const String cannyThresholdTrackbarName = "Canny threshold";
const String accumulatorThresholdTrackbarName = "Accumulator Threshold";


const int cannyThresholdInitialValue = 13; // 180
const int accumulatorThresholdInitialValue = 13; //30
const int maxAccumulatorThreshold = 200;
const int maxCannyThreshold = 255;
//declare and initialize both parameters that are subjects to change
int cannyThreshold = cannyThresholdInitialValue;
int accumulatorThreshold = accumulatorThresholdInitialValue;

// booleans for speed
const bool showImage = true;
const bool showTrackbars = false;
const bool loopVideo = false;


void on_trackbar( int, void* )
{//This function gets called whenever a
    // trackbar position is changed

}

////////////////////////
// HELPER FUNCTIONS ////
////////////////////////
String intToString(int number){
    stringstream ss;
    ss << number;
    return ss.str();
}

void printCandidates(candidate* candidateArray){
        printf("(x, y, radius, probability)\n");
    for(int i = 0; i < MAX_NUM_OBJECTS; i++){
        candidate temp = candidateArray[i];
        printf("(%d, %d, %d, %f)\n", temp.x, temp.y, temp.radius, temp.probability);
    }
}
//////////////////////////

void HoughDetection(Mat& src_gray, Mat& src, int cannyThreshold, int accumulatorThreshold, candidate* candidateArray)
{
    // will hold the results of the detection
    vector<Vec3f> circles;
    // runs the actual detection
    HoughCircles( src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows/8, cannyThreshold, accumulatorThreshold, MIN_OBJECT_RADIUS, MAX_OBJECT_RADIUS );

    // clone the colour, input image for displaying purposes
    int size = min((int)circles.size(), MAX_NUM_OBJECTS);

    //no feature/circle found
    if (size == 0){
        //printf("---- NO CANDIDATES ------ ");
    }

    // loop though circles and add them to candidate
    for(int i = 0; i < size; i++ )
    {
        int x = cvRound(circles[i][0]);
        int y = cvRound(circles[i][1]);
        int radius = cvRound(circles[i][2]);
        
        //printf("X: %d Y: %d Radius: %d \n", x, y, radius);

        candidateArray[i] = candidate(x, y, radius, 1); //x, y, radius, probability

        if(showImage){
            Point center(x, y);
            // circle center
            circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
            // circle outline
            circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
        }
    }
}

void convertToGray(Mat& src, Mat& src_gray){

    int tmp;
    Vec3b color;
    for(int i = 0;i < src.cols;i++){
        for(int j = 0;j < src.rows;j++){
            color = src.at<Vec3b>(j, i);
            // bit mask for quick integer multiplication
            tmp = color[0] << 1 ;
            tmp += ( color[1] << 2 ) + color[1];
            tmp += color[2];
            src_gray.at<uchar>(j,i) = (unsigned char)(tmp >> 3);
        }
    }

}


// WORKS BUT TAKES A LOOOOONG TIME
// source channel, target channel, width, height, radius
void gaussBlur_Naive(Mat src, Mat dst, int w, int h, int r) {
    float rs = ceil(r * 2.57);   // significant radius
    for(int i=0; i < h; i++)
        for(int j=0; j<w; j++) {
            float val = 0, wsum = 0;
            for(int iy = i-rs; iy<i+rs+1; iy++)
                for(int ix = j-rs; ix<j+rs+1; ix++) {
                    float x = min(w-1, max(0, ix));
                    float y = min(h-1, max(0, iy));
                    float dsq = (ix-j)*(ix-j)+(iy-i)*(iy-i);
                    float wght = exp( -dsq / (2*r*r) ) / (3*2*r*r);
                    printf("%d\n", wght );
                    val += src.at<uchar>(y,x) * wght;  wsum += wght;
                }
            dst.at<uchar>(i,j) = round(val/wsum);            
        }
}


// reflected indexing for border processing
int reflect(int M, int x)
{
    if(x < 0)
    {
        return -x - 1;
    }
    if(x >= M)
    {
        return 2*M - x - 1;
    }
    return x;
}

void gaussBlur_1D(Mat& src, Mat& dst){
          // coefficients of 1D gaussian kernel with sigma = 1
      double coeffs[] = {0.0545, 0.2442, 0.4026, 0.2442, 0.0545};
      
      Mat temp;
      
      float sum, x1, y1;
      
      dst = src.clone();
      temp = src.clone();
 
        // along y - direction
        for(int y = 0; y < src.rows; y++){
            for(int x = 0; x < src.cols; x++){
                sum = 0.0;
                for(int i = -2; i <= 2; i++){
                    y1 = reflect(src.rows, y - i);
                    sum = sum + coeffs[i + 2]*src.at<uchar>(y1, x);
                }
                temp.at<uchar>(y,x) = sum;
            }
        }
 
        // along x - direction
        for(int y = 0; y < src.rows; y++){
            for(int x = 0; x < src.cols; x++){
                sum = 0.0;
                for(int i = -2; i <= 2; i++){
                    x1 = reflect(src.cols, x - i);
                    sum = sum + coeffs[i + 2]*temp.at<uchar>(y, x1);
                }
                dst.at<uchar>(y,x) = sum;
            }
        }
}

void detectBall(Mat src, candidate* candidateArray) {
    /**
     * Takes in a Mat (image matrix) src and a pointer to an Array of Candidates
     * Returns void, but fills candidate Array with possible candidates
     */

    Mat src_gray(src.rows, src.cols, CV_8U);

    Mat src_gray_CV;
    

    // Convert it to gray
    cvtColor( src, src_gray_CV, COLOR_BGR2GRAY );

    convertToGray(src, src_gray);
    
    // Reduce the noise so we avoid false circle detection
    GaussianBlur( src_gray_CV, src_gray_CV, Size(9, 9), 2, 2 );
    gaussBlur_1D( src_gray, src_gray);

    // those paramaters cannot be =0
    // so we must check here
    cannyThreshold = max(cannyThreshold, 1);
    accumulatorThreshold = max(accumulatorThreshold, 1);

    //runs the detection, and update the display
    HoughDetection(src_gray, src, cannyThreshold, accumulatorThreshold, candidateArray);

    if(showImage){
        imshow(windowName,src);
        imshow(windowName + "Gray",src_gray);
        imshow(windowName + "Gray_CV",src_gray_CV);
    }
}
