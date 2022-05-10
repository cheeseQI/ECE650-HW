#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#define EXPECTED_RING 4
#define MAX_HOP 512
int checkRingArgument(int argc, char *argv[]) {
    if (argc != EXPECTED_RING) {
        perror("Invalid argument, usage: ringmaster <port_num> <num_players> <num_hops>\n");
        exit(1);
    }
    int pnum = atoi(argv[2]);
    if (pnum <= 1) {
        perror("Invalid players num, it should be larger than 1\n");
        exit(1);
    } 
    int hnum = atoi(argv[3]);
    if (hnum > MAX_HOP || hnum < 0) {
        perror("Invalid hops num, it should be in [0, 512]\n");
        exit(1);
    }
    return 0;
}
