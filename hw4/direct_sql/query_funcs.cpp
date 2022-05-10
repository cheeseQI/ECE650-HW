#include "query_funcs.h"
#include <iomanip>

//read some info from table according to sql command
void read_table(connection *C, string sqlCommand, int count) {
    nontransaction n(*C);
    result r(n.exec(sqlCommand));
    for (result::iterator it = r.begin(); it != r.end(); ++ it) {
        for (int i = 0; i < count; i ++) {
            if (i == 9 || i == 10) { //handle last two double in player table,
                cout << fixed << setprecision(1) << it[i].as<double>() << " ";
            }
            else cout << it[i]<< " ";
        }
        cout << endl;
    }
}

//add player tuple into the table
void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg) {
    stringstream tempCommand;
    work w(*C);
    //using escape for string to avoid sql injection
    tempCommand << "INSERT INTO player (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) VALUES " 
                "('" << team_id << "', '" << jersey_num << "', '" << w.esc(first_name) << "', '" << w.esc(last_name) << 
                "', '" << mpg << "', '" << ppg << "', '" << rpg << "', '" << apg << "', '" << spg << "', '" << bpg << "')";
    w.exec(tempCommand.str());
    w.commit();
}

//add team tuple into the table
void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses) {
    stringstream tempCommand;
    work w(*C);
    tempCommand << "INSERT INTO team (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES "
                "('" << name << "', '" << state_id << "', '" << color_id << "', '" << wins << 
                "', '" << losses << "')";
    w.exec(tempCommand.str());
    w.commit();
}

//add state tuple into the table
void add_state(connection *C, string name) {
    string sqlCommand = "INSERT INTO state (NAME) VALUES ('" + name + "')";
    work w(*C);
    w.exec(sqlCommand);
    w.commit();
}

//add new tuple with name into the color table
void add_color(connection *C, string name) {
    string sqlCommand = "INSERT INTO color (NAME) VALUES ('" + name + "')";
    work w(*C);
    w.exec(sqlCommand);
    w.commit();
}

void query1(connection *C,
	    int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg
            ) {
    string sqlCommand = "SELECT * FROM player ";
    if (use_mpg || use_ppg || use_rpg || use_apg || use_spg || use_bpg) {
        sqlCommand += "WHERE PLAYER_ID > -1 "; //always true, can be appended
    }
    if (use_mpg) {
        sqlCommand += "AND ( MPG BETWEEN " + to_string(min_mpg) + " AND " + to_string(max_mpg) + ") ";
    } 
    if (use_ppg) {
        sqlCommand += "AND ( PPG BETWEEN " + to_string(min_ppg) + " AND " + to_string(max_ppg) + ") ";
    }
    if (use_rpg) {
        sqlCommand += "AND ( RPG BETWEEN " + to_string(min_rpg) + " AND " + to_string(max_rpg) + ") ";
    }
    if (use_apg) {
        sqlCommand += "AND ( APG BETWEEN " + to_string(min_apg) + " AND " + to_string(max_apg) + ") ";
    }
    if (use_spg) {
        sqlCommand += "AND ( SPG BETWEEN " + to_string(min_spg) + " AND " + to_string(max_spg) + ") ";
    } 
    if (use_bpg) {
        sqlCommand += "AND ( BPG BETWEEN " + to_string(min_bpg) + " AND " + to_string(max_bpg) + ") ";
    }
    cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG" << endl;
    read_table(C, sqlCommand, 11);
}

void query2(connection *C, string team_color) {
    string sqlCommand = "SELECT team.NAME FROM team, color WHERE team.COLOR_ID = color.COLOR_ID AND color.NAME = '" + team_color + "';";
    cout << "NAME" << endl;
    read_table(C, sqlCommand, 1);
}

void query3(connection *C, string team_name) {
    string sqlCommand = "SELECT player.FIRST_NAME, player.LAST_NAME FROM player, team WHERE player.TEAM_ID = team.TEAM_ID "
    "AND team.NAME = '" + team_name + "' "
    "ORDER BY player.PPG DESC;";
    cout << "FIRST_NAME LAST_NAME" << endl;
    read_table(C, sqlCommand, 2);
}

void query4(connection *C, string team_state, string team_color) {
    string sqlCommand = "SELECT player.FIRST_NAME, player.LAST_NAME, player.UNIFORM_NUM FROM player, team, color, state WHERE "
    "player.TEAM_ID = team.TEAM_ID AND team.COLOR_ID = color.COLOR_ID AND team.STATE_ID = state.STATE_ID AND "
    "state.NAME = '" + team_state + "' " + "AND " + "color.NAME = '" + team_color + "';";
    cout << "FIRST_NAME LAST_NAME UNIFORM_NUM" << endl;
    read_table(C, sqlCommand, 3);
}

void query5(connection *C, int num_wins) {
    string sqlCommand = "SELECT player.FIRST_NAME, player.LAST_NAME, team.NAME, team.WINS FROM player, team WHERE "
    "player.TEAM_ID = team.TEAM_ID AND "
    "team.wins > '" + to_string(num_wins) + "';";
    cout << "FIRST_NAME LAST_NAME NAME WINS" << endl;
    read_table(C, sqlCommand, 4);
}
