#include "body.h"
#include "color.h"
#include "faf_levels.h"
#include "faf_objects.h"
#include "mathlib.h"
#include "shape.h"
#include <assert.h>

const double OBJECT_DENSITY = 1.;

const double DECORATION_RADIUS = 45.;
const rgb_color_t DECORATION_COLOR = {.r = 0, .g = 1, .b = 0};
const double EFFECT_RADIUS = 30.;
const rgb_color_t EFFECT_COLOR = {.r = 0, .g = 0, .b = 1};
const double GAS_RADIUS = 30.;
const rgb_color_t GAS_COLOR = {.r = 1, .g = 0, .b = 0};
const double OBSTACLE_RADIUS = 50.;
const rgb_color_t OBSTACLE_COLOR = {.r = 0, .g = 0, .b = 0};

const size_t DECORATIONS = 5;
const char *DECORATION_OPTIONS[5] = {"assets/object/Cactus.png",
                                     "assets/object/Cactus1.png",
                                     "assets/object/Snake.png",
                                     "assets/object/Snowman.png",
                                     "assets/object/Tree.png"};

const size_t EFFECTS = 6;
const char *EFFECTS_OPTIONS[6] = {"assets/object/Speed.png",
                                  "assets/object/Strength.png",
                                  "assets/object/GreenEnergy.png",
                                  "assets/object/Spikes.png",
                                  "assets/object/Gasleak.png",
                                  "assets/object/BananaPeel.png"};

const size_t OBSTACLES = 4;
const char *OBSTACLE_OPTIONS[4] = {"assets/object/Rock.png",
                                   "assets/object/Ice.png",
                                   "assets/object/Rock1.png",
                                   "assets/object/Rock2.png"};

typedef struct faf_object_info {
    faf_object_t object_type;
    faf_effect_t effect_type;
} faf_object_info_t;

faf_object_info_t *make_info(faf_object_t type, faf_effect_t effect_type) {
    faf_object_info_t *info = malloc(sizeof(faf_object_info_t));
    assert(info);
    info->object_type = type;
    info->effect_type = effect_type;
    return info;
}

faf_effect_t faf_objects_get_effect_type(faf_object_info_t *info) {
    return info->effect_type;
}

vector_t generate_position_on_road(vector_t scene_dim, double road_width, double object_radius) {
    double x = mathlib_rand_in_range((scene_dim.x - road_width) / 2 + object_radius,
                                     road_width + (scene_dim.x - road_width) / 2 - object_radius);
    double y = mathlib_rand_in_range(object_radius + 0.05 * scene_dim.y, scene_dim.y - object_radius);
    return (vector_t){.x = x, .y = y};
}

vector_t generate_position_off_road(vector_t scene_dim, double road_width, double object_radius) {
    double x = mathlib_rand_in_range(object_radius, scene_dim.x - object_radius);
    // Generate new x coordinate while it is not a valid postion (on the road)
    while (x > (scene_dim.x - road_width) / 2 - object_radius &&
           x < road_width + (scene_dim.x - road_width) / 2 + object_radius) {
        x = mathlib_rand_in_range(object_radius, scene_dim.x - object_radius);
    }
    double y = mathlib_rand_in_range(object_radius + 0.02 * scene_dim.y, scene_dim.y - object_radius);
    return (vector_t){.x = x, .y = y};
}

vector_t object_position(vector_t scene_dim, double road_width, double object_radius, list_t *list,
                         position_generator_t position_generator) {
    vector_t center = position_generator(scene_dim, road_width, object_radius);
    // Create a reference body
    body_t *ref_body = shape_init_circle(object_radius, (rgb_color_t){.r = 0, .g = 0, .b = 0},
                                         OBJECT_DENSITY, NULL, NULL);
    body_set_centroid(ref_body, center);
    for (size_t i = 0; i < list_size(list); i++) {
        body_t *body = list_get(list, i);
        // Generate new position if there is an overlap
        if (body_are_overlapping(body, ref_body)) {
            center = position_generator(scene_dim, road_width, object_radius);
            body_set_centroid(ref_body, center);
            i = 0;
        }
    }
    body_free(ref_body);
    return center;
}

void spawn_and_register_item(scene_t *scene, vector_t scene_dim, list_t *list,
                             double road_width, double obj_radius, const char *filename,
                             faf_object_t obj_type, faf_effect_t effect_type,
                             position_generator_t position_generator, rgb_color_t obj_color) {
    faf_object_info_t *info = make_info(obj_type, effect_type);
    body_t *item = shape_init_circle_with_sprite(obj_radius, obj_color, OBJECT_DENSITY,
                                                 info, (free_func_t)free, filename,
                                                 (vector_t){.x = obj_radius * 2, .y = obj_radius * 2});
    vector_t center = object_position(scene_dim, road_width, obj_radius, list, position_generator);
    body_set_centroid(item, center);
    list_add(list, item);
    scene_add_body_in_layer(scene, item, FAF_OBJECT_LAYER);
}

void faf_object_spawn_decorations(scene_t *scene, vector_t scene_dim, list_t *collision_bodies,
                                  double road_width, size_t num_decorations, faf_level_t level) {
    for (size_t i = 0; i < num_decorations; i++) {
        int idx;
        switch (level) {
            case DESERT_LEVEL: {
                idx = (int)mathlib_rand_in_range(0, 3 - 1e-4);
                break;
            }
            case ICE_LEVEL: {
                idx = 3;
                break;
            }
            case FOREST_LEVEL: {
                idx = 4;
                break;
            }
        }
        spawn_and_register_item(scene, scene_dim, collision_bodies, road_width, DECORATION_RADIUS,
                                DECORATION_OPTIONS[idx], FAF_DECORATION_OBJ, FAF_NULL,
                                generate_position_off_road, DECORATION_COLOR);
    }
}

void faf_object_spawn_effects(scene_t *scene, vector_t scene_dim, list_t *collision_bodies,
                              double road_width, size_t num_effects) {
    for (size_t i = 0; i < num_effects; i++) {
        int idx = (int)mathlib_rand_in_range(0, EFFECTS - 1e-4);
        const char *option = EFFECTS_OPTIONS[idx];
        faf_effect_t effect;
        switch (idx) {
            case 0: {
                effect = FAF_SPEED;
                break;
            }
            case 1: {
                effect = FAF_STRENGTH;
                break;
            }
            case 2: {
                effect = FAF_GREEN_ENERGY;
                break;
            }
            case 3: {
                effect = FAF_SLOWDOWN;
                break;
            }
            case 4: {
                effect = FAF_GASLEAK;
                break;
            }
            case 5: {
                effect = FAF_LOSE_CONTROL;
                break;
            }
        }
        spawn_and_register_item(scene, scene_dim, collision_bodies, road_width, EFFECT_RADIUS, option,
                                FAF_EFFECT_OBJ, effect, generate_position_on_road, EFFECT_COLOR);
    }
}

void faf_object_spawn_gas(scene_t *scene, vector_t scene_dim, list_t *collision_bodies,
                          double road_width, size_t num_gas) {
    for (size_t i = 0; i < num_gas; i++) {
        spawn_and_register_item(scene, scene_dim, collision_bodies, road_width, GAS_RADIUS,
                                "assets/object/Gas.png", FAF_GAS_OBJ, FAF_NULL,
                                generate_position_on_road, GAS_COLOR);
    }
}

void faf_object_spawn_obstacles(scene_t *scene, vector_t scene_dim, list_t *collision_bodies,
                                double road_width, size_t num_obstacles, faf_level_t level) {
    for (size_t i = 0; i < num_obstacles; i++) {
        int idx;
        switch (level) {
            case DESERT_LEVEL: {
                idx = 0;
                break;
            }
            case ICE_LEVEL: {
                idx = 1;
                break;
            }
            case FOREST_LEVEL: {
                idx = (int)mathlib_rand_in_range(2, OBSTACLES - 1e-4);
                break;
            }
            default: {
                break;
            }
        }
        spawn_and_register_item(scene, scene_dim, collision_bodies, road_width, OBSTACLE_RADIUS, OBSTACLE_OPTIONS[idx],
                                FAF_OBSTACLE_OBJ, FAF_NULL, generate_position_on_road, OBSTACLE_COLOR);
    }
}