#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "body.h"

/**
 * Initializes and returns a circle with a slice cut out.
 * Asserts that the required memory is allocated.
 * 
 * @param radius the radius of the circle
 * @param sector_angle the angle of the slice that is cut out
 * @param color the color of the circle
 * @param density the density of the circle
 * @param info the info for the circle
 * @param info_freer free function for the info
 * @return a pointer to the initialized circle
 */ 
body_t *shape_init_circle_sector(double radius, double sector_angle, rgb_color_t color,
                                 double density, void *info, free_func_t info_freer);

/**
 * Initializes and returns a circle with a slice cut out with a sprite.
 * Asserts that the required memory is allocated.
 * 
 * @param radius the radius of the circle
 * @param sector_angle the angle of the slice that is cut out
 * @param color the color of the circle
 * @param density the density of the circle
 * @param info the info for the circle
 * @param info_freer free function for the info
 * @param filename filename of the sprite image
 * @param dimensions dimensions of the sprite image
 * @return a pointer to the initialized circle
 */ 
body_t *shape_init_circle_sector_with_sprite(double radius, double sector_angle, rgb_color_t color,
                                             double density, void *info, free_func_t info_freer,
                                             const char *filename, vector_t dimensions);

/**
 * Initializes and returns a full circle
 * Asserts that the required memory is allocated.
 * 
 * @param radius the radius of the circle
 * @param color the color of the circle
 * @param density the density of the circle
 * @param info the info for the circle
 * @param info_freer free function for the info
 * @return a pointer to the initialized circle
 */ 
body_t *shape_init_circle(double radius, rgb_color_t color, double density,
                          void *info, free_func_t info_freer);

/**
 * Initializes and returns a full circle with a sprite
 * Asserts that the required memory is allocated.
 * 
 * @param radius the radius of the circle
 * @param color the color of the circle
 * @param density the density of the circle
 * @param info the info for the circle
 * @param info_freer free function for the info
 * @param filename filename of the sprite image
 * @param dimensions dimensions of the sprite image
 * @return a pointer to the initialized circle
 */ 
body_t *shape_init_circle_with_sprite(double radius, rgb_color_t color, double density, void *info,
                                      free_func_t info_freer, const char *filename, vector_t dimensions);

/**
 * Initializes and returns an rectangle body.
 *
 * @param l the length
 * @param h the height
 * @param color the desired color of the rectangle
 * @param density the density of the rectangle
 * @param info the info for the rectangle
 * @param info_freer free function for the info
 * @return a pointer to the initialized rectangle
 */
body_t *shape_init_rectangle(double l, double h, 
                        rgb_color_t color, double density,
                        void *info, free_func_t info_freer);

/**
 * Initializes and returns an rectangle body with a sprite.
 *
 * @param l the length
 * @param h the height
 * @param color the desired color of the rectangle
 * @param density the density of the rectangle
 * @param info the info for the rectangle
 * @param info_freer free function for the info
 * @param filename filename of the sprite image
 * @param dimensions dimensions of the sprite image
 * @return a pointer to the initialized rectangle
 */
body_t *shape_init_rectangle_with_sprite(double l, double h, rgb_color_t color, double density,
                                         void *info, free_func_t info_freer, const char *filename,
                                         vector_t dimensions);

/**
 * Initializes and returns a triangle for the AI car's collider.
 * 
 * @param ai_car the AI car
 * @return a pointer to the initialized collider
 */
body_t *shape_init_ai_collider(body_t *ai_car);

/**
 * Initializes and returns a triangle to indicate a player's car.
 * 
 * @param player_car the player car
 * @return a pointer to the initialized indicator
 */
body_t *shape_init_player_indicator(body_t *player_car);

#endif // #ifndef __SHAPE_H__