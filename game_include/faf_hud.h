#ifndef __FAF_HUD_H__
#define __FAF_HUD_H__

#include "body.h"
#include "hud.h"
#include <SDL2/SDL_image.h>

/**
 * Creates a new racing HUD.
 *
 * @param cars a list of cars in the scene. The car to track should be at index 0.
 * @return an initialized HUD for the racing scene.
 */
hud_t *faf_make_race_hud(list_t *cars);

#endif // #ifndef __FAF_HUD_H__
