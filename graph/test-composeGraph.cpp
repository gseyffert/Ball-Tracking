/** Test Code For composeGraph **/

#include "../BallTracking.h"
#include <cstdlib>

using namespace std;

/**
 * Generates a random double number between fMin and fMax
 * @param  fMin [description]
 * @param  fMax [description]
 * @return      [description]
 */
double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

int main(int argc, char* argv[]){
    int NUM_FRAMES = 10; //Number of frames to make
    int MAX_FRAMES_PER_FRAME = 4; //Frames will have a number of candidates between [1,MAX_FRAMES_PER_FRAME]

    // Allocate all the frames
    frame* f = new frame[NUM_FRAMES];
    for(int i=0; i<NUM_FRAMES; i++){
        // decide how many candidate for this frame
        int numCands = rand() % (MAX_FRAMES_PER_FRAME+1) +1; //in range [1,MAX_FRAMES_PER_FRAME+1]
        candidate* candidates = new candidate[numCands];

        // Populate candidates array for this frame
        for(int j=0; j<numCands; j++){
            candidate* c = &candidates[j];
            c->x = i;
            c->y = j;
            c->probability = fRand(0.0,1.0); //pick a random probability
            c->radius = fRand(1.0, 10.0);
        }

        // attach the candidates array to the current frame
        f[i].candidateList = candidates;
        f[i].numCandidates = numCands;
        f[i].nodes = NULL;
    }

    // t1 = timestamp();
    node* curNode = composeGraph(f, NUM_FRAMES);
    // t1 = timestamp() - t1;
    node* source = curNode;
    
    if (curNode->isStart != true) {
        cout << "First node is not source";
        return 0;
    }
    cout << "Source: " << endl;
    cout << *curNode << endl;

    for (int i = 0; i < NUM_FRAMES; i++) {
        
        for (int k = 0; k < f[i].numCandidates; k++) {
            //print all nodes
            cout << f[i].nodes[k] << " ";
        }
        cout << endl;
        
        if (i == NUM_FRAMES - 1) {
            for (int k = 0; k < f[i].numCandidates; k++) {
                curNode = &(f[i].nodes[k]);
                edge* curList = curNode->edgeList;
                if (curList[0].end->isSink != true) {
                    cout << "Node should be pointing to sink " << *(curList[k].start);
                    return 0;
                }
            }
        } else if (i == 0) {
            edge* curList = curNode->edgeList;
            for (int k = 0; k < f[i].numCandidates; k++) {
                node* endNode = curList[k].end;
                //Ensure equivalence of candidates in end nodes
                if (endNode == NULL) {
                    cout << "End node is null in edge #: " << k << " at the course node";
                    return 0;
                }
                if (endNode->cand->x != f[i].candidateList[k].x) {
                    cout << "Candidates x in end node not equivalent" << *(curList[k].end);
                    cout << "endNode: " << *endNode << endl;
                    cout << "was: " << *(endNode->cand) << " should be: " << f[i].candidateList[k] << endl;
                    return 0;
                }
                if (endNode->cand->y != f[i].candidateList[k].y) {
                    cout << "Candidates y in end node not equivalent " << *(curList[k].end);
                    cout << "endNode: " << *endNode << endl;
                    cout << "was: " << *(endNode->cand) << " should be: " << f[i].candidateList[k] << endl;
                    return 0;
                }
                if (endNode->cand->radius != f[i].candidateList[k].radius) {
                    cout << "Candidates radius in end node not equivalent " << *(curList[k].end);
                    cout << "endNode: " << *endNode << endl;
                    cout << "was: " << *(endNode->cand) << " should be: " << f[i].candidateList[k] << endl;
                    return 0;
                }
                if (endNode->cand->probability != f[i].candidateList[k].probability) {
                    cout << "Candidates prob in end node not equivalent " << *(curList[k].end);
                    cout << "endNode: " << *endNode << endl;
                    cout << "was: " << *(endNode->cand) << " should be: " << f[i].candidateList[k] << endl;
                    return 0; 
                }
                //Establish correctness of end node in current edge
                if (endNode->frameNum != f[i].nodes[k].frameNum) {
                    cout << "Wrong frame number in end node " << *(curList[k].end);
                    return 0;
                }
                if (endNode->candidateNum != f[i].nodes[k].candidateNum) {
                    cout << "Wrong candidate number in end node " << *(curList[k].end);
                    return 0;
                }
                if (endNode->numEdges != f[i].nodes[k].numEdges) {
                    cout << "Wrong edge count in end node " << *(curList[k].end);
                    return 0;
                }
            }
        } else {
            //Step through edgelists of nodes from previous frame
            for (int k = 0; k < f[i-1].numCandidates; k++) {
                curNode = &(f[i-1].nodes[k]);
                edge* curList = curNode->edgeList;
                
                //Compare nodes stored in edgelist to nodes stored in the current frame to
                //ensure that edges were built correctly
                for (int j = 0; j < f[i].numCandidates; j++) {
                    //Ensure equivalence of candidates stored in start nodes
                    node* startNode = curList[j].start;
                    node* endNode = curList[j].end;
                    if (startNode == NULL) {
                        cout << "Start node is null in frame #: " << i << "edge #: " << j;
                        return 0;
                    }
                    if (endNode == NULL) {
                        cout << "End node is null in frame #: " << i << "edge #: " << j;
                        return 0;
                    }
                    if (startNode->cand->x != f[i-1].candidateList[k].x) {
                        cout << "Candidates x in start node not equivalent " << *(curList[j].start);
                        cout << "endNode: " << *startNode << endl;
                        cout << "was: " << *(startNode->cand) << " should be: " << f[i-1].candidateList[k] << endl;
                        return 0;
                    }
                    if (startNode->cand->y != f[i-1].candidateList[k].y) {
                        cout << "Candidates x in start node not equivalent " << *(curList[j].start);
                        cout << "endNode: " << *startNode << endl;
                        cout << "was: " << *(startNode->cand) << " should be: " << f[i-1].candidateList[k] << endl;
                        return 0;
                    }
                    if (startNode->cand->radius != f[i-1].candidateList[k].radius) {
                        cout << "Candidates radius in start node not equivalent " << *(curList[j].start);
                        cout << "endNode: " << *startNode << endl;
                        cout << "was: " << *(startNode->cand) << " should be: " << f[i-1].candidateList[k] << endl;
                        return 0;
                    }
                    if (startNode->cand->probability != f[i-1].candidateList[k].probability) {
                        cout << "Candidates prob in start node not equivalent " << *(curList[j].start);
                        cout << "endNode: " << *startNode << endl;
                        cout << "was: " << *(startNode->cand) << " should be: " << f[i-1].candidateList[k] << endl;
                        return 0;
                    }
                    //Ensure equivalence of candidates in end nodes
                    if (endNode->cand->x != f[i].candidateList[j].x) {
                        cout << "Candidates x in end node not equivalent " << *(curList[j].end);
                        cout << "endNode: " << *endNode << endl;
                        cout << "was: " << *(endNode->cand) << " should be: " << f[i].candidateList[j] << endl;
                        return 0;
                    }
                    if (endNode->cand->y != f[i].candidateList[j].y) {
                        cout << "Candidates y in end node not equivalent " << *(curList[j].end);
                        cout << "endNode: " << *endNode << endl;
                        cout << "was: " << *(endNode->cand) << " should be: " << f[i].candidateList[j] << endl;
                        return 0;
                    }
                    if (endNode->cand->radius != f[i].candidateList[j].radius) {
                        cout << "Candidates radius in end node not equivalent " << *(curList[j].end);
                        cout << "endNode: " << *endNode << endl;
                        cout << "was: " << *(endNode->cand) << " should be: " << f[i].candidateList[j] << endl;
                        return 0;
                    }
                    if (endNode->cand->probability != f[i].candidateList[j].probability) {
                        cout << "Candidates prob in end node not equivalent " << *(curList[j].end);
                        cout << "endNode: " << *endNode << endl;
                        cout << "was: " << *(endNode->cand) << " should be: " << f[i].candidateList[j] << endl;
                        return 0; 
                    }
                    //Establish correctness of start node in current edge
                    if (startNode->frameNum != curNode->frameNum) {
                        cout << "Wrong frame number in start node " << *(curList[j].start);
                        return 0;
                    }
                    if (startNode->candidateNum != curNode->candidateNum) {
                        cout << "Wrong candidate number in start node " << *(curList[j].start);
                        return 0;
                    }
                    if (startNode->numEdges != curNode->numEdges) {
                        cout << "Wrong edge count in start node " << *(curList[j].start);
                        return 0;
                    }
                    //Establish correctness of end node in current edge
                    if (endNode->frameNum != f[i].nodes[j].frameNum) {
                        cout << "Wrong frame number in end node " << *(curList[j].end);
                        return 0;
                    }
                    if (endNode->candidateNum != f[i].nodes[j].candidateNum) {
                        cout << "Wrong candidate number in end node " << *(curList[j].end);
                        return 0;
                    }
                    if (endNode->numEdges != f[i].nodes[j].numEdges) {
                        cout << "Wrong edge count in end node " << *(curList[j].end);
                        return 0;
                    }
                    continue;
                }
            }
        }
        
    }

    curNode = curNode->edgeList[0].end;
    cout << "Sink:" << endl;
    cout << *curNode << endl;

    // Free the graph after we are done
    curNode = source;
    freeGraph(f, curNode, NUM_FRAMES);

    //Now free all the frames and candidates
    for(int i=0; i<NUM_FRAMES; i++){
        frame* curFrame = f + i;
        delete curFrame->candidateList;
    }
    delete f;
    return 0;
}
