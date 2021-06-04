#include "vector.h"
#include "sdl_wrapper.h"

const vector_t FAF_WINDOW_DIMENSIONS = {.x = 1000, .y = 500};

const char *FAF_LOADING_TEXT = "Loading";
const char *FAF_START_RACE = "Start Race";
const char *FAF_VIEW_LDBS = "View Leaderboards";
const char *FAF_VIEW_OPTIONS = "Options";
const char *FAF_TRACK_SELECT = "Select Your Track";
const char *FAF_CAR_SELECT = "Select Your Car";
const char *FAF_DIFFICULTY_SELECT = "Select Your Difficulty";
const char *FAF_GAME_OVER = "Game Over";
const char *FAF_RACE_OVER = "You Finished the Race!";
const char *FAF_OUT_OF_GAS = "You ran out of fuel!";

const SDL_Color FAF_WHITE_C = {.r = 225, .g = 225, .b = 225, .a = 255};
const SDL_Color FAF_BLACK_C = {.r = 0, .g = 0, .b = 0, .a = 255};
const SDL_Color FAF_RED_C = {.r = 255, .g = 0, .b = 0, .a = 255};
const SDL_Color FAF_DARKRED_C = {.r = 200, .g = 0, .b = 0, .a = 255};
const SDL_Color FAF_DARKBLUE_C = {.r = 0, .g = 0, .b = 125, .a = 255};
const SDL_Color FAF_GREEN_C = {.r = 0, .g = 200, .b = 0, .a = 255};
const SDL_Color FAF_GOLD_C = {.r = 255, .g = 215, .b = 0, .a = 255};
const SDL_Color FAF_SILVER_C = {.r = 211, .g = 211, .b = 211, .a = 255};
const SDL_Color FAF_BRONZE_C = {.r = 225, .g = 127, .b = 75, .a = 255};

const int FAF_FONT_MEDIUM = 24;
const int FAF_FONT_LARGE = 36;
const int FAF_FONT_XLARGE = 48;
const int FAF_FONT_XXL = 60;
const int FAF_FONT_XXXL = 72;