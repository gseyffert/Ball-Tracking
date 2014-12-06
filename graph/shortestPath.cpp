// Shortest Path Module
// 
// Computes the shortest path of a DAG from the source S to every other node
// Here each node represents a candidate ball in a frame and candidate balls in one frame have edges to every candidate ball in the next frame
// The start node (ie the node pointed to by graph) is not a candidate but it has edges to all the candidates in frame 1.
// The graph should also end in a sink node T which is not a candidate either but facilitates finding the shortest path.
// See the CandidateGraphModel.png for an illustration.

#include "../BallTracking.h"
#include <iostream>
#include <unordered_map> //hash table
#include <limits> //contains maxInt limit
#include <utility> //contains pair

//////////////////////////////////////////////
// Hash function for hashing a pair of ints //
//////////////////////////////////////////////
struct pairhash{
public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const
  {
    std::size_t xHash = std::hash<T>()(x.first);
    std::size_t yHash = std::hash<U>()(x.second);
    std::size_t finalHash = 0.5*(xHash+yHash)*(xHash+yHash+1)+yHash;
    return finalHash;
  }
};

using namespace std;

///////////////////////////////
// The recursive part of DFS //
///////////////////////////////
void recurseDFS(node* curNode, LinkedList<node*> &linearizedOrder, unordered_map<pair<int,int>,bool,pairhash> &visited){
    pair<int,int> key (curNode->frameNum, curNode->candidateNum);

    if(visited.count(key) == 0){ //curNode is not in hashTable = has not been visited yet
        // cout << "visiting node " << key.first << "," << key.second << endl;
        visited[key] = true; //mark node as visited
        for(int i=0; i<curNode->numEdges; i++){
            node* endpoint = curNode->edgeList[i].end; //get the endpoint of the edge out of curNode
            recurseDFS(endpoint, linearizedOrder, visited);
        }
        // Add curNode to the head of the linearized order
        linearizedOrder.insertFront(curNode);
    }
}

//////////////////////////////////////////////////////////
// Run DFS to linearize the DAG                         //
// The way we linearize is by doing a topological sort  //
//////////////////////////////////////////////////////////
void DFS(node* graph, LinkedList<node*> &linearizedOrder){
    unordered_map<pair<int,int>, bool, pairhash> visited;
    recurseDFS(graph, linearizedOrder, visited);
}

//////////////////////////////////////////
// The shortest path algorithm for DAGs //
//////////////////////////////////////////
LinkedList<node*>* shortestPath(node* graph, int numVerts){
    // Nodes are hashed by their frame num and candidate num
    unordered_map<pair<int,int>,double, pairhash> dist;
    unordered_map<pair<int,int>,node*, pairhash> prev; //prev[n] = the node that comes before node n in the shortest path from s to t
    LinkedList<node*> linearizedOrder;

    // There should be numVerts nodes in the graph so size hashtable appropriately
    dist.reserve(numVerts);
    prev.reserve(numVerts);

    cout << "Linearizing graph" << endl;
    // Linearize the graph
    DFS(graph, linearizedOrder);

    // Init dist and prev
    LLNode<node*>* cur = linearizedOrder.getNode(0); //get the head of the list
    while(cur != NULL){
        node* curNode = cur->item;
        pair<int,int> key(curNode->frameNum, curNode->candidateNum);
        dist[key] = numeric_limits<int>::max();
        prev[key] = NULL;
        cur = cur->next;
    }

    cout << "Finding shortest path" << endl;
    //reset to head
    cur = linearizedOrder.getNode(0);
    // Make distance from start to itself 0
    node* start = cur->item;
    pair<int,int> key (start->frameNum, start->candidateNum);
    dist[key] = 0;
    // Go through in topologically sorted order and compute min path
    while(cur != NULL){
        node* curNode = cur->item;
        pair<int,int> key(curNode->frameNum, curNode->candidateNum);
        // cout << "processing node " << key.first << "," << key.second << endl;

        for(int i=0; i<curNode->numEdges; i++){
            edge thisEdge = curNode->edgeList[i];
            node* dstNode = thisEdge.end;
            pair<int,int> dstKey (dstNode->frameNum, dstNode->candidateNum);

            // update shortest path to dstNode by going through curNode
            if(dist[dstKey] > dist[key]+thisEdge.weight){
                dist[dstKey] = dist[key] + thisEdge.weight;
                prev[dstKey] = curNode;
            }
        }
        cur = cur->next;
    }

    LinkedList<node*>* output = new LinkedList<node*>;
    // Now output the shortest path
    cur = linearizedOrder.getNode(linearizedOrder.length()-1); //Get sink
    node* curNode = cur->item;
    while(curNode != NULL){
        pair<int,int> key (curNode->frameNum, curNode->candidateNum);
        if (curNode->isStart) break;
        output->insertFront(curNode);
        curNode = prev[key];
    }    
    cout << "Done with shortest path" << endl;
    return output;
}