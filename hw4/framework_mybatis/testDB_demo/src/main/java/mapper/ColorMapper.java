package mapper;

import org.apache.ibatis.annotations.Param;

import java.util.List;

public interface ColorMapper {
    void initColorTable();
    void insert(List<Color> colors);
}
