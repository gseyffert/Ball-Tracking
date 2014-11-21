/**
 * Composes Graph from individual frames
 */

#include "../BallTracking.h"

using namespace std;

/**
 * Builds a graph out of the candidates of each frame in the frame array
 * @param  frameArray pointer to first element in the array
 * @param  numFrames  number of frames (for bounds checking)
 * @return            a pointer to the first node in the graph
 */
node* composeGraph(frame* frameArray, int numFrames){
    // The source node of the graph (doesnt represent a frame), links to all candidates in the first frame
    node* source = new node();
    source->isStart = true;

    // At the beginning the last frame is the source
    node* lastFrame = source;

    // RIGHT NOW THIS ONLY WORKS FOR THE FIRST FRAME SINCE THERE IS ONLY ONE NODE (the source) BEFORE FRAME 0
    // FOR OTHER FRAMES WE NEED TO MAKE AN EDGE BETWEEN EVERY NODE IN THE PREVIOUS FRAME AND THE CURRENT FRAME

    // Go through each frame
    for(int i=0; i < numFrames; i++){
        frame* curFrame = &frameArray[i];
        int numCandidates = curFrame->numCandidates;

        cout << "Frame " << i << " has " << numCandidates << " ball candidates." << endl;

        // Allocate the edges that will point from the previous frame to this frame
        edge* edgesToThisFrame = new edge[numCandidates];
        lastFrame->numEdges = numCandidates;

        candidate* candidateArray = curFrame->candidateList;
        // Go through each candidate of that frame and create a node for it
        // Also set one of the edges from the last frame candidates to this candidate
        for(int j=0; j<numCandidates; j++){
            candidate* curCand = &candidateArray[j];
            cout << "    Candidate " << j << " coords:(" << curCand->x << "," << curCand->y << ") confidence:" << curCand->probability << endl;

            // Allocate node for this candidate
            node* thisCand = new node(i, j, curCand);
            // cout << "This cand: " << *thisCand << endl;

            edge* edgeToThisCand = &edgesToThisFrame[j];
            edgeToThisCand->start = lastFrame;
            edgeToThisCand->end = thisCand;
            edgeToThisCand->weight = curCand->probability;

            cout << "    created edge (node) " << *(edgesToThisFrame[j].start) << " to (node) " << *(edgesToThisFrame[j].end) << endl << endl;
        }

        // Set the edge list of the previous frame's nodes
        lastFrame->edgeList = edgesToThisFrame;
    }
    return NULL;
}

/**
 * Test Code
 * @return 0
 */
int main(int argc, char* argv[]){
    frame* f = new frame;

    candidate* candidates = new candidate[4];
    for(int i=0; i < 4; i++){
        candidate* c = &candidates[i];
        c->x = i;
        c->y = i;
        c->probability = i;
        c->radius = i;
    }

    f->candidateList = candidates;
    f->numCandidates = 4;

    composeGraph(f, 1);
    return 0;
}