/** Test Code For composeGraph **/

#include "../BallTracking.h"
#include <random>

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
    int NUM_FRAMES = 100; //Number of frames to make
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

    // run BFS on the graph to make sure that everything is correct
    
    return 0;
}