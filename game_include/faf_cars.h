#ifndef __FAF_CARS_H__
#define __FAF_CARS_H__

#include "body.h"
#include "sdl_wrapper.h"
#include "window.h"
#include <stdbool.h>

typedef enum {
    FERRARI_488_GTE,
    PORSCHE_911,
    BUGATTI_CHIRON,
    MERCEDES_SLS_AMG,
    BMW_I8,
    LAMBORGHINI_HURACAN_EVO_SPYDER,
    ASTON_MARTON_VANQUISH
} faf_car_t;

// Information about the surface a car is on
typedef struct surface_info surface_info_t;

/**
 * Creates a surface info struct for a given surface.
 *
 * @param surf_coef the surface coefficient for the surface
 * @return a pointer to the initialized info.
 */
surface_info_t *faf_surface_init(double surf_coef);

/**
 * Creates a car of a given type.
 *
 * @param type the type of car desired.
 * @param is_player_car if the car is owned by a player
 * @param start_time the initial timestamp for the car
 * @return a pointer to the initialized body.
 */
body_t *faf_make_car(faf_car_t type, bool is_player_car, double start_time);

/**
 * Creates the ai_collider that protects the ai_car.
 *
 * @param ai_car the ai_car
 * @return the ai_collider
 */
body_t *faf_make_ai_car_collider(body_t *ai_car);

/**
 * Creates the indicator for the player car.
 * 
 * @param player_car the player car
 * @return the indicator
 */
body_t *faf_make_player_indicator(body_t *player_car);

/**
 * Handles a key press to control a given car.
 *
 * @param key the key pressed or released
 * @param type the type of key event
 * @param held_time the amount of time the key was held
 * @param car the car to control
 */
void faf_car_on_key(char key, key_event_type_t type, double held_time, body_t *car);

/**
 * Returns the amount of gas remaining for a player car.
 *
 * @param car the player car to check
 * @return the current gas remianing for the car.
 */
double faf_car_get_curr_gas(body_t *car);

/**
 * Returns the maximum gas for a player car.
 *
 * @param car the player car to check
 * @return the maximum gas for the car.
 */
double faf_car_get_max_gas(body_t *car);

/**
 * On-hit function for collisions with ai_collider.
 * 
 * @param ai_collider the player ai_collider to check
 * @param other the other body in collision
 * @param axis the axis of collision
 * @param aux an auxiliary value
 */
void faf_ai_collider_on_hit(body_t *ai_collider, body_t *other, vector_t axis, void *aux);

/**
 * On-hit function for collisions with a car.
 * 
 * @param car the player car to check
 * @param other the other body in collision
 * @param axis the axis of collision
 * @param aux an auxiliary value
 */
void faf_car_on_hit(body_t *car, body_t *other, vector_t axis, void *aux);

/**
 * Sets the window for the car.
 * 
 * @param car the car
 * @param window the window
 */
void faf_car_set_window(body_t *car, window_t *window);

/**
 * Gets the window for the car.
 * 
 * @param car the car
 * @return the window of the car
 */
window_t *faf_car_get_window(body_t *car);

/**
 * Gets the time elapsed for the car.
 * 
 * @param car the car
 * @return the time elapsed for the car
 */
double faf_car_get_time(body_t *car);

#endif // #ifndef __FAF_CARS_H__