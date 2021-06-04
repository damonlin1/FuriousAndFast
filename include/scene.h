#ifndef __SCENE_H__
#define __SCENE_H__

#include "body.h"
#include "list.h"
#include "vector.h"

/**
 * A collection of bodies and force creators.
 * The scene automatically resizes to store
 * arbitrarily many bodies and force creators.
 */
typedef struct scene scene_t;

/**
 * A function which adds some forces or impulses to bodies,
 * e.g. from collisions, gravity, or spring forces.
 * Takes in an auxiliary value that can store parameters or state.
 * 
 * @param aux an auxiliary value
 */
typedef void (*force_creator_t)(void *aux);

/**
 * Allocates memory for an empty scene.
 * Makes a reasonable guess of the number of bodies to allocate space for.
 * Asserts that the required memory is successfully allocated.
 *
 * @param dimensions the maximum dimensions of the scene
 * @return the new scene
 */
scene_t *scene_init(vector_t dimensions);

/**
 * Releases memory allocated for a given scene
 * and all the bodies and force creators it contains.
 *
 * @param scene a pointer to a scene returned from scene_init()
 */
void scene_free(scene_t *scene);

/**
 * Returns the number of layers in a scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @return the number of layers in the scene.
 */
size_t scene_num_layers(scene_t *scene);

/**
 * Returns a reference to a given layer in a scene.
 * DOES NOT create a deep copy of the body list.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param idx the layer to get in [0, scene_num_layers())
 */
list_t *scene_get_layer(scene_t *scene, size_t idx);

/**
 * Gets the number of bodies in a given scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @return the number of bodies added with scene_add_body()
 */
size_t scene_num_bodies(scene_t *scene);

/**
 * Adds a body to a scene on the default layer (1).
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param body a pointer to the body to add to the scene
 */
void scene_add_body(scene_t *scene, body_t *body);

/**
 * Adds a body to a scene at a specified layer.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param layer_no the layer number to add the body at
 * @param body a pointer to the body to add to the scene
 */
void scene_add_body_in_layer(scene_t *scene, body_t *body, size_t layer_no);

/**
 * Returns the dimensions of the scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @return the maximum dimensions of the scene
 */
vector_t scene_get_dimensions(scene_t *scene);

/**
 * Sets the dimensions of the scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param dimensions the new maximum dimensions of the scene
 */
void scene_set_dimensions(scene_t *scene, vector_t dimensions);

/**
 * Adds a force creator to a scene,
 * to be invoked every time scene_tick() is called.
 * The auxiliary value is passed to the force creator each time it is called.
 * The force creator is registered with a list of bodies it applies to,
 * so it can be removed when any one of the bodies is removed.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param forcer a force creator function
 * @param aux an auxiliary value to pass to forcer when it is called
 * @param bodies the list of bodies affected by the force creator.
 *   The force creator will be removed if any of these bodies are removed.
 *   This list does not own the bodies, so its freer should be NULL.
 * @param freer if non-NULL, a function to call in order to free aux
 */
void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies, free_func_t freer);

/**
 * Executes a tick of a given scene over a small time interval.
 * This requires executing all the force creators
 * and then ticking each body (see body_tick()).
 * If any bodies are marked for removal, they should be removed from the scene
 * and freed, along with any force creators acting on them.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param dt the time elapsed since the last tick, in seconds
 */
void scene_tick(scene_t *scene, double dt);

/**
 * Pauses the scene from ticking.
 * 
 * @param scene the scene to pause
 */
void scene_pause(scene_t *scene);

/**
 * Resumes the scene to tick.
 * 
 * @param scene the scene to resume
 */
void scene_resume(scene_t *scene);

/**
 * Reverts the current scene paused state.
 * 
 * @param scene the scene to revert
 */
void scene_toggle_pause(scene_t *scene);

#endif // #ifndef __SCENE_H__
