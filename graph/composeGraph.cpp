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
    source->numEdges = frameArray[0]->numCandidates;
    source->edgeList = new edge[source->numEdges]

    // At the beginning the last frame is the source
    node* lastFrame = source;

    // RIGHT NOW THIS ONLY WORKS FOR THE FIRST FRAME SINCE THERE IS ONLY ONE NODE (the source) BEFORE FRAME 0
    // FOR OTHER FRAMES WE NEED TO MAKE AN EDGE BETWEEN EVERY NODE IN THE PREVIOUS FRAME AND THE CURRENT FRAME


    //Initialize variables
    frame* curFrame, prevFrame = &frameArray[0];
    int numCandidates, numPrevCandidates = 1;
    candidate* curCand, prevCand, candidateArray;

    // Go through each frame
    for(int i = 0; i < numFrames; i++) {
        if (i != 0) {
            curFrame = &frameArray[i];
            numCandidates = curFrame->numCandidates;
            prevFrame = frameArray[i-1];
            numPrevCandidates = prevFrame->numCandidates;
        }
        candidate* candidateArray = curFrame->candidateList;
        cout << "Frame " << i << " has " << numCandidates << " ball candidates." << endl;

        // Go through each candidate of current frame and create a node for it
        // Also set edges from the last frame candidates to this candidate
        prevCands = prevFrame->
        for(int j = 0; j < numCandidates; j++) {
            //cout << "    Candidate " << j << " coords:(" << curCand->x << "," << curCand->y << ") confidence:" << curCand->probability << endl;
            //cout << "    created edge (node) " << *(edgesToThisFrame[j].start) << " to (node) " << *(edgesToThisFrame[j].end) << endl << endl;
            
            curCand = &candidateArray[j];
            edge* edgesToThisCand = new edge[prevCandidates];
            
            
            //Link source to first frame
            if (i = 0) {
                edge temp = new edge(source, );
                source->edgeList[j] = temp;
                
            } else {
                

                
            
                edge* edgeToThisCand = &edgesToThisFrame[j];
                edgeToThisCand->start = lastFrame;
                edgeToThisCand->end = thisCand;
                edgeToThisCand->weight = curCand->probability;
                
            }
           
        }

        // Set the edge list of the previous frame's nodes
        lastFrame->edgeList = edgesToThisFrame;
    }
    return source;
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
