#ifndef BALL_H
#define BALL_H

#include<string>

typedef struct {
	int frameNumber;	
	int candidateNum;
	edge* list;
} node;

typedef struct {
	int weight;
	node* to;
	node* from;
} edge;

typedef struct  {
	int x;
	int y;
	int radius;
	int probability;
} candidate;

typedef struct {
	candidate* list;
	uint64_t numCandidates;
} frame;

// frame* listOfFrames;
// candidate* listofCandidates;

graph composeGraph(frame*);

frame ballDetect(string jpg, /*[threshholds]*/);

listOfCandidates shortestPath(graph);

mp4 visualize(candidate*, string srcVidMp4);

int main(int argc, int **argv[]);

#endif
