#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ocl/ocl.hpp>
#include "clHelp.h"
#include "../BallTracking.h"

using namespace cv;
using namespace cv::ocl;

void oclToGray(cl_command_queue &queue,
               cl_context &context,
               cl_kernel &toGray,
               Mat* src, 
               Mat* dest,
               int numFrames) {
  size_t local_work_size[1] = {128};

  for (int i = 0; i < numFrames; i++) { 
    oclMat* curMat = new oclMat(src[i]);
    oclMat* destMat = new oclMat();
    size_t global_work_size[1] = {curMat.cols*curMat.rows};
    err = clSetKernelArg(toGray, 0, sizeof(cl_mem), (void *)&curMat.data);
    CHK_ERR(err); 
    err = clSetKernelArg(toGray, 1, sizeof(cl_mem), (void *)&destMat.data);
    CHK_ERR(err);
    err = clSetKernelArg(toGray, 2, sizeof(int), &curMat.rows);
    CHK_ERR(err);
    err = clSetKernelArg(toGray, 3, sizeof(int), &curMat.cols);
    CHK_ERR(err);
    err = clSetKernelArg(toGray, 4, sizeof(int), &curMat.step);
    CHK_ERR(err);
    err = clSetKernelArg(toGray, 5, sizeof(int), curMat.oclchannels());
    CHK_ERR(err);
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
    src[i] = destMat.Mat();
    curMat.release();
    destMat.release();
  }
}



