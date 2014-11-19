#ifndef BALL_H
#define BALL_H

#include<string>

// NODE STRUCT
typedef struct {
	int frameNum;	
	candidate* cand;
	edge* list;
} node;

// EDGE STRUCT
typedef struct {
	int weight;
	node* start;
	node* end;
} edge;

// CANDIDATE STRUCT
typedef struct  {
	int x;
	int y;
	int radius;
	int probability;
} candidate;

// FRAME Struct
typedef struct {
	candidate* list;
	uint64_t numCandidates;
} frame;

// frame* listOfFrames;
// candidate* listofCandidates;

// Input: Array of all the frames (each has array of candidates for that frame)
// Output: the source node of the graph (ie the start point)
node* composeGraph(frame*);

// Input: The filename for the jpeg and thresholds (to be decided)
// Output: An array of frames where each frame contains the candidate positions for that frame
frame* ballDetect(string jpg, /*[threshholds]*/);

// Input: the graph of frame candidates for every frame
// Output: a path where the returned pointer is the first node in the path and each node has only one edge out
// 		   there will be one node per frame representing which candidate we selected as the ball (cand pointer)
node* shortestPath(node* graph);

// Input: the trajectory path in the form of a nodes array, also the source and output string names for the mp4 videos
// Output: output mp4 video with the candidates drawn in a bounding box
void visualize(node* trajectory, string srcVidMp4, string outputVidName);

// Does all the plumbing
int main(int argc, int **argv[]);

#endif
