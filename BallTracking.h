typedef struct {
	int frameNumber;	
	int candidateNum;
	edge* list;
} node;

typedef struct {
	node* to;
	node* from;
	int weight(euclidian dist?);
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

listOfFrames;
listofCandidates;

graph composeGraph(*listOfFrames) {
	Graham
}

frame ballDetect(.jpg, [threshholds]) {
	Max + Myles
}

listOfCandidates shortestPath(graph) {
	Evan
}

mp4 visualize(listOfCandidates, srcVid.mp4) {
	Avi
}

main(int argc, int **argv[]) {
	David
}


