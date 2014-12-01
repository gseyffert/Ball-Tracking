/** Shortest Path Test Code
    compile with shortestPath.cpp **/

#include "../BallTracking.h"
#include <iostream>

using namespace std;

//Test Code
int main(){
    cout << "Creating graph" << endl;
    // Create Source node
    node* s = new node;
    s->isStart = true;
    s->frameNum = 0;
    s->candidateNum = 0;
    s->numEdges = 2;

    // Create 2 nodes for first frame
    node* l1Nodes = new node[2];
    l1Nodes[0].frameNum = 1;
    l1Nodes[1].frameNum = 1;
    l1Nodes[0].candidateNum = 1;
    l1Nodes[1].candidateNum = 2;
    l1Nodes[0].numEdges = 2;
    l1Nodes[1].numEdges = 2;

    // Create edges between s and both nodes of frame 1
    edge* sEdges = new edge[2];
    sEdges[0].start = s;
    sEdges[1].start = s;
    sEdges[0].end = &l1Nodes[0];
    sEdges[1].end = &l1Nodes[1];
    sEdges[0].weight = 4;
    sEdges[1].weight = 2;

    s->edgeList = sEdges;

    // Create second frame nodes
    node* l2Nodes = new node[2];
    l2Nodes[0].frameNum = 2;
    l2Nodes[1].frameNum = 2;
    l2Nodes[0].candidateNum = 1;
    l2Nodes[1].candidateNum = 2;
    l2Nodes[0].numEdges = 1;
    l2Nodes[1].numEdges = 1;

    // Create the 4 edges between frame 1 and 2
    edge* l1Edges = new edge[4];
    l1Edges[0].start = &l1Nodes[0];
    l1Edges[1].start = &l1Nodes[0];
    l1Edges[2].start = &l1Nodes[1];
    l1Edges[3].start = &l1Nodes[1];
    l1Edges[0].end = &l2Nodes[0];
    l1Edges[1].end = &l2Nodes[1];
    l1Edges[2].end = &l2Nodes[0];
    l1Edges[3].end = &l2Nodes[1];
    // set weights
    l1Edges[0].weight = 3;
    l1Edges[1].weight = 5;
    l1Edges[2].weight = 2;
    l1Edges[3].weight = 1;

    l1Nodes[0].edgeList = &l1Edges[0];
    l1Nodes[1].edgeList = &l1Edges[2];

    // Create node T
    node* t = new node;
    t->frameNum = -1;
    t->candidateNum = -1;
    t->numEdges = 0;
    t->edgeList = NULL;

    // Create edges between nodes of frame 2 and t
    edge* l2Edges = new edge[2];
    l2Edges[0].start = &l2Nodes[0];
    l2Edges[1].start = &l2Nodes[1];
    l2Edges[0].end = t;
    l2Edges[1].end = t;
    l2Edges[0].weight = 6;
    l2Edges[1].weight = 2;

    l2Nodes[0].edgeList = &l2Edges[0];
    l2Nodes[1].edgeList = &l2Edges[1];

    cout << "Running DFS" << endl;
    // LinkedList lin;
    // DFS(s, lin);
    LinkedList<node*>* o = shortestPath(s, 6);
    cout << "\nfinal path:" << endl;
    LLNode<node*>* cur = o->getNode(0);
    while(cur != NULL){
        node* c = cur->item;
        cout << *c << endl;
        cur = cur->next;
    }

    // cout << "num verts (should be 6): " << lin.length() << endl;
    // cout << "[";
    // LLNode* cur = lin.getNode(0);
    // while(cur != NULL){
    //     cout << cur->item->frameNum << " ";
    //     cur = cur->next;
    // }
    // cout << "]" << endl;

    // Free all the memory
    delete s;
    delete t;
    delete[] l1Nodes;
    delete[] l2Nodes;
    delete[] sEdges;
    delete[] l1Edges;
    delete[] l2Edges;
    return 0;
}