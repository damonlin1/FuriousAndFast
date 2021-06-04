#include "mathlib.h"
#include "polygon.h"
#include "shape.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

body_t *shape_init_circle_sector(double radius, double sector_angle, rgb_color_t color,
                                 double density, void *info, free_func_t info_freer) {
    return shape_init_circle_sector_with_sprite(radius, sector_angle, color, density, 
                                                info, info_freer, NULL, VEC_ZERO);
}

body_t *shape_init_circle_sector_with_sprite(double radius, double sector_angle, rgb_color_t color,
                                             double density, void *info, free_func_t info_freer,
                                             const char *filename, vector_t dimensions) {
    list_t *points = list_init(30, free);
    vector_t *pen = malloc(sizeof(vector_t));
    assert(pen);

    // if it is not a fill circle, start point at the origin
    if (sector_angle != 0) {
        *pen = VEC_ZERO;
        list_add(points, pen);
    }

    pen = malloc(sizeof(vector_t));
    assert(pen);
    pen->x = radius * cos(sector_angle / 2);        
    pen->y = radius * sin(sector_angle / 2);
    list_add(points, pen);

    double rot_angle = (2 * M_PI - sector_angle) / 30;
    for (size_t i = 0; i < 29; i++) {
        vector_t *new_pen = malloc(sizeof(vector_t));
        assert(new_pen);

        *new_pen = vec_rotate(*pen, rot_angle);
        list_add(points, new_pen);
        pen = new_pen;
    }

    double mass = density * polygon_area(points);

    if (filename) {
        return body_init_with_info_and_sprite(points, mass, color, info,
                                              info_freer, filename, dimensions);
    }
    return body_init_with_info(points, mass, color, info, info_freer);                               
}

body_t *shape_init_circle(double radius, rgb_color_t color, double density,
                          void *info, free_func_t info_freer) {
    return shape_init_circle_sector(radius, 0, color, density, info, info_freer);
}

body_t *shape_init_circle_with_sprite(double radius, rgb_color_t color, double density, void *info,
                                      free_func_t info_freer, const char *filename, vector_t dimensions) {
    return shape_init_circle_sector_with_sprite(radius, 0, color, density, info,
                                                info_freer, filename, dimensions);
}

body_t *shape_init_rectangle(double length, double height, rgb_color_t color, double density,
                             void *info, free_func_t info_freer) {
    return shape_init_rectangle_with_sprite(length, height, color, density, info, info_freer, NULL, VEC_ZERO);
}

body_t *shape_init_rectangle_with_sprite(double length, double height, rgb_color_t color, double density,
                                         void *info, free_func_t info_freer, const char *filename,
                                         vector_t dimensions) {
    list_t *rectangle_points = list_init(4, free);
    vector_t *bot_left = malloc(sizeof(vector_t));
    assert(bot_left);
    *bot_left = (vector_t) {.x = -length / 2., .y = - height / 2.};
    list_add(rectangle_points, bot_left);

    vector_t *top_left = malloc(sizeof(vector_t));
    assert(top_left);
    *top_left = (vector_t) {.x = -length / 2., .y = height / 2.};
    list_add(rectangle_points, top_left);
       
    vector_t *top_right = malloc(sizeof(vector_t));
    assert(top_right);
    *top_right = (vector_t) {.x = length / 2., .y = height / 2.};
    list_add(rectangle_points, top_right);
        
    vector_t *bot_right = malloc(sizeof(vector_t));
    assert(bot_right);
    *bot_right = (vector_t) {.x = length / 2., .y = -height / 2.};
    list_add(rectangle_points, bot_right);
        
    double rect_mass = density * length * height;
        
    if (filename) {
        return body_init_with_info_and_sprite(rectangle_points, rect_mass, color,
                                                  info, info_freer, filename, dimensions);
    }
    return body_init_with_info(rectangle_points, rect_mass, color, info, info_freer);
}

body_t *shape_init_triangle_with_info(double width, double height, rgb_color_t color,
                                      double mass, void *info, free_func_t info_freer) {
    list_t *points = list_init(3, free);

    vector_t *point1 = malloc(sizeof(vector_t));
    point1->x = -width / 2.;
    point1->y = 0;
    list_add(points, point1);

    vector_t *point2 = malloc(sizeof(vector_t));
    point2->x = width / 2.;
    point2->y = 0;
    list_add(points, point2);

    vector_t *point3 = malloc(sizeof(vector_t));
    point3->x = 0;
    point3->y = height;
    list_add(points, point3);

    return body_init_with_info(points, mass, color, info, info_freer);
}

body_t *shape_init_ai_collider(body_t *ai_car) {
    vector_t dimensions = body_get_dimensions(ai_car);
    rgb_color_t black = {.r = 0, .g = 0, .b = 0};
    body_t *ai_collider = shape_init_triangle_with_info(4 * dimensions.x, 20, black,
                                                        0.1, (void *)ai_car, NULL);
    return ai_collider;
}

body_t *shape_init_player_indicator(body_t *player_car) {
    vector_t dimensions = body_get_dimensions(player_car);
    rgb_color_t blue = {.r = 0, .g = 0, .b = 0.8};
    body_t *indicator = shape_init_triangle_with_info(0.5 * dimensions.x, 20, blue,
                                                      0.1, (void *)player_car, NULL);
    return indicator;
}