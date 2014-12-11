#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <cstdio>
#include <ctime>
// #include <omp.h>
#include "BallTracking.h"

//PARAMS :
// putts : 10,10,25
// dribbles : 5,50,75


//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=10;
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
const bool showImage = false;
const bool showTrackbars = false;

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

void HoughDetection(Mat& src_gray, Mat& src, int cannyThreshold, int accumulatorThreshold, candidate* candidateArray, int* numCandidates)
{
    // will hold the results of the detection
    vector<Vec3f> circles;
    // runs the actual detection
    HoughCircles( src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows/8, cannyThreshold, accumulatorThreshold, MIN_OBJECT_RADIUS, MAX_OBJECT_RADIUS );

    // clone the colour, input image for displaying purposes
    int size = min((int)circles.size(), MAX_NUM_OBJECTS);
	*numCandidates = size;

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
            //circle center
            circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
            //circle outline
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

void convertToGray_Optimized(Mat& src, Mat& src_gray){

    int tmp;
    Vec3b color;
    uchar *rowPtrSrcGray;

    // #pragma omp parallel for num_threads(16) schedule(dynamic) private(color, tmp, rowPtrSrcGray)
    for(int i = 0;i < src.cols;i++){
        rowPtrSrcGray= src_gray.ptr<uchar>(i);
        for(int j = 0;j < src.rows;j++){
            color = src.at<Vec3b>(j, i);
            // bit mask for quick integer multiplication
            tmp = color[0] << 1 ;
            tmp += ( color[1] << 2 ) + color[1];
            tmp += color[2];
            rowPtrSrcGray[j] = (unsigned char)(tmp >> 3);
        }
    }
}

// source channel, target channel, width, height, radius
void gaussBlur_Naive(Mat src, Mat dst, int w, int h, int r) {

    float rs = ceil(r * 2.57);   // significant radius
    for(int j=0; j<w; j++) {

        for(int i=0; i < h; i++){

            float val = 0, wsum = 0;

            for(int iy = i-rs; iy<i+rs+1; iy++){

                for(int ix = j-rs; ix<j+rs+1; ix++) {
								
                    float x = min(w-1, max(0, ix));
                    float y = min(h-1, max(0, iy));
                    float dsq = (ix-j)*(ix-j)+(iy-i)*(iy-i);
                    float wght = exp( -dsq / (2*r*r) ) / (3*2*r*r);
                    // printf("%d\n", wght );
                    val += src.at<uchar>(y,x) * wght;  wsum += wght;

                }
            }
            dst.at<uchar>(i,j) = round(val/wsum);            
        }
    }
}

// source channel, target channel, width, height, radius
void gaussBlur_Optimized2(Mat src, int w, int h) {
    // r = 2
    uchar *rowPtrSrc;
    uchar *rowPtrDst;
    int iy,ix,i,j;
    float dsq, wght;
    float val, wsum;

    // significant radius = 6
    // w = num_columns, w = col
    // h = num_rows, i = row
    // #pragma omp parallel for num_threads(1)
    // #pragma omp parallel for num_threads(16) schedule(dynamic) private(rowPtrSrc, rowPtrDst, iy,ix,i,j, dsq, wght, val, wsum) //reduction(+:totalCandidates)
    for(i=0; i < h; i++){
        rowPtrDst= src.ptr<uchar>(i);
        for(j=0; j<w; j++) {
            val = 0, wsum = 0;
            // printf("%d\n", omp_get_num_threads());
            // vsum = _mm_setzero_ps();
            for(iy = max(0,i-6); iy<=min(h-1,i+6); iy++) {
                rowPtrSrc= src.ptr<uchar>(iy);
                for(ix = max(0,j-6); ix<=min(w-1,j+6); ix++) {
                    dsq = (ix-j)*(ix-j)+(iy-i)*(iy-i);
                    wght = exp( -dsq / (2*2*2) ) / (3*2*2*2);
                    val += rowPtrSrc[ix] * wght;  wsum += wght;
                }
            }
            rowPtrDst[j] = round(val/wsum);            
        }
    }
}



// // source channel, target channel, width, height, radius
// void gaussBlur_Optimized(Mat src, int w, int h) {
//     // r = 2
    // uchar *rowPtrSrc;
    // uchar *rowPtrDst;
    // int x,y,iy,i,j;
    // float dsq, wght;
    // float val, wsum;

//     // significant radius = 6
//     // w = num_columns, j = col
//     // h = num_rows, i = row
//     // #pragma omp parallel for num_threads(1)
//     for(i=0; i < h; i++){
//         __m128 frame_line;
//         __m128 vsum;
//         __m128 vval;
//         __m128 vectorix;
//         __m128 vdsq;
//         __m128 vectorJ;
//         __m128 vectorIYI;

//         __m128 offsets = _mm_set_ps(0,1,2,3);

//         float *vsum_array;
//         vsum_array = (float*)malloc(4 * sizeof(float));
//         float *vval_array;
//         vval_array = (float*)malloc(4 * sizeof(float));

//         rowPtrDst= src.ptr<uchar>(i);
//         for(j=0; j<w; j++) {
//             vectorJ = _mm_set1_ps((float)j);
//             val = 0, wsum = 0;
//             // printf("%d\n", omp_get_num_threads());
//             vsum = _mm_setzero_ps();
//             vval = _mm_setzero_ps();
//             for(iy = max(0,i-6); iy<=min(h-1,i+6); iy++) {
//                 vectorIYI = _mm_set1_ps((float)((iy-i)*(iy-i)));
//                 int ix;
//                 rowPtrSrc= src.ptr<uchar>(iy);
//                 for(ix = max(0,j-6); ix + 3<=min(w-1,j+6); ix+=4) {
//                     frame_line = _mm_loadu_ps((float*)&rowPtrSrc[ix]);
//                     vectorix = _mm_add_ps(_mm_set1_ps((float)ix), offsets);
//                     vdsq = _mm_sub_ps(vectorix, vectorJ);
//                     vdsq = _mm_mul_ps(vdsq,vdsq);
//                     vdsq = _mm_add_ps(vdsq,vectorIYI);
//                     vdsq = _mm_div_ps(_mm_exp_ps(_mm_div_ps(_mm_sub_ps(_mm_setzero_ps(),vdsq), _mm_set1_ps(8))), _mm_set1_ps(24))
//                     // dsq = (ix-j)*(ix-j)+(iy-i)*(iy-i);
//                     // wght = exp( -dsq / (2*2*2) ) / (3*2*2*2);
//                     vval = _mm_add_ps(vval, _mm_mul_ps(frame_line,vdsq));
//                     vsum = _mm_add_ps(vsum, vdsq);
//                     // val += rowPtrSrc[ix] * wght;  wsum += wght;
//                 }
//                 for(; ix<=min(w-1,j+6); ix++) {
//                     dsq = (ix-j)*(ix-j)+(iy-i)*(iy-i);
//                     wght = exp( -dsq / (2*2*2) ) / (3*2*2*2);
//                     val += rowPtrSrc[ix] * wght;  wsum += wght;
//                 }
//             }
//             _mm_storeu_ps(vsum_array, vsum);
//             wsum += *vsum_array + *(vsum_array+1) + *(vsum_array + 2) + *(vsum_array + 3);
//             _mm_storeu_ps(vval_array, vval);
//             val += *vval_array + *(vval_array+1) + *(vval_array + 2) + *(vval_array + 3);
//             rowPtrDst[j] = round(val/wsum);            
//         }
//     }
// }

// int* boxesForGauss()  // standard deviation, number of boxes
// {
//     // sigma = 2
//     // n = 3
//    float wIdeal = sqrt((12*2*2/3)+1);  // Ideal averaging filter width 
//    int wl = floor(wIdeal);
//    if(wl%2==0)
//         wl--;
//    int wu = wl+2;
              
//    int mIdeal = (12*2*2 - 3*wl*wl - 4*3*wl - 3*3)/(-4*wl - 4);
//    int m = round(mIdeal);
//    // var sigmaActual = Math.sqrt( (m*wl*wl + (n-m)*wu*wu - n)/12 );
//    int*  sizes = (int*) malloc(sizeof(int) * 3);

//    for(int i=0; i<3; i++){
//     if (i<m)
//         sizes[i] = wl;
//     else
//         sizes[i] = wu;
//    }
//    for(int i = 0; i < 3; i++){
//         printf("(%d, %d)\n", i, sizes[i]);
//     }
//    return sizes;
// }
// 

void boxBlur_2 (Mat& scl, int w,int h, int r) {

    uchar *rowPtrSrc;
    uchar *rowPtrDst;
    int x,y, iy, i;
    float val;

    int bigR = (r+r+1)*(r+r+1);

    for(i=0; i<h; i++){
        rowPtrDst= scl.ptr<uchar>(i);
        #pragma omp parallel for num_threads(8) schedule(dynamic,50) private(i)
        for(int j=0; j<w; j++) {
            val = 0;
            for(int iy = max(0,i-r); iy<=min(h-1,i+r); iy++){
                int ix;
                rowPtrSrc= scl.ptr<uchar>(iy);
                // for(ix = max(0,j-r); ix + 5<=min(w-1,j+r); ix+=6) {
                //         val += rowPtrSrc[ix];
                //         val += rowPtrSrc[ix+1];
                //         val += rowPtrSrc[ix+2];
                //         val += rowPtrSrc[ix+3];
                //         val += rowPtrSrc[ix+4];
                //         val += rowPtrSrc[ix+5];
                // }
                // for(ix = max(0,j-r); ix + 3<=min(w-1,j+r); ix+=4) {
                //         val += rowPtrSrc[ix];
                //         val += rowPtrSrc[ix+1];
                //         val += rowPtrSrc[ix+2];
                //         val += rowPtrSrc[ix+3];
                // }
                for(ix = max(0,j-r); ix <=min(w-1,j+r); ix++) {
                        val += rowPtrSrc[ix];
                }
            rowPtrDst[j] = val/(bigR);
            }
        }
    }
}


void boxBlur_3 (Mat& scl, Mat& tcl, int w,int h, int r) {

    uchar *rowPtrSrc;
    uchar *rowPtrDst;
    int x,y, ix, iy, i,j;
    float val;

    __m128 frame_line;
    __m128 vsum;
    __m128 vval;
    __m128 vectorix;
    __m128 vdsq;

    float *vval_array;
    vval_array = (float*)malloc(4 * sizeof(float));

    int bigR = (r+r+1)*(r+r+1);

    // #pragma omp parallel for num_threads(16) schedule(dynamic) private(rowPtrSrc, rowPtrDst, iy,ix,i,j, val)
    for(i=0; i<h; i++){
        rowPtrDst= tcl.ptr<uchar>(i);
        for(j=0; j<w; j++) {
            val = 0;
            vval = _mm_setzero_ps();
            for(iy = max(0,i-r); iy<=min(h-1,i+r); iy++){
                int ix;
                rowPtrSrc= scl.ptr<uchar>(iy);
                for(ix = max(0,j-r); ix+3<=min(w-1,j+r); ix+=4) {
                    frame_line = _mm_loadu_ps((float*)&rowPtrSrc[ix]);
                    vval = _mm_add_ps(vval, frame_line);
                }
                for(ix = max(0,j-r); ix<=min(w-1,j+r); ix++) {
                    val += rowPtrSrc[ix];
                }
            _mm_storeu_ps(vval_array, vval);
            val += *vval_array + *(vval_array+1) + *(vval_array + 2) + *(vval_array + 3);
            rowPtrDst[j] = val/(bigR);
            }
        }
    }
}


void gaussBlur_2 (Mat& scl, int w, int h) {
    // int* bxs = boxesForGauss(); = 
    // boxBlur_3 (scl, tcl, w, h, (3-1)/2);
    // boxBlur_3 (tcl, scl, w, h, (5-1)/2);
    // boxBlur_3 (scl, tcl, w, h, (5-1)/2);
    boxBlur_2 (scl, w, h, (3-1)/2);
    boxBlur_2 (scl, w, h, (5-1)/2);
    boxBlur_2 (scl, w, h, (5-1)/2);
}


void detectBall(Mat src, candidate* candidateArray, const int type, int* numCandidates) {
    /**
     * Takes in a Mat (image matrix) src and a pointer to an Array of Candidates
     * Returns void, but fills candidate Array with possible candidates
     */
    Mat src_gray;
    if (type == OPEN_CV){
        // Convert it to gray
        cvtColor( src, src_gray, COLOR_BGR2GRAY );
        // Reduce the noise so we avoid false circle detection
        GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );
    }
    else if (type == NAIVE){
        src_gray = Mat(src.rows, src.cols, CV_8U);
        convertToGray(src, src_gray);
        gaussBlur_Naive( src_gray, src_gray, src_gray.cols,  src_gray.rows, 2);
    }
    else if (type == OPTIMIZED){
        src_gray = Mat(src.rows, src.cols, CV_8U);
        convertToGray(src, src_gray);
        gaussBlur_2( src_gray, src_gray.cols,  src_gray.rows);
    }

    // those paramaters cannot be = 0 so we must check here
    cannyThreshold = max(cannyThreshold, 1);
    accumulatorThreshold = max(accumulatorThreshold, 1);

    //runs the detection, and update the display
    HoughDetection(src_gray, src, cannyThreshold, accumulatorThreshold, candidateArray, numCandidates);

    if(showImage){
        imshow(windowName,src);
        waitKey(5);
        //imshow(windowName + "Gray",src_gray);
    }
}
