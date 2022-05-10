package mapper;

public class Player {
    private int player_id;
    private int team_id;
    private int uniform_num;
    private String first_name;
    private String last_name;
    private int mpg;
    private int ppg;
    private int rpg;
    private int apg;
    private double spg;
    private double bpg;
    private Team team; //n player has 1 team, need association in xml

    public Player() {
    }

    public Player(int player_id, int team_id, int uniform_num, String first_name, String last_name,
                  int mpg, int ppg, int rpg, int apg, double spg, double bpg) {
        this.player_id = player_id;
        this.team_id = team_id;
        this.uniform_num = uniform_num;
        this.first_name = first_name;
        this.last_name = last_name;
        this.mpg = mpg;
        this.ppg = ppg;
        this.rpg = rpg;
        this.apg = apg;
        this.spg = spg;
        this.bpg = bpg;
    }

    public Player(int team_id, int uniform_num, String first_name, String last_name,
                  int mpg, int ppg, int rpg, int apg, double spg, double bpg) {
        this.team_id = team_id;
        this.uniform_num = uniform_num;
        this.first_name = first_name;
        this.last_name = last_name;
        this.mpg = mpg;
        this.ppg = ppg;
        this.rpg = rpg;
        this.apg = apg;
        this.spg = spg;
        this.bpg = bpg;
    }

    public String getFirst_name() {
        return first_name;
    }

    public String getLast_name() {
        return last_name;
    }

    public int getUniform_num() {
        return uniform_num;
    }

    public int getTeam_wins(){
        return team.getWins();
    }

    public Team getTeam() {
        return team;
    }

    public void setTeam(Team team) {
        this.team = team;
    }

    @Override
    public String toString() {
        return "Player{" +
                "player_id=" + player_id +
                ", team_id=" + team_id +
                ", uniform_num=" + uniform_num +
                ", first_name='" + first_name + '\'' +
                ", last_name='" + last_name + '\'' +
                ", mpg=" + mpg +
                ", ppg=" + ppg +
                ", rpg=" + rpg +
                ", apg=" + apg +
                ", spg=" + spg +
                ", bpg=" + bpg +
                '}';
    }
}
