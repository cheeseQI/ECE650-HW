package mapper;

import org.apache.ibatis.io.Resources;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.LinkedList;
import java.util.List;

public class QueryHandler {
    //get all tuples of color from file
    public List<Color> getColors(String fileName) throws IOException {
        List<Color> ans = new LinkedList<>();
        InputStream in = QueryHandler.class.getClassLoader().getResourceAsStream(fileName);
        BufferedReader br = new BufferedReader(new InputStreamReader(in));
        String s;
        while ((s = br.readLine()) != null) {
            String[] colorTuple = s.split(" ");
            Color c = new Color(Integer.parseInt(colorTuple[0]), colorTuple[1]);
            ans.add(c);
        }
        return ans;
    }

    public List<Player> getPlayers(String fileName) throws IOException {
        List<Player> ans = new LinkedList<>();
        InputStream in = QueryHandler.class.getClassLoader().getResourceAsStream(fileName);
        BufferedReader br = new BufferedReader(new InputStreamReader(in));
        String s;
        while ((s = br.readLine()) != null) {
            String[] playerTuple = s.split(" ");
            Player p = new Player(Integer.parseInt(playerTuple[0]), Integer.parseInt(playerTuple[1]), Integer.parseInt(playerTuple[2]),
                    playerTuple[3], playerTuple[4], Integer.parseInt(playerTuple[5]), Integer.parseInt(playerTuple[6]), Integer.parseInt(playerTuple[7]),
                    Integer.parseInt(playerTuple[8]), Double.parseDouble(playerTuple[9]), Double.parseDouble(playerTuple[10]));
            ans.add(p);
        }
        return ans;
    }

    public List<State> getStates(String fileName) throws IOException{
        List<State> ans = new LinkedList<>();
        InputStream in = QueryHandler.class.getClassLoader().getResourceAsStream(fileName);
        BufferedReader br = new BufferedReader(new InputStreamReader(in));
        String s;
        while ((s = br.readLine()) != null) {
            String[] stateTuple = s.split(" ");
            State state = new State(Integer.parseInt(stateTuple[0]), stateTuple[1]);
            ans.add(state);
        }
        return ans;
    }

    public List<Team> getTeams(String fileName) throws IOException {
        List<Team> ans = new LinkedList<>();
        InputStream in = QueryHandler.class.getClassLoader().getResourceAsStream(fileName);
        BufferedReader br = new BufferedReader(new InputStreamReader(in));
        String s;
        while ((s = br.readLine()) != null) {
            String[] teamTuple = s.split(" ");
            Team t = new Team(Integer.parseInt(teamTuple[0]), teamTuple[1], Integer.parseInt(teamTuple[2]),
                    Integer.parseInt(teamTuple[3]), Integer.parseInt(teamTuple[4]), Integer.parseInt(teamTuple[5]));
            ans.add(t);
        }
        return ans;
    }


}
