// Shortest Path Module
// 
// Computes the shortest path of a DAG from the source S to every other node
// Here each node represents a candidate ball in a frame and candidate balls in one frame have edges to every candidate ball in the next frame
// The start node (ie the node pointed to by graph) is not a candidate but it has edges to all the candidates in frame 1.
// See the CandidateGraphModel.jpg for an illustration.

#include "../BallTracking.h"
#include <iostream>

node* shortestPath(node* graph){

}