#ifndef __BODY_H__
#define __BODY_H__

#include "color.h"
#include "list.h"
#include "vector.h"
#include <stdbool.h>
#include <SDL2/SDL_image.h>

/**
 * A rigid body constrained to the plane.
 * Implemented as a polygon with uniform density.
 * Bodies can accumulate forces and impulses during each tick.
 */
typedef struct body body_t;

/**
 * A generic function that can be called on a body.
 * 
 * @param body the body to call the function on
 * @param arg an argument to pass in
 */
typedef void (*body_func_t)(body_t *body, void *arg);

/**
 * Initializes a body without any info.
 * Acts like body_init_with_info() where info and info_freer are NULL.
 * 
 * @param shape a list of vectors describing the initial shape of the body
 * @param mass the mass of the body (if INFINITY, stops the body from moving)
 * @param color the color of the body, used to draw it on the screen
 * @return a pointer to the newly allocated body
 */
body_t *body_init(list_t *shape, double mass, rgb_color_t color);

/**
 * Allocates memory for a body with the given parameters.
 * The body is initially at rest.
 * Asserts that the mass is positive and that the required memory is allocated.
 *
 * @param shape a list of vectors describing the initial shape of the body
 * @param mass the mass of the body (if INFINITY, stops the body from moving)
 * @param color the color of the body, used to draw it on the screen
 * @param info additional information to associate with the body,
 *   e.g. its type if the scene has multiple types of bodies
 * @param info_freer if non-NULL, a function call on the info to free it
 * @return a pointer to the newly allocated body
 */
body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer);

/**
 * Allocates memory for a body with the given parameters and a sprite.
 * The body is initially at rest.
 * Asserts that the mass is positive and that the required memory is allocated.
 *
 * @param shape a list of vectors describing the initial shape of the body
 * @param mass the mass of the body (if INFINITY, stops the body from moving)
 * @param color the color of the body, used to draw it on the screen
 * @param info additional information to associate with the body,
 *   e.g. its type if the scene has multiple types of bodies
 * @param info_freer if non-NULL, a function call on the info to free it
 * @param filename filename of the sprite image
 * @param dimensions dimensions of the sprite image
 * @return a pointer to the newly allocated body
 */
body_t *body_init_with_info_and_sprite(list_t *shape, double mass, rgb_color_t color, void *info,
                                       free_func_t info_freer, const char *filename, vector_t dimensions);

/**
 * Releases the memory allocated for a body.
 *
 * @param body a pointer to a body returned from body_init()
 */
void body_free(body_t *body);

/**
 * Gets the current shape of a body.
 * Returns a newly allocated vector list, which must be list_free()d.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the polygon describing the body's current position
 */
list_t *body_get_shape(body_t *body);

/**
 * Gets the current shape of a body.
 * Returns a reference to the body's polygon.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the polygon describing the body's current position
 */
list_t *body_get_shape_nocpy(body_t *body);

/**
 * Gets the current center of mass of a body.
 * While this could be calculated with polygon_centroid(), that becomes too slow
 * when this function is called thousands of times every tick.
 * Instead, the body should store its current centroid.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's center of mass
 */
vector_t body_get_centroid(body_t *body);

/**
 * Gets the current velocity of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's velocity vector
 */
vector_t body_get_velocity(body_t *body);

/**
 * Gets the mass of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the mass passed to body_init(), which must be greater than 0
 */
double body_get_mass(body_t *body);

/**
 * Gets the display color of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the color passed to body_init(), as an (R, G, B) tuple
 */
rgb_color_t body_get_color(body_t *body);

/**
 * Gets the information associated with a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the info passed to body_init()
 */
void *body_get_info(body_t *body);

/**
 * Returns a body's orientation in the plane.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the orientation of the body
 */
double body_get_rotation(body_t *body);

/**
 * Returns the radius of the smallest sphere that contains the given body.
 *
 * @param body the body to check
 * @return the radius of the smallest sphere that contains the given
 */
double body_get_bounding_radius(body_t *body);

/**
 * Returns the SDL_Surface of the sprite visual attached to the body.
 *
 * @param body the body to check
 * @return the SDL_Surface pointer of the sprite visual attached to the body.
 */
SDL_Surface *body_get_surface(body_t *body);

/**
 * Returns the dimensions of the rendered image.
 *
 * @param body the body to check
 * @return the dimensions of the rendered image
 */
vector_t body_get_dimensions(body_t *body);

/**
 * Returns whether the body is in debug mode.
 *
 * @param body the body to check
 * @return debug_mode true or false
 */
bool body_get_debug_mode(body_t *body);

/**
 * Translates a body to a new position.
 * The position is specified by the position of the body's center of mass.
 *
 * @param body a pointer to a body returned from body_init()
 * @param x the body's new centroid
 */
void body_set_centroid(body_t *body, vector_t x);

/**
 * Changes a body's velocity (the time-derivative of its position).
 *
 * @param body a pointer to a body returned from body_init()
 * @param v the body's new velocity
 */
void body_set_velocity(body_t *body, vector_t v);

/**
 * Changes a body's orientation in the plane.
 * The body is rotated about its center of mass.
 * Note that the angle is *absolute*, not relative to the current orientation.
 *
 * @param body a pointer to a body returned from body_init()
 * @param angle the body's new angle in radians. Positive is counterclockwise.
 */
void body_set_rotation(body_t *body, double angle);

/**
 * Sets the display color of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @param color the new color to display, as an (R, G, B) tuple
 */
void body_set_color(body_t *body, rgb_color_t color);

/**
 * Sets the SDL_Surface of the sprite visual for a body.
 * 
 * @param body the body to set the surface of
 * @param surface the surface to set the body to
 */
void body_set_surface(body_t *body, SDL_Surface *surface);

/**
 * Sets the debug mode to bool mode.
 *
 * @param body the body to check
 * @param mode debug_mode true or false
 */
void body_set_debug_mode(body_t *body, bool mode);

/**
 * Applies a force to a body over the current tick.
 * If multiple forces are applied in the same tick, they should be added.
 * Should not change the body's position or velocity; see body_tick().
 *
 * @param body a pointer to a body returned from body_init()
 * @param force the force vector to apply
 */
void body_add_force(body_t *body, vector_t force);

/**
 * Applies an impulse to a body.
 * An impulse causes an instantaneous change in velocity,
 * which is useful for modeling collisions.
 * If multiple impulses are applied in the same tick, they should be added.
 * Should not change the body's position or velocity; see body_tick().
 *
 * @param body a pointer to a body returned from body_init()
 * @param impulse the impulse vector to apply
 */
void body_add_impulse(body_t *body, vector_t impulse);

/**
 * Calculates the impulse on two bodies during collision.
 * 
 * @param body1 the first body in collision
 * @param body2 the second body in collision
 * @param axis the axis of collision
 * @param elasticity the elasticity of collision (between 0 and 1)
 * @return the impulse exerted on body1
 */
vector_t body_calculate_impulse(body_t *body1, body_t *body2, vector_t axis, double elasticity);

/**
 * Updates the body after a given time interval has elapsed.
 * Sets acceleration and velocity according to the forces and impulses
 * applied to the body during the tick.
 * The body should be translated at the *average* of the velocities before
 * and after the tick.
 * Resets the forces and impulses accumulated on the body.
 *
 * @param body the body to tick
 * @param dt the number of seconds elapsed since the last tick
 */
void body_tick(body_t *body, double dt);

/**
 * Returns if a body appears on the screen bounded by the input vectors.
 *
 * @param body the body to check
 * @param lower_bounds lower left corner of the screen
 * @param upper_bounds upper right corner of the screen
 * @return whether the body appears on the screen
 */
bool body_is_on_screen(body_t *body, vector_t lower_bounds, vector_t upper_bounds);

/**
 * Registers a function to perform every tick on a given body.
 *
 * @param body the body to add the function to
 * @param f the function to register
 */
void body_register_tick_func(body_t *body, body_func_t f);

/**
 * Unregisters a function to perform every tick on a given body.
 *
 * @param body the body to remove the function from
 * @param f the function to unregister
 */
void body_unregister_tick_func(body_t *body, body_func_t f);

/**
 * Checks if two bodies are overlapping.
 *
 * @param body1 a pointer to a body returned from body_init()
 * @param body2 a pointer to a body returned from body_init()
 * @return whether body1 and body2 are overlapping
 */
bool body_are_overlapping(body_t *body1, body_t *body2);

/**
 * Marks a body for removal--future calls to body_is_removed() will return true.
 * Does not free the body.
 * If the body is already marked for removal, does nothing.
 *
 * @param body the body to mark for removal
 */
void body_remove(body_t *body);

/**
 * Returns whether a body has been marked for removal.
 * This function returns false until body_remove() is called on the body,
 * and returns true afterwards.
 *
 * @param body the body to check
 * @return whether body_remove() has been called on the body
 */
bool body_is_removed(body_t *body);

#endif // #ifndef __BODY_H__