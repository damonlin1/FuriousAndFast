#ifndef __FAF_LEVELS_H__
#define __FAF_LEVELS_H__

#include "scene.h"

// Scene layer definitions
extern const size_t FAF_HIDDEN_LAYER;
extern const size_t FAF_BACKGROUND_LAYER;
extern const size_t FAF_FOREGROUND_LAYER;
extern const size_t FAF_OBJECT_LAYER;
extern const size_t FAF_CAR_LAYER;

// Different levels in the game
typedef enum {
    DESERT_LEVEL = 0,
    ICE_LEVEL = 1,
    FOREST_LEVEL = 2
} faf_level_t;

/**
 * Returns the dimensions of the scene.
 * 
 * @return a vector of the scene dimensions
 */
vector_t faf_get_scene_dimensions();

/**
 * Returns the width of the road.
 * 
 * @return the road width
 */
double faf_get_road_width();

/**
 * Creates and returns the scene for a level with player and AI cars.
 * 
 * @param type the type of level to create
 * @param cars the list of cars in the level
 * @param ai_colliders the list of AI colliders in the level
 * @return the scene for the level
 */
scene_t *faf_make_level(faf_level_t type, list_t *cars, list_t *ai_colliders);

#endif // #ifndef __FAF_LEVELS_H__