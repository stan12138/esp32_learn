#include <math.h>

// 一个颜色
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

// 色表
typedef struct {
    color_t rgb[7];
    uint8_t mem[7];
    uint8_t len;
} color_map_t;

// 显存
typedef struct {
    char mem[122881];
    int width;
    int height;
    int len;
} memory_t;

// 一个像素 
typedef struct {
    color_t color;
    int x;
    int y;
} pixel_t;


// 计算两个颜色之间的距离
uint16_t color_distance(color_t *p1, color_t *p2);
// 计算出来一个给定的RGB颜色应该被转换为哪个墨水屏颜色
uint8_t color_trans(color_t *p, color_map_t *map);
// 往显存里设置一个像素，如果color_map设置为NULL，代表要直接指定墨水屏颜色
void set_pixle(memory_t *memory, pixel_t *p, color_map_t *color_map, uint8_t color);

// 清空缓存，刷新为白色
void clear_screen(memory_t *memory);








