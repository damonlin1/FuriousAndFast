#ifndef __FAF_OBJECT_H__
#define __FAF_OBJECT_H__

#include "scene.h"
#include "vector.h"

typedef enum {
    FAF_CAR_OBJ,
    FAF_SURFACE_OBJ,
    FAF_OBSTACLE_OBJ,
    FAF_EFFECT_OBJ,
    FAF_GAS_OBJ,
    FAF_DECORATION_OBJ,
    FAF_OTHER_OBJ
} faf_object_t;

typedef enum {
    FAF_SPEED,
    FAF_STRENGTH,
    FAF_GREEN_ENERGY,
    FAF_SLOWDOWN,
    FAF_GASLEAK,
    FAF_LOSE_CONTROL,
    FAF_NULL
} faf_effect_t;

// Information about the generic object
typedef struct faf_object_info faf_object_info_t;

/**
 * Returns the type of effect for a faf_object_info_t if there is one.
 * 
 * @param info the info
 * @return the type of effect or FAF_NULL if there isn't one
 */
faf_effect_t faf_objects_get_effect_type(faf_object_info_t *info);

/**
 * A function called to generate a position.
 * 
 * @param scene_dim the dimensions of the scene
 * @param road_width the width of the road in the scene
 * @param object_radius the object's radius
 * @return a vector of where to spawn the object
 */
typedef vector_t position_generator_t(vector_t scene_dim, double road_width, double object_radius);

/**
 * Spawns decorations into a level on the side of the road.
 * 
 * @param scene the scene to spawn the decorations in
 * @param scene_dim the dimensions of the scene
 * @param collision_bodies the list of collision bodies to add to
 * @param road_width the width of the road in the scene
 * @param num_decorations the number of decorations to spawn
 * @param level the level for the decorations
 */
void faf_object_spawn_decorations(scene_t *scene, vector_t scene_dim, list_t *collision_bodies,
                                  double road_width, size_t num_decorations, faf_level_t level);

/**
 * Spawns effects into a level.
 * 
 * @param scene the scene to spawn the effects in
 * @param scene_dim the dimensions of the scene
 * @param collision_bodies the list of collision bodies to add to
 * @param road_width the width of the road in the scene
 * @param num_effects the number of effects to spawn
 */
void faf_object_spawn_effects(scene_t *scene, vector_t scene_dim, list_t *collision_bodies,
                              double road_width, size_t num_effects);

/**
 * Spawns gas into a level.
 * 
 * @param scene the scene to spawn the gas in
 * @param scene_dim the dimensions of the scene
 * @param collision_bodies the list of collision bodies to add to
 * @param road_width the width of the road in the scene
 * @param num_gas the number of gas to spawn
 */
void faf_object_spawn_gas(scene_t *scene, vector_t scene_dim, list_t *collision_bodies,
                          double road_width, size_t num_gas);

/**
 * Spawns obstacles into a level.
 * 
 * @param scene the scene to spawn the obstacles in
 * @param scene_dim the dimensions of the scene
 * @param collision_bodies the list of collision bodies to add to
 * @param road_width the width of the road in the scene
 * @param num_obstacles the number of obstacles to spawn
 * @param level the level for the obstacles
 */
void faf_object_spawn_obstacles(scene_t *scene, vector_t scene_dim, list_t *collision_bodies,
                                double road_width, size_t num_obstacles, faf_level_t level);

#endif // #ifndef __FAF_OBJECT_H__