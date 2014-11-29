/**
 * Composes Graph from individual frames
 */

#include "../BallTracking.h"
#include <cmath> //sqrt, pow

using namespace std;

/**
 * Builds a graph out of the candidates of each frame in the frame array
 * @param  frameArray pointer to first element in the array
 * @param  numFrames  number of frames (for bounds checking)
 * @return a pointer to the first node in the graph
 */
node* composeGraph(frame* frameArray, int numFrames){
    // The source node of the graph (doesn't represent a frame), links to all candidates in the first frame
    node* source, sink = new node();
    sink->isSink = true;
    sink->edgeList = new edge[frameArray[numFrames-1]->numCandidates]
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
            curFrame->nodes[j].frameNum = i;
            curFrame->nodes[j].candNum = j;
            curFrame->nodes[j].cand = curCand;
            node* curNode = &(curFrame->nodes[j]);
            
            //Initialize edgeList for current node
            curNode->edgeList = new edge[prevFrame->numCandidates];

            //Link to source if first frame
            if (i == 0) {
                edge* temp = new edge(source, curNode, curNode->candidate->probability); //for the edges from the source node we just weight with the confidence level of the candidate
                source->edgeList[j] = temp;
            }
            //Set edges from the prev frame's nodes to start at itself end at this candidate's node if not first frame
            else {
                prevNodes = prevFrame->nodes;
                for (int k = 0; k < numPrevCandidates; k++) {
                    node* oldNode = &prevNodes[k];
                    curNode->edgeList[k].start = oldNode;
                    curNode->edgeList[k].end = curNode;
                    curNode->edgeList[k].weight = euclidianDistHeuristic(oldNode, curNode); //update w/ appropriate heuristic function
                    if (i == numFrames - 1) {
                        sink->edgeList[k].start = curNode;
                        sink->edgeList[k].end = sink;
                    }
                }
            }
        }
    }
    return source;
}

/**
 * Edge weight heuristic which computes the euclidian distance between the end and start node
 * Therefore transitions between candidates that are close together will be prerferred by the shortest path algorithm
 * Also multiply by the reciprocal of the smallest confidence level 
 * (ie a high confidence level will decrease this value, a low confidence level will increase it)
 * @param  startNode pointer to the source node of the edge
 * @param  endNode   pointer to the destination node of the edge
 * @return           a decimal edge weight
 */
double euclidianDistHeuristic(node* startNode, node* endNode){
    double val;
    candidate* startCand = startNode->candidate;
    candidate* endCand = endNode->candidate;
    val = pow((endCand->x - startCand->x),2);
    val = val + pow((endCand->y - startCand->y),2);
    val = sqrt(val);
    double confidenceLevel = min(startCand->probability, endCand->probability);
    confidenceLevel = 1/confidenceLevel;
    val = val * confidenceLevel;
    return val;
}

/**
 * Test Code
 * @return 0
 */
int main(int argc, char* argv[]){
    frame* f = new frame[1000];

    candidate* candidates = new candidate[4];
    for(int i = 0; i < 4; i++){
        candidate* c = &candidates[i];
        c->x = i;
        c->y = i;
        c->probability = i;
        c->radius = i;
    }

    for (int i = 0; i < 1000; i++) {
        f[i]->candidateList = candidates;
        f[i]->numCandidates = 4;
    }
    
    t1 = timestamp();
    node* curNode = composeGraph(f, 1);
    t1 = timestamp() - t1;

    for (int i = 0; i < 1000; i++) {
        edge* tmp = curNode->edgeList;
        for (int j = 0; j < curNode->numEdges; j++) {
            tmp 
        }
    }
    
    return 0;
}
