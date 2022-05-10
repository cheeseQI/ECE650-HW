package mapper;

public class Team {
    private int team_id;
    private String name;
    private int state_id;
    private int color_id;
    private int wins;
    private int losses;

    //private Color color; //n->1, n team can have same color

    public Team() {
    }

    public Team(int team_id, String name, int state_id, int color_id, int wins, int losses) {
        this.team_id = team_id;
        this.name = name;
        this.state_id = state_id;
        this.color_id = color_id;
        this.wins = wins;
        this.losses = losses;
    }

    public Team(String name, int state_id, int color_id, int wins, int losses) {
        this.name = name;
        this.state_id = state_id;
        this.color_id = color_id;
        this.wins = wins;
        this.losses = losses;
    }

    public int getWins() {
        return wins;
    }

    @Override
    public String toString() {
        return "Team{" +
                "team_id=" + team_id +
                ", name='" + name + '\'' +
                ", state_id=" + state_id +
                ", color_id=" + color_id +
                ", wins=" + wins +
                ", losses=" + losses +
                '}';
    }
}
