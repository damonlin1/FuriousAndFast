#include "body.h"
#include "color.h"
#include "faf_cars.h"
#include "faf_levels.h"
#include "faf_objects.h"
#include "forces.h"
#include "mathlib.h"
#include "scene.h"
#include "shape.h"
#include "vector.h"
#include <assert.h>
#include <stdlib.h>

// Scene layer definitions
const size_t FAF_HIDDEN_LAYER = 0;
const size_t FAF_BACKGROUND_LAYER = 1;
const size_t FAF_FOREGROUND_LAYER = 2;
const size_t FAF_OBJECT_LAYER = 3;
const size_t FAF_CAR_LAYER = 4;

// General scene properties
const rgb_color_t FAF_REGULAR_ROAD_COLOR = {.r = (float)0.1, .g = (float)0.1, .b = (float)0.1};
const rgb_color_t FAF_ROAD_STRIPE_COLOR = {.r = (float)1, .g = (float)1, .b = (float)1};
const double FAF_ROAD_STRIPE_WIDTH = 10;
const double FAF_ROAD_STRIPE_HEIGHT = 25;
const double FAF_ROAD_STRIPE_SPACING = 50;
const size_t FAF_ROAD_LANES = 3;
const double FAF_DEFAULT_DENSITY = 1;
const size_t FAF_INIT_NUM_BODIES_IN_SCENE = 10;
const double FAF_ELASTICITY = 1.;
const size_t FAF_NUM_DECORATIONS = 80;
const size_t FAF_NUM_EFFECTS = 60;
const size_t FAF_NUM_OBSTACLES = 70;
const size_t FAF_NUM_GAS = 80;
const vector_t FAF_DIMENSIONS = {.x = 1000, .y = 20000};
const double FAF_ROAD_WIDTH = 700;
const double FAF_ROAD_COEF = 0.05;
const double FAF_SIDE_WIDTH = 150;
const double FAF_BLOCK_LENGTH = 1000;
const double FAF_BLOCK_WIDTH = 10;
const vector_t FAF_FINISH_LINE_DIMENSIONS = {.x = 700, .y = 100};
const rgb_color_t FAF_FINISH_LINE_COLOR = {.r = 0, .g = 0, .b = 0};

// Properties of the desert level
const rgb_color_t FAF_DESERT_BACKGROUND_COLOR = {.r = (float)0.95, .g = (float)0.64, .b = (float)0.38};
const double FAF_DESERT_SAND_COEF = 0.5;

// Properties of the ice level
const rgb_color_t FAF_ICE_BACKGROUND_COLOR = {.r = (float)0.84, .g = (float)0.93, .b = (float)0.94};
const double FAF_ICE_ICE_COEF = 0.1;

// Properties of the forest level
const rgb_color_t FAF_FOREST_BACKGROUND_COLOR = {.r = (float)0.12, .g = (float)0.55, .b = (float)0.13};
const double FAF_FOREST_TREES_COEF = 0.7;

vector_t faf_get_scene_dimensions() {
    return FAF_DIMENSIONS;
}

double faf_get_road_width() {
    return FAF_ROAD_WIDTH;
}

scene_t *faf_make_level(faf_level_t type, list_t *cars, list_t *ai_colliders) {
    rgb_color_t side_color;
    double side_coef;

    switch (type) {
        case DESERT_LEVEL: {
            side_color = FAF_DESERT_BACKGROUND_COLOR;
            side_coef = FAF_DESERT_SAND_COEF;
            break;
        }
        case ICE_LEVEL: {
            side_color = FAF_ICE_BACKGROUND_COLOR;
            side_coef = FAF_ICE_ICE_COEF;
            break;
        }
        case FOREST_LEVEL: {
            side_color = FAF_FOREST_BACKGROUND_COLOR;
            side_coef = FAF_FOREST_TREES_COEF;
            break;
        }
    }

    assert(cars);

    scene_t *scene = scene_init(FAF_DIMENSIONS);
    list_t *collision_bodies = list_init(FAF_INIT_NUM_BODIES_IN_SCENE, NULL);

    // Add decorations on the side of the road
    faf_object_spawn_decorations(scene, FAF_DIMENSIONS, collision_bodies, FAF_ROAD_WIDTH,
                                 FAF_NUM_DECORATIONS, type);

    // Add random effects
    faf_object_spawn_effects(scene, FAF_DIMENSIONS, collision_bodies,
                             FAF_ROAD_WIDTH, FAF_NUM_EFFECTS);

    // Add gas
    faf_object_spawn_gas(scene, FAF_DIMENSIONS, collision_bodies,
                         FAF_ROAD_WIDTH, FAF_NUM_GAS);

    // Add obstacles
    faf_object_spawn_obstacles(scene, FAF_DIMENSIONS, collision_bodies, FAF_ROAD_WIDTH,
                               FAF_NUM_OBSTACLES, type);

    // Add the road
    for (int i = 0; i < (int)(FAF_ROAD_WIDTH / FAF_BLOCK_WIDTH); i++) {
        for (int j = 0; j < (int)(FAF_DIMENSIONS.y / FAF_BLOCK_LENGTH); j++) {
            surface_info_t *surf_info = faf_surface_init(FAF_ROAD_COEF);
            body_t *road = shape_init_rectangle(FAF_BLOCK_WIDTH, FAF_BLOCK_LENGTH, FAF_REGULAR_ROAD_COLOR,
                                                FAF_DEFAULT_DENSITY, surf_info, free);
            vector_t center = {.x = FAF_SIDE_WIDTH + FAF_BLOCK_WIDTH / 2 + i * FAF_BLOCK_WIDTH,
                               .y = FAF_BLOCK_LENGTH / 2 + j * FAF_BLOCK_LENGTH};
            body_set_centroid(road, center);
            scene_add_body_in_layer(scene, road, FAF_FOREGROUND_LAYER);
            list_add(collision_bodies, road);
        }
    }

    // Add stripes on the road
    for (double curr_y = 0; curr_y < 0.995 * FAF_DIMENSIONS.y; curr_y += FAF_ROAD_STRIPE_SPACING) {
        for (size_t i = 1; i < FAF_ROAD_LANES; i++) {
            faf_object_t *obj_type = malloc(sizeof(faf_object_t));
            *obj_type = FAF_OTHER_OBJ;
            body_t *stripe = shape_init_rectangle(FAF_ROAD_STRIPE_WIDTH, FAF_ROAD_STRIPE_HEIGHT, 
                                                  FAF_ROAD_STRIPE_COLOR, FAF_DEFAULT_DENSITY,
                                                  obj_type, free);
            double dist_from_side = (FAF_DIMENSIONS.x - FAF_ROAD_WIDTH) / 2;
            double curr_x = i * FAF_ROAD_WIDTH / FAF_ROAD_LANES + dist_from_side;
            vector_t center = {.x = curr_x, .y = curr_y};
            body_set_centroid(stripe, center);
            scene_add_body_in_layer(scene, stripe, FAF_FOREGROUND_LAYER);
        }
    }

    // Add the background
    for (int i = 0; i < (int)(FAF_SIDE_WIDTH / FAF_BLOCK_WIDTH); i++) {
        for (int j = 0; j < (int)(FAF_DIMENSIONS.y / FAF_BLOCK_LENGTH); j++) {
            // Left side
            surface_info_t *surf_l_info = faf_surface_init(side_coef);
            body_t *background_left = shape_init_rectangle(FAF_BLOCK_WIDTH, FAF_BLOCK_LENGTH, side_color,
                                                            FAF_DEFAULT_DENSITY, surf_l_info, free);
            vector_t center_l = {.x = FAF_BLOCK_WIDTH / 2 + i * FAF_BLOCK_WIDTH, .y = FAF_BLOCK_LENGTH / 2 + j * FAF_BLOCK_LENGTH};
            body_set_centroid(background_left, center_l);
            scene_add_body_in_layer(scene, background_left, FAF_BACKGROUND_LAYER);
            list_add(collision_bodies, background_left);
            // Right side
            surface_info_t *surf_r_info = faf_surface_init(side_coef);
            body_t *background_right = shape_init_rectangle(FAF_BLOCK_WIDTH, FAF_BLOCK_LENGTH, side_color,
                                                            FAF_DEFAULT_DENSITY, surf_r_info, free);
            vector_t center_r = {.x = FAF_ROAD_WIDTH + FAF_SIDE_WIDTH + FAF_BLOCK_WIDTH / 2 + i * FAF_BLOCK_WIDTH,
                                .y = FAF_BLOCK_LENGTH / 2 + j * FAF_BLOCK_LENGTH};
            body_set_centroid(background_right, center_r);
            scene_add_body_in_layer(scene, background_right, FAF_BACKGROUND_LAYER);
            list_add(collision_bodies, background_right);
        }
    }

    // Add finish line
    faf_object_t *obj_type = malloc(sizeof(faf_object_t));
    *obj_type = FAF_OTHER_OBJ;
    body_t *finish_line = shape_init_rectangle_with_sprite(FAF_FINISH_LINE_DIMENSIONS.x, FAF_FINISH_LINE_DIMENSIONS.y,
                                                           FAF_FINISH_LINE_COLOR, FAF_DEFAULT_DENSITY, obj_type, free,
                                                           "assets/object/FinishLine.png", FAF_FINISH_LINE_DIMENSIONS);
    vector_t center = {.x = FAF_DIMENSIONS.x / 2, .y = FAF_DIMENSIONS.y - 3 * FAF_FINISH_LINE_DIMENSIONS.y / 2};
    body_set_centroid(finish_line, center);
    scene_add_body_in_layer(scene, finish_line, FAF_FOREGROUND_LAYER);

    // Add all cars to collision bodies
    for (size_t i = 0; i < list_size(cars); i++) {
        body_t *car = list_get(cars, i);
        list_add(collision_bodies, car);
    }

    // Register all cars for collision with bodies
    double *aux = malloc(sizeof(double));
    *aux = FAF_ELASTICITY;
    for (size_t i = 0; i < list_size(cars); i++) {
        body_t *car = list_get(cars, i);
        for (size_t j = 0; j < list_size(collision_bodies); j++) {
            body_t *other = list_get(collision_bodies, j);
            create_collision(scene, car, other, (collision_handler_t)faf_car_on_hit, aux, free);
        }
    }

    // Register all AI colliders for collision with bodies
    for (size_t i = 0; i < list_size(ai_colliders); i++) {
        body_t *collider = list_get(ai_colliders, i);
        for (size_t j = 0; j < list_size(collision_bodies); j++) {
            body_t *other = list_get(collision_bodies, j);
            create_collision(scene, collider, other, (collision_handler_t)faf_ai_collider_on_hit, aux, free);
        }
    }

    list_free(collision_bodies);

    return scene;
}