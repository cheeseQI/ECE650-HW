#include "exerciser.h"


void testQ1_MPG(connection *C) {
    //eg from the readme, find between 35-40 for mmp
    query1(C, 1, 35, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
void testQ1_PPG(connection *C) {
    cout << endl;
    query1(C, 0, 35, 40, 1, 15, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
void testQ1_RPG(connection *C) {
    cout << endl;
    query1(C, 0, 35, 40, 0, 15, 20, 1, 5, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
void testQ1_APG(connection *C) { //find 1.1 - 1.5
    cout << endl;
    query1(C, 0, 35, 40, 0, 15, 20, 0, 5, 10, 1, 1.1, 1.5, 0, 0, 0, 0, 0, 0);
}
void testQ1_SPG(connection *C) { //find 1.0 - 1.2
    cout << endl;
    query1(C, 0, 35, 40, 0, 15, 20, 0, 5, 10, 0, 1.1, 1.5, 1, 1, 1.2, 0, 0, 0);
}
void testQ1_BPG(connection *C) { //find 0.5 - 1.0
    cout << endl;
    query1(C, 0, 35, 40, 0, 15, 20, 0, 5, 10, 0, 1.1, 1.5, 0, 1, 1.2, 1, 0.5, 1);
}

void exercise(connection *C) {
    testQ1_MPG(C);
    testQ1_PPG(C);
    testQ1_RPG(C);
    testQ1_APG(C);
    testQ1_SPG(C);
    testQ1_BPG(C);
    //this following will only find tyler lydon
    query1(C, 1, 35, 40, 1, 10, 15, 1, 5, 10, 1, 1, 3, 1, 1, 1.2, 1, 1.0, 1.5);
    //find team with maroon color
    query2(C, "Maroon");
    //find players for duke team, from highest to lowest ppg 
    query3(C, "Duke");
    //find players in state SC wearing Orange color
    query4(C, "SC", "Orange");
    //find players with teams winning more than 9 times
    query5(C, 9);
}
