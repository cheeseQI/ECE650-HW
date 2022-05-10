import mapper.*;
import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.util.List;

public class readFileTest {
    //@Test
    void testColor() throws IOException {
        QueryHandler h = new QueryHandler();
        List<Color> cs = h.getColors("color.txt");
        for (Color c: cs) {
            //System.out.println(" name: " + c.name);
        }
    }

    //@Test
    void testPlayer() throws IOException {
        QueryHandler h = new QueryHandler();
        List<Player> ps = h.getPlayers("player.txt");
        for (Player p: ps) {
            /*System.out.println(" teamID: " + p.team_id + " uniform: " + p.uniform_num + " first: " + p.first_name +
                    " last: " + p.last_name + " mpg: " + p.mpg + " ppg : " + p.ppg + " rpg: " + p.rpg +
                    " apg: " + p.apg +
                    " spg: " + p. spg + " bpg: " + p.bpg);*/
        }
    }

    //@Test
    void testTeam() throws IOException {
        QueryHandler h = new QueryHandler();
        List<Team> ts = h.getTeams("team.txt");
        for (Team t: ts) {
            /*System.out.println("name: " + t.name + " stateid: " + t.state_id + " colorid: " + t.color_id +
                    " wins: " + t.wins + " losses: " + t.losses);*/
        }
    }

    @Test
    void testState() throws IOException {
        QueryHandler h = new QueryHandler();
        List<State> ss = h.getStates("state.txt");
        for (State s: ss) {
            //System.out.println("name: " + s.name);
        }
    }
}
