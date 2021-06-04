#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "key_handler.h"
#include "hud.h"
#include "scene.h"
#include "vector.h"

/**
 * A screen viewing a scene.
 */
typedef struct window window_t;

/**
 * Allocates memory for a window.
 * Asserts that the required memory is successfully allocated.
 *
 * @param scene the scene the window is viewing
 * @param center the point in the scene the window is centered on
 * @param dims the dimensions of the window
 * @return the new window
 */
window_t *window_init(scene_t *scene, vector_t center, vector_t dims);

/**
 * Releases memory allocated for a given window and the scene it is viewing.
 *
 * @param window a pointer to a window returned from window_init()
 */
void window_free(window_t *window);

/**
 * Returns the scene viewed by a window.
 *
 * @param window a pointer to a window returned from window_init()
 * @return the scene the given window is viewing.
 */
scene_t *window_get_scene(window_t *window);

/**
 * Sets the scene viewed by a window. The old scene is freed.
 * Also zeroes out the window's velocity focused body.
 *
 * @param window a pointer to a window returned from window_init()
 * @param scene a pointer to a scene return from scene_init()
 * @param new_center the center of the window in the new scene
 */
void window_set_scene(window_t *window, scene_t *new_scene, vector_t new_center);

/**
 * Returns the center of the window.
 *
 * @param window a pointer to a window returned from window_init()
 * @return the center of the window (in scene space)
 */
vector_t window_get_center(window_t *window);

/**
 * Sets the center of the window.
 *
 * @param window a pointer to a window returned from window_init()
 * @param new_center the new center of the window (in scene space)
 */
void window_set_center(window_t *window, vector_t new_center);

/**
 * Returns the dimensions of the window
 *
 * @param window a pointer to a window returned from window_init()
 * @return the dimensions of the window
 */
vector_t window_get_dims(window_t *window);

/**
 * Returns the velocity of the window.
 *
 * @param window a pointer to a window returned from window_init()
 * @return the velocity of the window (in scene space)
 */
vector_t window_get_velocity(window_t *window);

/**
 * Sets the velocity of the window.
 *
 * @param window a pointer to a window returned from window_init()
 * @param new_velocity the new velocity of the window (in scene space)
 */
void window_set_velocity(window_t *window, vector_t new_velocity);

/**
 * Sets the window to stay centered on a given body.
 * Takes precedence over a window's velocity, but does not remove it.
 *
 * @param window a pointer to a window returned from window_init()
 * @param body a pointer to the body to follow.
 * @param focus_offset the offset to follow the body at.
 */
void window_follow_body(window_t *window, body_t *body, vector_t focus_offset);

/**
 * Executes a tick of a given window over a small time interval.
 * This executes scene_tick() for the scene it is viewing.
 *
 * @param window a pointer to a window returned from window_init()
 * @param dt the time elapsed since the last tick, in seconds
 */
void window_tick(window_t *window, double dt);

/**
 * Handles a key press for a window.
 *
 * @param window a pointer to a window returned from window_init()
 * @param key the key pressed
 * @param type the type of key event
 * @param held_time the amount of time the key was held.
 */
void window_on_key(window_t *window, char key, key_event_type_t type, double held_time);

/**
 * Registers a key handler to the window.
 *
 * @param window a pointer to a window returned from window_init()
 * @param f the key handler functions
 * @param aux auxiliary data for f
 * @param aux_freer a function to free aux
 */
void window_add_key_handler(window_t *window, key_handler_t f, void *aux, free_func_t aux_freer);

/**
 * Clears all key handlers registered to a window.
 * 
 * @param window a pointer to a window returned from window_init()
 */
void window_clear_key_handlers(window_t *window);

/**
 * Clears all key handlers registered to a window without freeing them.
 * 
 * @param window a pointer to a window returned from window_init()
 */
void window_clear_key_handlers_no_free(window_t *window);

/**
 * Transforms a vector from scene-space to window-space.
 *
 * @param window the window that the vector lives in
 * @param v the scene-space vector to transform.
 * @return v in winow-space.
 */
vector_t scene_to_window_space(window_t *window, vector_t v);

/**
 * Sets the HUD for a given window.
 *
 * @param window a pointer to a window returned from window_init()
 * @param hud the new HUD for the window
 */
void window_set_hud(window_t *window, hud_t *hud);

/**
 * Sets the HUD for a given window withour freeing the previous HUD.
 * 
 * @param window a pointer to a window returned from window_init()
 * @param hud the new HUD for the window
 */
void window_set_hud_no_free(window_t *window, hud_t *hud);

/**
 * Gets the key handlers for the window.
 * 
 * @param window a pointer to a window returned from window_init()
 * @return the key handlers for the window
 */
list_t *window_get_key_handlers(window_t *window);

/**
 * Sets the key handlers for the window.
 * 
 * @param window a pointer to a window returned from window_init()
 * @param handlers the handler to set for the window
 */
void window_set_key_handlers(window_t *window, list_t *handlers);

/**
 * Gets the HUD for a given window.
 *
 * @param window a pointer to a window returned from window_init()
 * @return the HUD for the window
 */
hud_t *window_get_hud(window_t *window);

/**
 * Clears the scene for the window.
 * 
 * @param window the window to clear the scene for
 */
void window_clear_scene(window_t *window);

#endif // #ifndef __WINDOW_H__
