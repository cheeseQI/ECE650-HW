package mapper;

import java.util.List;

public interface StateMapper {
    void initStateTable();
    void insert(List<State> states);
}
