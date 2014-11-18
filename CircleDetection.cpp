#include <stdio.h>
#include <sstream>
#include <string>
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;
/** @function main */
int main(int argc, char** argv)
{
  Mat src, src_gray, HSV;

  /// Read the image
  src = imread( argv[1], 1 );

  if( !src.data )
    { return -1; }

  /// Convert it to gray
  cvtColor( src, src_gray, COLOR_BGR2GRAY );

  /// Reduce the noise so we avoid false circle detection
  GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );

  vector<Vec3f> circles;

  cvtColor(src,HSV,COLOR_BGR2HSV);

  /// Apply the Hough Transform to find the circles
  HoughCircles( src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows/8, 200, 100, 1, 400);
  // HoughCircles( src, circles, HOUGH_GRADIENT, 1, src_gray.rows/8, 200, 100, 1, 400);

  /// Draw the circles detected
  for( size_t i = 0; i < circles.size(); i++ )
  {
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      // circle center
      circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
      // circle outline
      circle( src, center, radius, Scalar(150,255,50), 3, 8, 0 );

      circle( src_gray, center, 3, Scalar(0,255,0), -1, 8, 0 );
      // circle outline
      circle( src_gray, center, radius, Scalar(150,255,50), 3, 8, 0 );
      printf("Circle at %f, %f with radius %f\n", circles[i][0], circles[i][1], circles[i][2]);
   }

  /// Show your results
  
  namedWindow( "Hough Circle Transform Demo", WINDOW_AUTOSIZE );
  imshow( "Hough Circle Transform Demo", src );
  imshow( "Grey", src_gray );
  imshow( "HSV", HSV );

  waitKey(0);
  return 0;
}