#ifndef __FAF_LEADERBOARD_H__
#define __FAF_LEADERBOARD_H__

#include "faf_levels.h"
#include "hud.h"
#include <stdbool.h>

// How many records are stored for a given level
extern const size_t FAF_NUM_RECORDS;

// Struct for a single record
typedef struct faf_record faf_record_t;

// Struct for an entire leaderboard for a level
typedef struct faf_leaderboard faf_leaderboard_t;

typedef struct faf_lb_hud {
    size_t idx;
    widget_t *times[5];
} faf_lb_hud_t;

/**
 * Gets the pathname to the file to write to for a level.
 * 
 * @param level the level
 * @return the pathname to the file
 */
char *faf_get_level_fname(faf_level_t level);

/**
 * Saves the leaderboard.
 * 
 * @param leaderboard the leaderboard to save
 */
void faf_save_leaderboard(faf_leaderboard_t *leaderboard);

/**
 * Gets the leaderboard for a given level.
 * 
 * @param level the level
 * @return the leaderboard for the level
 */
faf_leaderboard_t *faf_get_leaderboard(faf_level_t level);

/**
 * Updates the leaderboard for a level.
 * 
 * @param level the level
 * @param time the time of the race
 * @return whether the leaderboard has been updated 
 */
bool faf_update_leaderboard(faf_level_t level, double time);

/**
 * Updates the HUD for the leaderboard.
 * 
 * @param hud the HUD to update
 */
void faf_leaderboard_hud_update(hud_t *hud);

#endif // #ifndef __FAF_LEADERBOARD_H__