package mapper;

import org.apache.ibatis.annotations.Param;

import java.util.List;

public interface PlayerMapper {
    void initPlayerTable();
    void insert(List<Player> players);
    List<Player> query1(@Param("use_mpg") int use_mpg, @Param("min_mpg") int min_mpg, @Param("max_mpg") int max_mpg,
                @Param("use_ppg") int use_ppg, @Param("min_ppg") int min_ppg, @Param("max_ppg") int max_ppg,
                @Param("use_rpg") int use_rpg, @Param("min_rpg") int min_rpg, @Param("max_rpg") int max_rpg,
                @Param("use_apg") int use_apg, @Param("min_apg") int min_apg, @Param("max_apg") int max_apg,
                @Param("use_spg") int use_spg, @Param("min_spg") double min_spg, @Param("max_spg") double max_spg,
                @Param("use_bpg") int use_bpg, @Param("min_bpg") double min_bpg, @Param("max_bpg") double max_bpg);
    List<Player> query3(@Param("team_name") String team_name);
    List<Player> query4(@Param("team_state") String team_state, @Param("team_color") String team_color);
    List<Player> query5(@Param("num_wins") int num_wins);
}
