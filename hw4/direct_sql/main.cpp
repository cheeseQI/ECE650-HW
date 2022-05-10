#include <iostream>
#include <pqxx/pqxx>
#include <fstream>
#include <vector>

#include "exerciser.h"

using namespace std;
using namespace pqxx;

/**
 * @brief Get the Tuples into vector, from the file
 * 
 * @param fileName of the target
 * @return vector<string> store all content of each tuple
 */
vector<string> getTuples(string fileName) {
  vector<string> ans;
  ifstream myFile;

  myFile.open(fileName);
  if (myFile.fail()) {
    perror("cannot open the color file");
    exit(EXIT_FAILURE);
  }
  while (!myFile.eof()) {
    string tempLine;
    getline(myFile, tempLine);
    if (tempLine.length() == 0) break;
    ans.push_back(tempLine);
  }
  myFile.close();

  /*for (vector<string>::iterator it = ans.begin(); it != ans.end(); ++ it) {
    cout << *it << endl;
  }*/
  return ans;
}
//init the table with info
void initTable(connection * myDB) {
  work w(*myDB);
  w.exec("DROP TABLE IF EXISTS player, team, state, color");
  w.exec("CREATE TABLE player (PLAYER_ID SERIAL PRIMARY KEY NOT NULL," 
                              "TEAM_ID integer NOT NULL," 
                              "UNIFORM_NUM  integer NOT NULL,"    
                              "FIRST_NAME varchar(50) NOT NULL," 
                              "LAST_NAME varchar(50) NOT NULL,"
                              "MPG integer NOT NULL,"
                              "PPG integer NOT NULL,"
                              "RPG integer NOT NULL,"
                              "APG integer NOT NULL,"
                              "SPG real NOT NULL,"
                              "BPG real NOT NULL)");
  w.exec("CREATE TABLE team (TEAM_ID SERIAL PRIMARY KEY NOT NULL," 
                            "NAME varchar(50) NOT NULL,"
                            "STATE_ID integer NOT NULL," 
                            "COLOR_ID integer NOT NULL, "
                            "WINS integer NOT NULL, "
                            "LOSSES integer NOT NULL)");
  w.exec("CREATE TABLE state (STATE_ID SERIAL PRIMARY KEY NOT NULL, NAME varchar(50) NOT NULL)");
  w.exec("CREATE TABLE color (COLOR_ID SERIAL PRIMARY KEY NOT NULL, NAME varchar(50) NOT NULL)");
  w.commit();
}
//insert info of player into table
void insertPlayerInfo(connection * myDB, string fileName) {
  vector<string> dataLine = getTuples(fileName);
  for (vector<string>::iterator it = dataLine.begin(); it != dataLine.end(); ++ it) {
    stringstream tempStream;
    int id; //primary key
    int teamId;
    int uniformNum;
    string firstName;
    string lastName;
    int mpg, ppg, rpg, apg;
    double spg, bpg;

    string temp = *it;
    tempStream << temp;
    tempStream >> id >> teamId >> uniformNum >> firstName >> lastName >> mpg >> ppg >> rpg >> apg >> spg >> bpg; //split the line into (id name) 
    add_player(myDB, teamId, uniformNum, firstName, lastName, mpg, ppg, rpg, apg, spg, bpg);
  }
}
//insert info of team into table
void insertTeamInfo(connection * myDB, string fileName) {
  vector<string> dataLine = getTuples(fileName);
  for (vector<string>::iterator it = dataLine.begin(); it != dataLine.end(); ++ it) {
    stringstream tempStream;
    int id;
    string teamName;
    int stateId, colorId, wins, losses;

    string temp = *it;
    tempStream << temp;
    tempStream >> id >> teamName >> stateId >> colorId >> wins >> losses;
    add_team(myDB, teamName, stateId, colorId, wins, losses);
  }
}
//inerst info of color into table
void insertColorInfo(connection * myDB, string fileName) {
  vector<string> dataLine = getTuples(fileName);
  for (vector<string>::iterator it = dataLine.begin(); it != dataLine.end(); ++ it) {
    stringstream tempStream;
    int id;
    string name;
    string temp = *it;
    tempStream << temp;
    tempStream >> id >> name; //split the line into (id name) 
    add_color(myDB, name);
  }
}
//insert info of state into table
void insertStateInfo(connection * myDB, string fileName) {
  vector<string> dataLine = getTuples(fileName);
  for (vector<string>::iterator it = dataLine.begin(); it != dataLine.end(); ++ it) {
    stringstream tempStream;
    int id;
    string name;
    string temp = *it;
    tempStream << temp;
    tempStream >> id >> name;
    add_state(myDB, name);
  }
}

int main (int argc, char *argv[]) {
  //Allocate & initialize a Postgres connection object
  connection *C;
  try{//Establish a connection to the database
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
  } catch (const exception &e){
    cerr << e.what() << endl;
    return 1;
  }
  try { // init / drop&create table and load value
    initTable(C);
    insertColorInfo(C, "./color.txt");
    insertPlayerInfo(C, "./player.txt");
    insertTeamInfo(C, "./team.txt");
    insertStateInfo(C, "./state.txt");
  } catch (exception const &e) {
    cerr << e.what() << endl;
    return 1;
  }

  exercise(C);
  //Close database connection
  C->disconnect();
  return 0;
}


