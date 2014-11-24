/**
 * Composes Graph from individual frames
 */

#include "../BallTracking.h"

using namespace std;

/**
 * Builds a graph out of the candidates of each frame in the frame array
 * @param  frameArray pointer to first element in the array
 * @param  numFrames  number of frames (for bounds checking)
 * @return a pointer to the first node in the graph
 */
node* composeGraph(frame* frameArray, int numFrames){
    // The source node of the graph (doesn't represent a frame), links to all candidates in the first frame
    node* source = new node();
    source->isStart = true;
    source->numEdges = frameArray[0]->numCandidates;
    source->edgeList = new edge[source->numEdges]

    //Initialize loop variables
    frame* curFrame, prevFrame = &frameArray[0];
    int numCandidates = curFrame->numCandidates; 
    int numPrevCandidates = 1;
    candidate* curCand, candidateArray;
    node* prevNodes;

    // Go through each frame
    for(int i = 0; i < numFrames; i++) {
        if (i != 0) {
            curFrame = &frameArray[i];
            numCandidates = curFrame->numCandidates;
            prevFrame = &frameArray[i-1];
            numPrevCandidates = prevFrame->numCandidates;
        }
        
        candidate* candidateArray = curFrame->candidateList;
        curFrame->nodes = new node[numCandidates];
        
        // Go through each candidate of current frame and create a node for it
        for(int j = 0; j < numCandidates; j++) {
            curCand = &candidateArray[j];
            
            //Allocate graph node for this candidate and add node to current frame's list of nodes
            node* curNode = new node(i, j, curCand);
            curFrame->nodes[j] = curNode;
            
            //Initialize edgeList for current node
            curNode->edgeList = new edge[prevFrame->numCandidates];
            
            //Link to source if first frame
            if (i = 0) {
                edge* temp = new edge(source, curNode, 0.0); //update 0.0 w/ appropriate heuristic function
                source->edgeList[j] = temp;
            }
            //Set edges from the prev frame's nodes to start at itself end at this candidate's node if not first frame
            else {
                prevNodes = prevFrame->nodes;
                for (int k = 0; k < numPrevCandidates; k++) {
                    node* oldNode = prevNodes[k];
                    edge* tempEdge = new edge(oldNode, curNode, 0.0);
                    curNode->edgeList[j] = tempEdge;
                }
            }
        }
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
    for(int i = 0; i < 4; i++){
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
