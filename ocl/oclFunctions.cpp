#include <ocl.hpp>;
#include "clHelp.h";

using namespace cv;
using namespace cv::ocl;

void oclToGray(cl_command_queue &queue,
               cl_context &context,
               cl_kernel &toGray,
               Mat* src, 
               Mat* dest,
               int numFrames) {

  size_t global_work_size[1] = {} 
  size_t local_work_size[1] = {128};

  for (int i = 0; i < numFrames; i++) {
    oclMat* curMat = new oclMat(src[i]);
    oclMat* destMat = curMat;
    err = clSetKernelArg(toGray, 0, sizeof(cl_mem), (void *)&curMat.data);
    CHK_ERR(err); 
    err = clSetKernelArg(toGray, 1, sizeof(cl_mem), (void *)&destMat.data);
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
    src[i] = curMat.Mat();
  }
}

void oclBlur(cl_command_queue &queue,
             cl_context &context,
             cl_kernel gaussianBlur,
             Mat& src,
             int numFrames) {



}



