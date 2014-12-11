/** Makes a video out of a sequence of images 

    requires having ffmpeg installed and in the system path
**/

#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "BallTracking.h"

using namespace std;

/**
 * Creates a video using ffmpeg from a sequence of images, need to pass in the pattern of the image file names using %d to indicate sequence numbers
 * requires having ffmpeg installed and in the system path.
 * Ex: runffmpeg("test\\putts\\%d.jpg", "test.mp4", "200", "720x400", "24");
 * or runffmpeg("test\\putts\\%d.jpg", "test.mp4", "200"); which uses the default image size and framerate
 * 
 * @param  filePattern ex: img%d.jpg if images are named img1.jpg, img2.jpg,...,imgn.jpg
 * @param  outName     the name of the output video
 * @param  startNum    the number that the frames start at, ie if the first image is img100jpg then it should be 100
 * @param  size        Resize the images before converting to a video, ie "720x400", if empty string then no resizinge (OPTIONAL)
 * @param  framerate   Set the framerate, if -1 then use default ffmpeg framerate (OPTIONAL)
 * @return             the return code of ffmpeg, 0 is good, anything else indicates something went wrong.
 */
int runffmpeg(string filePattern, string outName, string startNum, string size/*=""*/, string framerate/*=""*/){
    bool defaultFR = false;
    bool defaultSize = false;

    if(atoi(startNum.c_str()) < 0){
        cout << "Must provide a starting image number greater than 0" << endl;
        return -1;
    }

    if(size == ""){
        defaultSize = true;
    }

    if(framerate == ""){
        defaultFR = true;
    }

    string command = "";
    command += "ffmpeg -y ";
    // Set the start num
    command += "-start_number " + startNum +" ";

    // Set the filename pattern
    command += "-i \"" + filePattern + "\" ";

    // Set the codec
    command += "-vcodec mpeg4 ";

    if(!defaultFR){
        command += "-framerate " + framerate + " ";
    }

    if(!defaultSize){
        command += "-s " + size + " ";
    }

    // Set the output name
    command += outName;

    cout << "executing command: " << endl;
    cout << command << endl << endl;

    string c = "ffmpeg -y -start_number 200 -i "+filePattern+" -vcodec mpeg4 "+outName;

    int retCode = system(command.c_str());

    if(retCode != 0){
        cout << endl << "ERROR: Something went wrong converting the images to video." << endl;
    }
    return retCode;
}

// int main(){
//     runffmpeg("test\\putts\\%d.jpg", "test.mp4", "200");
// }
