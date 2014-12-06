/**
 * Composes Graph from individual frames Parallelized w/ openMP
 * We split composing into two parallelized steps
 * 1. create all the nodes in the frames in parallel
 * 2. link the nodes appropriately
 */

#include "../BallTracking.h"
#include <cmath> //sqrt, pow
#include <omp.h> //openMp

using namespace std;

/**
 * Builds a graph out of the candidates of each frame in the frame array
 * @param  frameArray pointer to first element in the array
 * @param  numFrames  number of frames (for bounds checking)
 * @return a pointer to the first node in the graph
 */
node* composeGraph(frame* frameArray, int numFrames){
    // Create and initialize the source and sink nodes
    node* source = new node();
    source->isStart = true;
    source->numEdges = frameArray[0].numCandidates;
    source->edgeList = NULL; //Gets created when we process frame0 and create nodes for frame 0

    node* sink = new node();
    sink->isSink = true;
    sink->edgeList = NULL; //Sink has no outgoing edges
    sink->numEdges = 0;

    int NUM_THREADS = omp_get_max_threads();
    candidate* curCand;
    node* curNode;
    int i,k,numEdges;

    omp_set_num_threads(NUM_THREADS);

    // First create all the nodes in parallel
    #pragma omp parallel for private(i, k, numEdges, curNode) schedule(dynamic)
    for (i = 0; i < numFrames; i++) {
        //allocate node array in frame
        frameArray[i].nodes = new node[frameArray[i].numCandidates];
        
        //assign node fields
        for (k = 0; k < frameArray[i].numCandidates; k++) {
            curNode = &(frameArray[i].nodes[k]);
            //# edges for curNode = number of candidates in next frame
            //Only 1 edge for nodes in last frame (points to sink)
            if (i == numFrames - 1) {
                numEdges = 1;
            } else {
                numEdges = frameArray[i+1].numCandidates; 
            }
            curNode->frameNum = i;
            curNode->candidateNum = k;
            curNode->cand = &(frameArray[i].candidateList[k]);
            curNode->numEdges = numEdges;
            curNode->edgeList = new edge[numEdges];
        }
    }

    // check to make sure everything got allocated properly
    for (int j = 0; j<numFrames; j++) {
        int numCands = frameArray[j].numCandidates;
        if(!frameArray[j].nodes){
            cout << "Node was not correctly allocated for idx " << j << endl;
            return NULL;
        }
        // If this causes a segfault then the array was allocated an incorrect size
        for(int k = 0; k<numCands; k++)
            frameArray[j].nodes[k];
    }

    cout << "Looks like it got allocated properly" << endl;

    //Initialize loop variables
    frame* curFrame, *prevFrame;

    // Number of candidates to walkthrough in the current and previous frames
    int numPrevCandidates, numCandidates;

    // Go through each frame
    for(int fNum = 0; fNum < numFrames; fNum++) {
        // cout << "Doing frame " << fNum << endl;
        curFrame = &frameArray[fNum];
        numCandidates = curFrame->numCandidates;
        // Dealing with frame->frame edges (ie not source->frame)
        if (fNum != 0) {
            // Get prev frames
            prevFrame = &frameArray[fNum-1];
            numPrevCandidates = prevFrame->numCandidates;
        }else // Dealing with source->frame edges
        {
            // We are going to link the source to the first frame so there is no prev Frame
            prevFrame = NULL;
            // There is only one source node so we set numPrevCandidates=1
            numPrevCandidates = 1;
        }
        
        candidate* candidateArray = curFrame->candidateList;

        // walkthrough the previous frames nodes and link them to this frames nodes
        for(int prevCandNum = 0; prevCandNum < numPrevCandidates; prevCandNum++){
            // cout << "connecting prevCand " << prevCandNum << endl;
            node* prevFrameNode;
            // Handle the first frame where we are linking source node to the nodes in the first frame
            if(fNum == 0){
                prevFrameNode = source;
            }else{
                prevFrameNode = &(prevFrame->nodes[prevCandNum]);
            }
            // Set num edges to be the number of candidates in the current frame
            prevFrameNode->numEdges = numCandidates;
            // Allocate the edgelist for prev frame nodes
            prevFrameNode->edgeList = new edge[numCandidates];

            // Go through each candidate of current frame and create a node for it
            for(int curCandNum = 0; curCandNum < numCandidates; curCandNum++){
                // cout << "to cur cand num " << curCandNum << endl;
                // Get a pointer to this node so that we can more easily make edges to it
                node* curNode = &(curFrame->nodes[curCandNum]);

                // Link the prev frame node to all the current nodes by setting edge fields
                prevFrameNode->edgeList[curCandNum].start = prevFrameNode;
                prevFrameNode->edgeList[curCandNum].end = curNode;

                // If the edge is from source->frame then source won't have a candidate so just 
                //use the inverse of the confidence as the edge weight (smaller is better)
                if(fNum == 0){
                    double invConfidence = 1/(curNode->cand->probability);
                    prevFrameNode->edgeList[curCandNum].weight = invConfidence;
                }
                else{
                    prevFrameNode->edgeList[curCandNum].weight = euclidianDistHeuristic(prevFrameNode, curNode);
                }
            }
            // cout << "-----" << endl << endl;
        }
        // cout << "########" << endl << endl;
    }
    // cout << "Connecting sink node" << endl;

    // get last frame
    frame* lastFrame = &(frameArray[numFrames-1]);
    numPrevCandidates = lastFrame->numCandidates;
    
    // Now link the last frame's nodes to the sink node
    for(int prevCandNum = 0; prevCandNum < numPrevCandidates; prevCandNum++){
        node* prevNode = &(lastFrame->nodes[prevCandNum]);
        // connect the prevNode to the sink node
        prevNode->edgeList = new edge[1];
        prevNode->edgeList[0].start = prevNode;
        prevNode->edgeList[0].end = sink;
        double invConfidence = 1/(prevNode->cand->probability);
        prevNode->edgeList[0].weight = invConfidence;
    }

    cout << "Done building graph" << endl;

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
    candidate* startCand = startNode->cand;
    candidate* endCand = endNode->cand;
    val = pow((endCand->x - startCand->x),2);
    val = val + pow((endCand->y - startCand->y),2);
    val = sqrt(val);
    double confidenceLevel = min(startCand->probability, endCand->probability);
    confidenceLevel = 1/confidenceLevel;
    val = val * confidenceLevel;
    return val;
}
