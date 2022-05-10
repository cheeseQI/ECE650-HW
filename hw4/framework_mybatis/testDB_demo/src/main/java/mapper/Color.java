package mapper;

public class Color {
    private int color_id;
    private String name;

    public Color(int color_id, String name) {
        this.color_id = color_id;
        this.name = name;
    }

    public Color(String name) {
        this.name = name;
    }
}
