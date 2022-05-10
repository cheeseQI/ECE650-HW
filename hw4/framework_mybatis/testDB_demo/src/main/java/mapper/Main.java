package mapper;

import org.apache.ibatis.session.SqlSession;

import java.io.IOException;
import java.util.List;

public class Main {
    public static void main(String[] args) throws IOException {
        //get session(java program <-> database)
        SqlSession session = SqlSessionUtils.getSqlSession();
        //get one instance whose class implements the given interface(proxy pattern)
        ColorMapper cm = session.getMapper(ColorMapper.class);
        cm.initColorTable();
        PlayerMapper pm = session.getMapper(PlayerMapper.class);
        pm.initPlayerTable();
        TeamMapper tm = session.getMapper(TeamMapper.class);
        tm.initTeamTable();
        StateMapper sm = session.getMapper(StateMapper.class);
        sm.initStateTable();
        //commit automatically
        /** insert here **/
        QueryHandler h = new QueryHandler();
        cm.insert(h.getColors("color.txt"));
        pm.insert(h.getPlayers("player.txt"));
        tm.insert(h.getTeams("team.txt"));
        sm.insert(h.getStates("state.txt"));
        /** query exercise here **/
        List<Player> players = pm.query1(1, 35, 40, 1, 10, 15, 1, 5, 10,
                1, 1, 3, 1, 1, 1.2, 1, 1.0, 1.5);
        System.out.println(players);
        //List<Player> players1 = pm.query1(1,35, 40 ,0,0,0,0,0,0,0,0, 0, 0,0,0,0,0,0);
        //System.out.println(players1);
        //find green team
        List<String> teamNames = tm.query2("Green");
        System.out.println(teamNames);
        //find pittsburgh player
        List<Player> q3Info = pm.query3("Pittsburgh");
        System.out.println("FIRST_NAME LAST_NAME");
        for (Player p: q3Info) {
            System.out.println(p.getFirst_name() + " " + p.getLast_name());
        }
        //find duke player
        List<Player> q4Info = pm.query4("NC", "DarkBlue");
        System.out.println("FIRST_NAME LAST_NAME UNIFORM_NUM");
        for (Player p: q4Info) {
            System.out.println(p.getFirst_name() + " " + p.getLast_name() + " " + p.getUniform_num());
        }
        //find wins > 9
        List<Player> q5Info = pm.query5(9);
        System.out.println("FIRST_NAME LAST_NAME NAME WINS");
        for (Player p: q5Info) {
            System.out.println( p.getFirst_name() + " " + p.getLast_name() + " " + p.getTeam_wins());
        }
        session.close();
    }
}
