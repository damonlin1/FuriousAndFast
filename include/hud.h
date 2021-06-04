#ifndef __HUD_H__
#define __HUD_H__

#include "list.h"
#include <SDL2/SDL_image.h>

/**
 * A hud.
 */
typedef struct hud hud_t;

/**
 * A widget in the hud.
 */
typedef struct widget widget_t;

/**
 * A generic function that can be called on a widget.
 */
typedef void (*widget_func_t)(widget_t *);

/**
 * Initializes a new widget.
 *
 * @param surface an SDL_Surface to display the widget
 * @param orientation an SDL_Rect to display the widget
 * @param angle angle of the widget
 * @param tick_func the tick function for the widget
 * @param aux auxiliary data for the widget
 * @param aux_freer a function to free the auxiliary data
 * @return a reference to the newly initialized widget.
 */
widget_t *widget_init(SDL_Surface *surface, SDL_Rect orientation, double angle, widget_func_t tick_func, void *aux, free_func_t aux_freer);

/**
 * Frees a widget
 *
 * @param widget a pointer returned from widget_init()
 */
void widget_free(widget_t *widget);

/**
 * Returns the aux data from a widget.
 *
 * @param widget a pointer returned from widget_init()
 * @return the aux data of the widget.
 */
void *widget_get_aux(widget_t *widget);

/**
 * Returns the angle of a widget.
 *
 * @param widget a pointer returned from widget_init()
 * @param the angle of the widget
 */
double widget_get_angle(widget_t *widget);

/**
 * Returns the SDL_Surface to render a widget.
 * 
 * @param widget a pointer returned from widget_init()
 * @return the surface to render the widget.
 */
SDL_Surface *widget_get_surface(widget_t *widget);

/**
 * Returns the SDL_Rect to render a widget.
 * 
 * @param widget a pointer returned from widget_init()
 * @return the rectangle to render the widget.
 */
SDL_Rect widget_get_rect(widget_t *widget);

/**
 * Sets the angle in the widget.
 *
 * @param widget a pointer returned from widget_init()
 * @param angle angle to set the widget to 
 */
void widget_set_angle(widget_t *widget, double angle);

/**
 * Sets the SDL_Surface of a widget
 * 
 * @param widget a pointer returned from widget_init()
 * @param surface the surface that you want to update the widget with
 */
void widget_set_surface(widget_t *widget, SDL_Surface *surface);

/**
 * Sets the SDL_Rect of a widget
 * 
 * @param widget a pointer returned from widget_init()
 * @param coordinates the SDL_Rect that you want to update the widget with
 */
void widget_set_rect(widget_t *widget, SDL_Rect coordinates);

/**
 * Initializes a new HUD.
 * 
 * @param aux the auxiliary argument to pass in
 * @param aux_freer a function for freeing the aux
 * @return the newly initialized HUD
 */
hud_t *hud_init(void *aux, free_func_t aux_freer);

/**
 * Frees a HUD.
 *
 * @param hud a pointer returned from hud_init()
 */
void hud_free(hud_t *hud);

/**
 * Adds a widget to a HUD.
 *
 * @param hud a pointer returned from hud_init()
 * @param widget the widget to add.
 */
void hud_add_widget(hud_t *hud, widget_t *widget);

/**
 * Ticks a HUD.
 *
 * @param hud a pointer returned from hud_init()
 */
void hud_tick(hud_t *hud);

/**
 * Returns a reference to the HUD's widgets.
 *
 * @param hud a pointer returned from hud_init()
 * @return a reference to the list of widgets for the hud
 */
list_t *hud_get_widgets(hud_t *hud);

/**
 * Returns the auxiliary value of the HUD.
 * 
 * @param hud a pointer returned from hud_init()
 * @return the auxiliary value of the HUD.
 */
void *hud_get_aux(hud_t *hud);


#endif // #ifndef __HUD_H__