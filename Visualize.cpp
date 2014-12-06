#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <cstdio>
#include <ctime>
#include "BallTracking.h"

////////////////////////
// HELPER FUNCTIONS ////
////////////////////////
String intToString(int number){
    stringstream ss;
    ss << number;
    return ss.str();
}

void draw(Mat& src, node* myNode)
{

    candidate* cand = myNode->cand;
    int x = cand->x;
    int y = cand->y;
    int radius = cand->radius;
    printf("X: %d Y: %d Radius: %d \n", x, y, radius);

    Point center(x, y);
    // circle center
    circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
    // circle outline
    circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );

}


void visualize(LinkedList<node*>* trajectory, string srcVidMp4){
    /**
     * 
     */

    VideoCapture cap(srcVidMp4); // open the specified video

    if(!cap.isOpened()) { // check if we succeeded
        printf("Video File not found!\n");
        return;
    }
    int LEN_VIDEO = cap.get(CAP_PROP_FRAME_COUNT);

    /////////////    VIDEO WRITER   ////////////////////////
    // string::size_type pAt = srcVidMp4.find_last_of('.');                  // Find extension point
    // const string NAME = srcVidMp4.substr(0, pAt) + "-Tracked" + ".avi";   // Form the new name with container
    // int ex = static_cast<int>(cap.get(CAP_PROP_FOURCC));     // Get Codec Type- Int form

    // // Transform from int to char via Bitwise operators
    // char EXT[] = {(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0};

    // Size S = Size((int) cap.get(CAP_PROP_FRAME_WIDTH),    // Acquire input size
    //               (int) cap.get(CAP_PROP_FRAME_HEIGHT));

    // // VideoWriter outputVideo; // Open the output
    // // outputVideo.open(NAME, ex, cap.get(CAP_PROP_FPS), S, true);
    // VideoWriter outputVideo("out.avi",static_cast<int>(cap.get(CAP_PROP_FOURCC)),10, S,true);
    // if (!outputVideo.isOpened())
    // {
    //     cout  << "Could not open the output video for write: ";
    //     return;
    // }

    Mat currFrame;
    vector<Mat> spl;
    int i = 0;
    node* myNode;
    string picName;
    for(;;) //Show the image captured in the window and repeat
    {
        cap >> currFrame; // read
        if (currFrame.empty()){  // check if at end
            cout  << "Frame Empty\n";
            break;
        }
        if (i >= LEN_VIDEO){
            cout  << "Video size and Num Nodes dont match";
            break;
        }
        myNode = trajectory->getNode(i)->item;

        draw(currFrame, myNode);

        //save
        // outputVideo.write(currFrame);
        // outputVideo << currFrame;

        picName = "output/" +  intToString(i) + ".jpg";
        imwrite(picName, currFrame);
        i++;
    }

    cout << "Finished drawing\n";
}