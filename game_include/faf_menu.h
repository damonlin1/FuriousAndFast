#ifndef __FAF_MENU_H__
#define __FAF_MENU_H__

#include "window.h"
#include "faf_levels.h"
#include "hud.h"
#include "vector.h"

extern const vector_t FAF_WINDOW_DIMENSIONS;

window_t *faf_game_start();

hud_t *faf_make_leaderboard_hud();

/**
 * Get the difficulty of the ai_cars.
 * @return 1: easy; 2: medium; 3: hard 
 */
int faf_get_difficulty();

/**
 * Set the difficulty of the ai_cars.
 * @param difficulty 1: easy; 2: medium; 3: hard 
 */
void faf_set_difficulty(int difficulty);

#endif // #ifndef __FAF_MENU_H__