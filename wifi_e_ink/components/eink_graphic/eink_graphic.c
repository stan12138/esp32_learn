#include <stdio.h>
#include "eink_graphic.h"

uint16_t color_distance(color_t *p1, color_t *p2) {
    uint16_t dr = (p1->r > p2->r) ? (uint16_t) (p1->r - p2->r) : (uint16_t) (p2->r - p1->r);
    uint16_t dg = (p1->g > p2->g) ? (uint16_t) (p1->g - p2->g) : (uint16_t) (p2->g - p1->g);
    uint16_t db = (p1->b > p2->b) ? (uint16_t) (p1->b - p2->b) : (uint16_t) (p2->b - p1->b);

    return dr + dg + db;
}

uint8_t color_trans(color_t *p, color_map_t *map) {
    uint16_t min_distance = color_distance(p, &(map->rgb[0]));
    uint8_t res = map->mem[0];
    for(int i=0; i < map->len; i++) {
        uint16_t tmp = color_distance(p, &(map->rgb[i]));
        if(tmp < min_distance) {
            min_distance = tmp;
            res = map->mem[i];
        }
    }
    return res;
}

void set_pixle(memory_t *memory, pixel_t *p, color_map_t *color_map, uint8_t color) {
    uint8_t real_color = color;
    if(color_map != NULL) {
        real_color = color_trans(&(p->color), color_map);
    }
    // printf("color: %d\n", real_color);
    int pixle_index = memory->width * p->y + p->x;
    int char_index = pixle_index / 2;
    if(pixle_index%2 == 0) {
        memory->mem[char_index] = (memory->mem[char_index] & 0x0f) + (real_color << 4);
    } else {
        memory->mem[char_index] = (memory->mem[char_index] & 0xf0) + (real_color & 0x0f);
    }
}

void clear_screen(memory_t *memory) {
    for(int i=0; i < memory->len; i++) {
        memory->mem[i] = 0x33;
    }
}
