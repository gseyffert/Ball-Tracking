/**
 * Frees all the memory associated with a graph (edges and nodes)
 * After this is run all the frame->nodes pointers are null for each frame;
*/

#include "../BallTracking.h"
#include <iostream>

// Starts at end of frame array and frees nodes for that frame then frees edges that pointed to those nodes
void freeGraph(frame* listOfFrames, node* graph, int numFrames){
    cout << "Starting graph freeing" << endl;

    // Start by freeing the sink node, first get the sink node
    edge* lastFrameEdgeList = listOfFrames[numFrames-1].nodes[0].edgeList;
    if(lastFrameEdgeList == NULL){
        cout << "Error: The first node in the last frame has a null edgelist when it should point to the sink node" << endl;
        return;
    }
    node* sink = lastFrameEdgeList[0].end;

    if(!sink->isSink){
        cout << "Error: Sink node doesnt have isSink flag set, maybe getting the wrong node?" << endl;
        cout << *sink << endl;
    }

    // Free the sink node
    delete sink;

    // Start from end frame
    for(int i=numFrames-1; i>=0; i--){
        frame* curFrame = &listOfFrames[i];
        int numCandidates = curFrame->numCandidates;
        cout << "Current frame: " << i << ", num nodes to free: " << numCandidates << endl;

        // go through each node and first free all the edges out of it
        for(int j=0; j<numCandidates; j++){
            node* curNode = &(curFrame->nodes[j]);
            cout << "current node: " << *curNode << endl;
            // free each edge leaving that node before freeing the node itself
            cout << "freeing edges" << endl;
            delete curNode->edgeList;
            // for(int k=0; k<curNode->numEdges; k++){
            //     cout << "freeing edge num " << k << endl;
            //     edge* curEdge = &(curNode->edgeList[k]);
            //     delete curEdge;
            // }
        }
        cout << "Freeing all nodes in the frame" << endl;
        delete curFrame->nodes;
        curFrame->nodes = NULL;
        cout << "Successfully freed all nodes in frame " << i << endl;
        cout << "\n" << endl;
    }

    // Now need to free source and all edges out of it
    cout << "Freeing source node" << endl;
    node* source = graph;
    if(!source || !(source->isStart)){
        cout << "Error: This may not be the correct source node" << endl;
        cout << "Source node: " << *source << endl;
        return;
    }
    delete source->edgeList;
    delete source;
    cout << "Done freeing source" << endl;
    // for(int i=0; i<source->numEdges; i++){
    //     cout << "Freeing edge " << i << endl;
    //     edge* cur
    // }
    graph = NULL;
}