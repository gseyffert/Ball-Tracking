#ifndef BALL_H
#define BALL_H

#include<string>
#include<iostream>
#include "opencv2/opencv.hpp"
#include "utils/LinkedList.cpp"
#include <emmintrin.h>
#include <xmmintrin.h>

using namespace std;
using namespace cv;


//////////////////////////
// Optimization Types ////
//////////////////////////
const int OPEN_CV = 0;
const int NAIVE = 1;
const int OPTIMIZED = 2;

struct edge;
struct candidate;
struct node;
struct frame;

// NODE STRUCT
typedef struct node{
	bool isStart; //Set this for node S
	bool isSink;	 //Set this for node T
	int frameNum;
	int candidateNum;
	candidate* cand;
	int numEdges;
	edge* edgeList;
	  
	node(){
		this->isStart = false;
		this->isSink = false;
		this->frameNum = -1;
		this->candidateNum = -1;
		this->cand = NULL;
		this->numEdges = 0;
		this->edgeList = NULL;
	}  

	node(int fnum, int cnum, candidate* c){
		this->isStart = false;
		this->isSink = false;
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
} node;

// EDGE STRUCT
typedef struct edge{
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
} edge;

// CANDIDATE STRUCT
typedef struct candidate{
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
	
	friend ostream& operator<<(ostream& o, const candidate& c){
		o << "(" << c.x << "," << c.y << "," << c.radius << "," << c.probability << ")";
		return o;
	}
} candidate;

// FRAME Struct
typedef struct frame {
	int numCandidates;
	candidate* candidateList;
	node* nodes; //Created & populated when the graph is composed
} frame;

// frame* listOfFrames;
// candidate* listofCandidates;

// Input: Array of all the frames (each has array of candidates for that frame), and number of frames
// Output: the source node of the graph (ie the start point)
node* composeGraph(frame*, int);

node* composeGraphOptimized(frame*, int);

// Input: The filename for the jpeg and thresholds (to be decided)
// Output: An array of frames where each frame contains the candidate positions for that frame
void detectBall(Mat, candidate*, int type, int*);

// Input: the graph of frame candidates for every frame + number of vertices in the graph
// Output: a path where the returned pointer is the first node in the path and each node has only one edge out
// 		   there will be one node per frame representing which candidate we selected as the ball (cand pointer)
LinkedList<node*>* shortestPath(node* graph, int numVerts);

// Input: the trajectory path in the form of a nodes array, also the source and output string names for the mp4 videos
// Output: output mp4 video with the candidates drawn in a bounding box
void visualize(LinkedList<node*>* trajectory, string srcVidMp4, int length);

// Frees all the memory allocated when we compose the graph
// Input: the array of frames and number of frames
// Output: Nothing, all the fames->nodes pointers are deallocated and set to null
void freeGraph(frame* listOfFrames, node* graph, int numFrames);

/**
 * Edge weight heuristic which computes the euclidian distance between the end and start node
 * Therefore transitions between candidates that are close together will be prerferred by the shortest path algorithm
 * Also multiply by the reciprocal of the smallest confidence level 
 * (ie a high confidence level will decrease this value, a low confidence level will increase it)
 * @param  startNode pointer to the source node of the edge
 * @param  endNode   pointer to the destination node of the edge
 * @return           a decimal edge weight
 */
double euclidianDistHeuristic(node* startNode, node* endNode);

/**
 * Creates a video using ffmpeg from a sequence of images, need to pass in the pattern of the image file names using %d to indicate sequence numbers
 * requires having ffmpeg installed and in the system path.
 * Ex: runffmpeg("test\\putts\\%d.jpg", "test.mp4", "200", "720x400", "24");
 * or runffmpeg("test\\putts\\%d.jpg", "test.mp4", "200"); which uses the default image size and framerate
 * 
 * @param  filePattern ex: img%d.jpg if images are named img1.jpg, img2.jpg,...,imgn.jpg
 * @param  outName     the name of the output video
 * @param  startNum    the number that the frames start at, ie if the first image is img100jpg then it should be 100
 * @param  size        Resize the images before converting to a video, ie "720x400", if empty string then no resizinge (OPTIONAL)
 * @param  framerate   Set the framerate, if -1 then use default ffmpeg framerate (OPTIONAL)
 * @return             the return code of ffmpeg, 0 is good, anything else indicates something went wrong.
 */
int runffmpeg(string filePattern, string outName, string startNum, string size="", string framerate="");

// main.cpp Does all the plumbing
#endif
