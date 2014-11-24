#ifndef BALL_H
#define BALL_H

#include<string>
#include<iostream>
#include "utils/LinkedList.cpp"

using namespace std;

struct edge;
struct candidate;
struct node;
struct frame;

// NODE STRUCT
struct node{
	int frameNum;
	int candidateNum;
	bool isStart = false; //Set this for node S
	bool isSink = false;	 //Set this for node T
	int numEdges;
	candidate* cand;
	edge* edgeList;

	// bool operator==(const node &other) const
	//   { return (frameNum == other.frameNum
	//             && candidateNum == other.candidateNum);
	//   }
	  
	node(){
		this->frameNum = -1;
		this->candidateNum = -1;
		this->numEdges = 0;
		this->cand = NULL;
		this->edgeList = NULL;
	}  

	node(int fnum, int cnum, candidate* c){
		this->frameNum = fnum;
		this->candidateNum = cnum;
		this->cand = c;
		this->numEdges = 0;
		this->edgeList = NULL;
	}

	// You can print a node by doing cout << [node_var] and it will print (frameNum,candidateNum)
	friend ostream& operator<<(ostream& o, const node& n){
		o << "(" << n.frameNum << "," << n.candidateNum << ")";
		return o;
	}
};

// EDGE STRUCT
struct edge{
	double weight;
	node* start;
	node* end;

	edge(){
		weight = 0;
		start = NULL;
		end = NULL;
	}

	edge(node* s, node* e, double w){
		weight = w;
		start = s;
		end = e;
	}
};

// CANDIDATE STRUCT
struct candidate{
	int x;
	int y;
	int radius;
	double probability;

	candidate(){
		this->x = 0;
		this->y = 0;
		this->radius = 0;
		this->probability = 0;
	}

	candidate(int x, int y, int radius, double prob){
		this->x = x;
		this->y = y;
		this->radius = radius;
		this->probability = prob;
	}
};

// FRAME Struct
struct frame{
	int numCandidates;
	candidate* candidateList;
	node* nodes;
};

// frame* listOfFrames;
// candidate* listofCandidates;

// Input: Array of all the frames (each has array of candidates for that frame), and number of frames
// Output: the source node of the graph (ie the start point)
node* composeGraph(frame*, int);

// Input: The filename for the jpeg and thresholds (to be decided)
// Output: An array of frames where each frame contains the candidate positions for that frame
frame* ballDetect(string jpg /*, [threshholds]*/);

// Input: the graph of frame candidates for every frame + number of vertices in the graph
// Output: a path where the returned pointer is the first node in the path and each node has only one edge out
// 		   there will be one node per frame representing which candidate we selected as the ball (cand pointer)
LinkedList<node*> shortestPath(node* graph, int numVerts);

// Input: the trajectory path in the form of a nodes array, also the source and output string names for the mp4 videos
// Output: output mp4 video with the candidates drawn in a bounding box
void visualize(node* trajectory, string srcVidMp4, string outputVidName);

// Does all the plumbing
int main(int argc, char* argv[]);

#endif
