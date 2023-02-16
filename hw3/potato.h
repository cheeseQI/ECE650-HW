typedef struct potato_t {
    /* record hops and socket of ring master */
    int remainingHops;
    // next path will be put at idx pathCount
    int pathCount; 
    int path[512];
    int isAlive;
} potato;

// initialization done by ring master
void initPotato(potato * p, int hops) {
    for (int i = 0; i < 512; i ++) {
        p->path[i] = 0;
    }
    p->remainingHops = hops;
    p->pathCount = 0;
    p->isAlive = 1;
}

/**
 * @brief modify the information record in potato, and the
 * potato will be adjusted per each pass time
 * @param p potato to be adjusted
 * @param nextPlayer who will be the next one to get potato 
 */
void adjustPotato(potato * p, int nextPlayer) {
    p->remainingHops --;
    // add next player into path
    p->path[p->pathCount] = nextPlayer; 
    p->pathCount ++;
}
