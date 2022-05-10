package mapper;

public class State {
    private int state_id;
    private String name;

    public State(int state_id, String name) {
        this.state_id = state_id;
        this.name = name;
    }

    public State(String name) {
        this.name = name;
    }
}
