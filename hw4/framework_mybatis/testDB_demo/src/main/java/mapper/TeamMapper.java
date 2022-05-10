package mapper;

import org.apache.ibatis.annotations.Param;

import java.util.List;

public interface TeamMapper {
    void initTeamTable();
    void insert(List<Team> teams);
    List<String> query2(@Param("team_color") String team_color);
}
