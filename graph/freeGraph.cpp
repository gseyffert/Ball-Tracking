/**
 * Frees all the memory associated with a graph (edges and nodes)
 * After this is run all the frame->nodes pointers are null for each frame;
*/

#include "../BallTracking.h"
#include <iostream>

// Starts at end of frame array and frees nodes for that frame then frees edges that pointed to those nodes
void freeGraph(frame* listOfFrames, node* graph, int numFrames){
    cout << "Starting graph freeing" << endl;
    // Start from end frame
}