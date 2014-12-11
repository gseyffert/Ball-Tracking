#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "OpenCLUtilities/openCLUtilities.hpp"


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#include <opencv2/gpu/gpu.hpp>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
//#include <omp.h>
// #include <omp.h>
#include "BallTracking.h"
#include "clhelp.h"
#include "clhelp.cpp"
//PARAMS :
// putts : 10,10,25
// dribbles : 5,50,75

using namespace cl;
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
const bool showImage = true;
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

void oclToGray(cl_command_queue &queue,
               cl_context &context,
               cl_kernel &toGray,
               cl_kernel &gaussBlur,
               Mat* src, 
               Mat* dest,
               int numFrames) {
  
   ::size_t local_work_size[1] = {128};

   cl_int err = CL_SUCCESS;
   for (int i = 0; i < numFrames; i++) { 
    /*Mat curMat = src[i];
    Mat* destMat = new Mat();*/
    int rows = src[i].rows;
    int cols = src[i].cols;
    int size = rows*cols;
    unsigned char *in_array;
    unsigned char *out_array = new unsigned char[rows*cols];
    //if(src[i].isContinuous())
    in_array = src[i].data;

    cl_mem srcc, dst_gray, dst_blur;


    srcc = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(uchar)*size*3,NULL,&err);
    CHK_ERR(err);
    dst_gray = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(uchar)*size,NULL,&err);
    CHK_ERR(err);
    dst_blur = clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(uchar)*size,NULL,&err);
    CHK_ERR(err);

    err = clEnqueueWriteBuffer(queue, srcc, true, 0, sizeof(uchar)*size*3, in_array, 0, NULL, NULL);
    CHK_ERR(err);


    ::size_t global_work_size[1] = {(unsigned long)size};


    err = clSetKernelArg(toGray, 0, sizeof(cl_mem), &srcc);
    CHK_ERR(err); 
    err = clSetKernelArg(toGray, 1, sizeof(cl_mem), &dst_gray);
    CHK_ERR(err);
    err = clSetKernelArg(toGray, 2, sizeof(int), &rows);
    CHK_ERR(err);
    err = clSetKernelArg(toGray, 3, sizeof(int), &cols);
    CHK_ERR(err);

    printf("converting to gray\n");
    err = clEnqueueNDRangeKernel(queue,
        toGray,
        1,
        NULL,
        global_work_size,
        local_work_size,
        0,
        NULL,
        NULL
        );
    CHK_ERR(err);

    printf("Done converting to gray\n");

    err = clSetKernelArg(gaussBlur, 0, sizeof(cl_mem), &dst_gray);
    CHK_ERR(err); 
    err = clSetKernelArg(gaussBlur, 1, sizeof(cl_mem), &dst_blur);
    CHK_ERR(err);
    err = clSetKernelArg(gaussBlur, 2, sizeof(int), &rows);
    CHK_ERR(err);
    err = clSetKernelArg(gaussBlur, 3, sizeof(int), &cols);
    CHK_ERR(err);
    err = clEnqueueNDRangeKernel(queue,
        gaussBlur,
        1,
        NULL,
        global_work_size,
        local_work_size,
        0,
        NULL,
        NULL
        );
    CHK_ERR(err);
    printf("Done blurring\n");

    err = clEnqueueReadBuffer(queue, dst_blur, true, 0, sizeof(uchar)*size,
                out_array, 0, NULL, NULL);
    CHK_ERR(err);    
    printf("Done reading\n");
    
    dest[i].rows = rows;
    dest[i].cols = cols;
    dest[i].data = out_array;
    printf("Done reading2\n");

    clReleaseMemObject(srcc);
    clReleaseMemObject(dst_gray);
    clReleaseMemObject(dst_blur); 
    printf("Done reading3\n");
  }
}

void convertToGray_Optimized(Mat& src, Mat& src_gray){

    std::string toGray_kernel_str, gaussBlur_kernel_str;
    

    cl_int err = CL_SUCCESS;
    /* Provide names of the OpenCL kernels
     * and cl file that they're kept in */
    std::string toGray_name_str = std::string("toGray");
    std::string toGray_kernel_file = std::string("toGray.cl");
    std::string gaussBlur_name_str = std::string("gaussBlur");
    std::string gaussBlur_kernel_file = std::string("gaussBlur.cl");
    cl_vars_t cv;
    cl_kernel toGray, gaussBlur;
    
    /* Read OpenCL file into STL string */
    readFile(toGray_kernel_file, toGray_kernel_str);
    readFile(gaussBlur_kernel_file, gaussBlur_kernel_str);

    /* Initialize the OpenCL runtime
     * Source in clhelp.cpp */
    initialize_ocl(cv);
    
    /* Compile all OpenCL kernels */
    compile_ocl_program(toGray, cv, toGray_kernel_str.c_str(),
                        toGray_name_str.c_str());
    compile_ocl_program(gaussBlur, cv, gaussBlur_kernel_str.c_str(),
                        gaussBlur_name_str.c_str());
    
    oclToGray(cv.commands, cv.context, toGray, gaussBlur, &src, &src_gray, 1);

    err = clFlush(cv.commands);
    CHK_ERR(err);
    
    uninitialize_ocl(cv);

}


void convertToGray(Mat& src, Mat& src_gray) {
    int tmp;
    Vec3b color;
    for(int i = 0;i < src.cols;i++){
        for(int j = 0;j < src.rows;j++){
            color = src.at<Vec3b>(j, i);
            //bit mask for quick integer multiplication
            tmp = color[0] << 1 ;
            tmp += ( color[1] << 2 ) + color[1];
            tmp += color[2];
            src_gray.at<uchar>(j,i) = (unsigned char)(tmp >> 3);
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
                    val += src.at<uchar>(y,x) * wght;  
                    wsum += wght;

                }
            }
            dst.at<uchar>(i,j) = round(val/wsum);            
        }
    }
}

// source channel, target channel, width, height, radius
void gaussBlur_Optimized(Mat src, int w, int h) {
    // r = 2
    uchar *rowPtrSrc;
    uchar *rowPtrDst;
    int x,y,iy,ix,i,j;
    float dsq, wght;
    float val, wsum;
    // significant radius = 6
    // w = num_columns, w = col
    // h = num_rows, i = row
    // #pragma omp parallel for num_threads(1)
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


/*
// source channel, target channel, width, height, radius
void gaussBlur_Optimized(Mat src, int w, int h) {
    // r = 2
    uchar *rowPtrSrc;
    uchar *rowPtrDst;
    int x,y,iy,i,j;
    float dsq, wght;
    float val, wsum;

    // significant radius = 6
    // w = num_columns, j = col
    // h = num_rows, i = row
    // #pragma omp parallel for num_threads(1)
    for(i=0; i < h; i++){
        __m128 frame_line;
        __m128 vsum;
        __m128 vval;
        __m128 vectorix;
        __m128 vdsq;
        __m128 vectorJ;
        __m128 vectorIYI;

        __m128 offsets = _mm_set_ps(0,1,2,3);

        float *vsum_array;
        vsum_array = (float*)malloc(4 * sizeof(float));
        float *vval_array;
        vval_array = (float*)malloc(4 * sizeof(float));

        rowPtrDst= src.ptr<uchar>(i);
        for(j=0; j<w; j++) {
            vectorJ = _mm_set1_ps((float)j);
            val = 0, wsum = 0;
            // printf("%d\n", omp_get_num_threads());
            vsum = _mm_setzero_ps();
            vval = _mm_setzero_ps();
            for(iy = max(0,i-6); iy<=min(h-1,i+6); iy++) {
                vectorIYI = _mm_set1_ps((float)((iy-i)*(iy-i)));
                int ix;
                rowPtrSrc= src.ptr<uchar>(iy);
                for(ix = max(0,j-6); ix + 3<=min(w-1,j+6); ix+=4) {
                    frame_line = _mm_loadu_ps((float*)&rowPtrSrc[ix]);
                    vectorix = _mm_add_ps(_mm_set1_ps((float)ix), offsets);
                    vdsq = _mm_sub_ps(vectorix, vectorJ);
                    vdsq = _mm_mul_ps(vdsq,vdsq);
                    vdsq = _mm_add_ps(vdsq,vectorIYI);
                    vdsq = _mm_div_ps(_mm_exp_ps(_mm_div_ps(_mm_sub_ps(_mm_setzero_ps(),vdsq), _mm_set1_ps(8))), _mm_set1_ps(24))
                    // dsq = (ix-j)*(ix-j)+(iy-i)*(iy-i);
                    // wght = exp( -dsq / (2*2*2) ) / (3*2*2*2);
                    vval = _mm_add_ps(vval, _mm_mul_ps(frame_line,vdsq));
                    vsum = _mm_add_ps(vsum, vdsq);
                    // val += rowPtrSrc[ix] * wght;  wsum += wght;
                }
                for(; ix<=min(w-1,j+6); ix++) {
                    dsq = (ix-j)*(ix-j)+(iy-i)*(iy-i);
                    wght = exp( -dsq / (2*2*2) ) / (3*2*2*2);
                    val += rowPtrSrc[ix] * wght;  wsum += wght;
                }
            }
            _mm_storeu_ps(vsum_array, vsum);
            wsum += *vsum_array + *(vsum_array+1) + *(vsum_array + 2) + *(vsum_array + 3);
            _mm_storeu_ps(vval_array, vval);
            val += *vval_array + *(vval_array+1) + *(vval_array + 2) + *(vval_array + 3);
            rowPtrDst[j] = round(val/wsum);            
        }
    }
}
*/

void detectBall(Mat src, candidate* candidateArray, const int type, int* numCandidates ) {
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
        cout<<"ENTERS LOOP\n";
        convertToGray_Optimized(src, src_gray);
        gaussBlur_Optimized( src_gray, src_gray.cols,  src_gray.rows);
    }

    // those paramaters cannot be = 0 so we must check here
    cannyThreshold = max(cannyThreshold, 1);
    accumulatorThreshold = max(accumulatorThreshold, 1);

    //runs the detection, and update the display
    HoughDetection(src_gray, src, cannyThreshold, accumulatorThreshold, candidateArray, numCandidates);

    if(showImage){
        imshow(windowName,src);
        waitKey(5);
        imshow(windowName + "Gray",src_gray);
    }
}
