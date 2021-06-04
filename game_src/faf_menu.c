#include "color.h"
#include "faf_audio.h"
#include "faf_cars.h"
#include "faf_hud.h"
#include "faf_leaderboard.h"
#include "faf_menu.h"
#include "mathlib.h"
#include "sdl_wrapper.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

extern const vector_t FAF_WINDOW_DIMENSIONS;

extern const char *FAF_LOADING_TEXT;
extern const char *FAF_START_RACE;
extern const char *FAF_VIEW_LDBS;
extern const char *FAF_VIEW_OPTIONS;
extern const char *FAF_TRACK_SELECT;
extern const char *FAF_CAR_SELECT;
extern const char *FAF_DIFFICULTY_SELECT;
extern const char *FAF_GAME_OVER;
extern const char *FAF_RACE_OVER;
extern const char *FAF_OUT_OF_GAS;

extern const SDL_Color FAF_WHITE_C;
extern const SDL_Color FAF_BLACK_C;
extern const SDL_Color FAF_RED_C;
extern const SDL_Color FAF_DARKRED_C;
extern const SDL_Color FAF_DARKBLUE_C;
extern const SDL_Color FAF_GREEN_C;
extern const SDL_Color FAF_GOLD_C;
extern const SDL_Color FAF_SILVER_C;
extern const SDL_Color FAF_BRONZE_C;

extern const int FAF_FONT_MEDIUM;
extern const int FAF_FONT_LARGE;
extern const int FAF_FONT_XLARGE;
extern const int FAF_FONT_XXL;
extern const int FAF_FONT_XXXL;

const vector_t FAF_MENU_OPTION_DIMS = {.x = 450, .y = 75};

const double FAF_TRACK_LEN = 20000;

const size_t FAF_NUM_CARS = 6;

const faf_car_t CAR_TYPES[7] = {FERRARI_488_GTE, PORSCHE_911, BUGATTI_CHIRON, MERCEDES_SLS_AMG,
                                BMW_I8, LAMBORGHINI_HURACAN_EVO_SPYDER, ASTON_MARTON_VANQUISH};
const int NUM_CAR_TYPES = 7;
const char CAR_PREVIEWS[7][50] = {
    "assets/car/Ferrari488GTE.png",
    "assets/car/Porsche911.png",
    "assets/car/BugattiChiron.png",
    "assets/car/MercedesSLSAMG.png",
    "assets/car/BMWI8.png",
    "assets/car/LamborghiniHuracanEvoSpyder.png",
    "assets/car/AstonMartinVanquish.png"
};
const char CAR_NAMES[7][50] = {
    "Ferrari 488 GTE",
    "Porsche 911",
    "Bugatti Chiron",
    "Mercedes SLS AMG",
    "BMW i8",
    "Lamborghini Huracan EVO",
    "Aston Martin Vanquish"
};
const char *CAR_POWER = "Power:";
const char *CAR_HANDLING = "Handling:";
const char *CAR_EFFICIENCY = "Efficiency:";
const size_t CAR_POWERS[7] = {
    4,
    2, 
    5,
    3,
    4,
    4,
    3
};
const size_t CAR_HANDLINGS[7] = {
    2,
    5, 
    4,
    3,
    5,
    2,
    4
};
const size_t CAR_EFFICIENCIES[7] = {
    2,
    2, 
    4,
    5,
    3,
    4,
    3
};

const size_t NUM_LEVELS = 3;
const char LEVEL_PREVIEWS[3][50] = {
    "assets/menus/DesertPreview.png",
    "assets/menus/IcePreview.png",
    "assets/menus/ForestPreview.png"
};
const char LEVEL_DESCRIPTIONS[3][50] = {
    "Desert Dunes",
    "Frozen Tundra",
    "Overgrown Forest"
};
const faf_level_t LEVEL_TYPES[3] = {
    DESERT_LEVEL,
    ICE_LEVEL,
    FOREST_LEVEL
};

const double RACE_START_DELAY = 4.5;
const vector_t RACE_FOCUS_OFFSET = {.x = 0, .y = 100};

const size_t MAIN_MENU_NUM_OPTIONS = 2;

const char *LOADING_BGOUND_PATH = "assets/menus/FafLoadPage.png";
const char *MAIN_BGOUND_PATH = "assets/menus/MainMenuBG.png";

const int NUM_DIFFICULTIES = 3;
const char DIFFICULTY_DESCRIPTIONS[3][10] = {
    "Easy",
    "Medium",
    "Hard"
};

int DIFFICULTY = 3;
faf_level_t CURR_LEVEL = DESERT_LEVEL;
list_t *CARS_LIST = NULL;

typedef struct faf_menu_info {
    size_t curr_opt_idx;
    size_t max_opt_idx;
    faf_level_t level;
    faf_car_t car;
} faf_menu_info_t;

typedef struct faf_menu_opt {
    size_t idx;
    faf_menu_info_t *parent_info; 
} faf_menu_opt_t;

typedef struct pause_info {
    size_t idx;
    hud_t *old_hud;
    list_t *old_handlers;
} pause_info_t;

typedef struct pause_opt {
    size_t idx;
    pause_info_t *info;
} pause_opt_t;

hud_t *faf_make_main_menu_hud();
void faf_mm_on_key(char key, key_event_type_t type, double held_time, window_t *window);

hud_t *faf_make_game_over_hud() {
    hud_t *hud = hud_init(NULL, NULL);

    // Add background
    SDL_Surface *bgound_img = IMG_Load("assets/menus/MenuBackground.png");
    assert(bgound_img);
    SDL_Rect bgound_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                            .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                            .w = (int)FAF_WINDOW_DIMENSIONS.x, .h = (int)FAF_WINDOW_DIMENSIONS.y};
    widget_t *bgound = widget_init(bgound_img, bgound_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, bgound);

    // Add game over title
    TTF_Font *font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_XXL);
    assert(font);
    SDL_Surface *message = TTF_RenderText_Solid(font, FAF_GAME_OVER, FAF_DARKRED_C);
    assert(message);
    SDL_Rect msg_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                         .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 200),
                         .w = message->w, .h = message->h};
    widget_t *title = widget_init(message, msg_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, title);
    TTF_CloseFont(font);

    // Add out of gas image
    SDL_Surface *oog_img = IMG_Load("assets/menus/OutOfGas.png");
    assert(oog_img);
    SDL_Rect img_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                         .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                         .w = (int)(FAF_WINDOW_DIMENSIONS.x / 2), .h = (int)(FAF_WINDOW_DIMENSIONS.y / 2)};
    widget_t *oog_wid = widget_init(oog_img, img_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, oog_wid);

    // Add message
    font = TTF_OpenFont("assets/fonts/Sansation-Bold.ttf", FAF_FONT_XLARGE);
    assert(font);
    message = TTF_RenderText_Solid(font, FAF_OUT_OF_GAS, FAF_WHITE_C);
    assert(message);
    msg_rect = (SDL_Rect) {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                           .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 200),
                           .w = message->w, .h = message->h};
    widget_t *oog_desc = widget_init(message, msg_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, oog_desc);
    TTF_CloseFont(font);

    return hud;
}

hud_t *faf_make_race_over_hud(faf_level_t level, double time) {
    hud_t *hud = hud_init(NULL, NULL);

    // Add background
    SDL_Surface *bgound_img = IMG_Load("assets/menus/MenuBackground.png");
    assert(bgound_img);
    SDL_Rect bgound_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                            .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                            .w = (int)FAF_WINDOW_DIMENSIONS.x, .h = (int)FAF_WINDOW_DIMENSIONS.y};
    widget_t *bgound = widget_init(bgound_img, bgound_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, bgound);

    // Add race over title
    TTF_Font *font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_XXL);
    assert(font);
    SDL_Surface *message = TTF_RenderText_Solid(font, FAF_RACE_OVER, FAF_GOLD_C);
    assert(message);
    SDL_Rect msg_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                         .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 200),
                         .w = message->w, .h = message->h};
    widget_t *title = widget_init(message, msg_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, title);
    TTF_CloseFont(font);

    // Add flag image
    SDL_Surface *flag_img = IMG_Load("assets/menus/FinishFlag.png");
    assert(flag_img);
    SDL_Rect img_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. - 100),
                         .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 50),
                         .w = (int)(FAF_WINDOW_DIMENSIONS.x / 3.), .h = (int)(FAF_WINDOW_DIMENSIONS.y / 3.)};
    widget_t *flag_wid = widget_init(flag_img, img_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, flag_wid);

    // Add place
    size_t place = list_size(CARS_LIST);
    double player_y = body_get_centroid((body_t *)list_get(CARS_LIST, 0)).y;
    for (size_t i = 1; i < list_size(CARS_LIST); i++) {
        double car_y = body_get_centroid((body_t *)list_get(CARS_LIST, i)).y;
        if (player_y > car_y) {
            place--;
        }
    }
    SDL_Color c;
    char plc_text[4];
    switch (place) {
        case (1): {
            c = FAF_GOLD_C;
            sprintf(plc_text, "%zdst", place);
            break;
        }
        case (2): {
            c = FAF_SILVER_C;
            sprintf(plc_text, "%zdnd", place);
            break;
        }
        case (3): {
            c = FAF_BRONZE_C;
            sprintf(plc_text, "%zdrd", place);
            break;
        }
        default: {
            c = FAF_BRONZE_C;
            sprintf(plc_text, "%zdth", place);
            break;
        }
    }
    font = TTF_OpenFont("assets/fonts/Sansation-Bold.ttf", 100);
    assert(font);
    SDL_Surface *txt = TTF_RenderText_Solid(font, plc_text, c);
    assert(txt);
    SDL_Rect plc_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. + 125),
                         .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 50),
                         .w = txt->w, .h = txt->h};
    widget_t *plc_wid = widget_init(txt, plc_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, plc_wid);
    TTF_CloseFont(font);

    // Add message
    font = TTF_OpenFont("assets/fonts/Sansation-Bold.ttf", FAF_FONT_XLARGE);
    assert(font);
    size_t mins = (size_t)(time / 60.);
    size_t secs = (size_t)(time) - (mins * 60);
    size_t msecs = (size_t)(time * 1000.) - (secs * 1000) - (mins * 1000 * 60);
    char time_buff[100];
    sprintf(time_buff, "Your time: %zdm %zd.%03zds", mins, secs, msecs);
    message = TTF_RenderText_Solid(font, time_buff, FAF_WHITE_C);
    assert(message);
    msg_rect = (SDL_Rect) {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                           .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 50),
                           .w = message->w, .h = message->h};
    widget_t *time_msg = widget_init(message, msg_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, time_msg);
    TTF_CloseFont(font);

    if (faf_update_leaderboard(level, time)) {
        // Add new record text
        font = TTF_OpenFont("assets/fonts/Sansation-Bold.ttf", FAF_FONT_XLARGE);
        assert(font);
        message = TTF_RenderText_Solid(font, "New Record!", FAF_GOLD_C);
        assert(message);
        msg_rect = (SDL_Rect) {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                            .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 125),
                            .w = message->w, .h = message->h};
        widget_t *time_msg = widget_init(message, msg_rect, 0, NULL, NULL, NULL);
        hud_add_widget(hud, time_msg);
        TTF_CloseFont(font);
    }

    return hud;
}

void faf_end_of_race_hud_on_key(char key, key_event_type_t type, double held_time, window_t *window) {
    if (type == KEY_PRESSED) {
        return;
    }

    if (key == ' ' || key == SDLK_ESCAPE || key == SDLK_RETURN) {
        window_clear_key_handlers(window);
        window_set_hud(window, faf_make_main_menu_hud());
        window_add_key_handler(window, (key_handler_t)faf_mm_on_key, window, NULL);
    }
}

void faf_pause_arrow_tick(widget_t *arrow) {
    assert(arrow);
    pause_opt_t *opt = widget_get_aux(arrow);
    assert(opt);

    if (opt->idx == opt->info->idx) {
        SDL_Surface *img = IMG_Load("assets/menus/RightArrow.png");
        widget_set_surface(arrow, img);
    }
    else {
        widget_set_surface(arrow, NULL);
    }
}

hud_t *faf_make_pause_hud(hud_t *old_hud, list_t *old_handlers) {
    assert(old_hud);
    assert(old_handlers);

    pause_info_t *info = malloc(sizeof(pause_info_t));
    info->idx = 1;
    info->old_hud = old_hud;
    info->old_handlers = old_handlers;

    hud_t *hud = hud_init(info, free);

    // Add background
    SDL_Surface *bgound_img = IMG_Load("assets/menus/BlueGray.png");
    assert(bgound_img);
    SDL_Rect bgound_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                            .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                            .w = 250, .h = 300};
    widget_t *bgound = widget_init(bgound_img, bgound_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, bgound);

    // Add title
    TTF_Font *font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_XLARGE);
    assert(font);
    SDL_Surface *message = TTF_RenderText_Solid(font, "Paused", FAF_DARKRED_C);
    assert(message);
    SDL_Rect msg_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                         .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 125),
                         .w = message->w, .h = message->h};
    widget_t *title = widget_init(message, msg_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, title);
    TTF_CloseFont(font);

    // Add options
    font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_LARGE);
    assert(font);
    message = TTF_RenderText_Solid(font, "Resume", FAF_WHITE_C);
    assert(message);
    msg_rect = (SDL_Rect) {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                           .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 50),
                           .w = message->w, .h = message->h};
    widget_t *resume = widget_init(message, msg_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, resume);
    message = TTF_RenderText_Solid(font, "Exit", FAF_WHITE_C);
    assert(message);
    msg_rect = (SDL_Rect) {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                           .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 25),
                           .w = message->w, .h = message->h};
    widget_t *exit = widget_init(message, msg_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, exit);
    TTF_CloseFont(font);

    // Add option arrows
    pause_opt_t *opt = malloc(sizeof(pause_opt_t));
    assert(opt);
    opt->idx = 1;
    opt->info = info;
    SDL_Rect arrow_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. - 105),
                           .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 50),
                           .w = 40, .h = 40};
    widget_t *arrow = widget_init(NULL, arrow_rect, 0, faf_pause_arrow_tick, opt, free);
    hud_add_widget(hud, arrow);
    opt = malloc(sizeof(pause_opt_t));
    assert(opt);
    opt->idx = 2;
    opt->info = info;
    arrow_rect = (SDL_Rect) {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. - 70),
                             .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 25),
                             .w = 40, .h = 40};
    arrow = widget_init(NULL, arrow_rect, 0, faf_pause_arrow_tick, opt, free);
    hud_add_widget(hud, arrow);

    return hud;
}

void faf_pause_on_key(char key, key_event_type_t type, double held_time, window_t *window) {
    assert(window);
    if (type == KEY_PRESSED) {
        return;
    }

    pause_info_t *info = hud_get_aux(window_get_hud(window));
    assert(info);

    switch (key) {
        case (UP_ARROW):
        case (DOWN_ARROW): {
            info->idx = info->idx == 1 ? 2 : 1;
            break;
        }
        case (' '):
        case (SDLK_RETURN): {
            if (info->idx == 1) {
                window_set_key_handlers(window, info->old_handlers);
                window_set_hud(window, info->old_hud);
                scene_resume(window_get_scene(window));
                faf_audio_set_volume(2, 25);
            }
            else {
                list_free(info->old_handlers);
                hud_free(info->old_hud);
                window_clear_key_handlers(window);
                window_set_hud(window, faf_make_main_menu_hud());
                window_add_key_handler(window, (key_handler_t)faf_mm_on_key, window, NULL);
                window_clear_scene(window);
                list_free(CARS_LIST);
                CARS_LIST = NULL;
                faf_audio_end_race();
            }
            break;
        }
    }
}

void faf_car_check_race_over(body_t *car, void *dt) {
    assert(car);

    window_t *window = faf_car_get_window(car);
    assert(window);

    double gas = faf_car_get_curr_gas(car);
    if (gas <= 0) {
        window_clear_key_handlers(window);
        window_set_hud(window, faf_make_game_over_hud());
        window_add_key_handler(window, (key_handler_t)faf_end_of_race_hud_on_key, window, NULL);
        faf_audio_end_race();
        window_clear_scene(window);
        list_free(CARS_LIST);
        CARS_LIST = NULL;
        return;
    }

    double pos = body_get_centroid(car).y;
    if (pos > FAF_TRACK_LEN) {
        window_clear_key_handlers(window);
        window_set_hud(window, faf_make_race_over_hud(CURR_LEVEL, faf_car_get_time(car)));
        window_add_key_handler(window, (key_handler_t)faf_end_of_race_hud_on_key, window, NULL);
        faf_audio_end_race();
        window_clear_scene(window);
        list_free(CARS_LIST);
        CARS_LIST = NULL;
    }
}

void faf_race_on_key(char key, key_event_type_t type, double held_time, window_t *window) {
    assert(window);
    if (type == KEY_PRESSED) {
        return;
    }
    if (key == SDLK_ESCAPE) {
        scene_pause(window_get_scene(window));
        hud_t *pause_hud = faf_make_pause_hud(window_get_hud(window), window_get_key_handlers(window));
        window_clear_key_handlers_no_free(window);
        window_set_hud_no_free(window, pause_hud);
        window_add_key_handler(window, (key_handler_t)faf_pause_on_key, window, NULL);
        faf_audio_set_volume(2, 0);
    }
}

void faf_setup_race(window_t *window, faf_level_t level_type, faf_car_t player_car_type) {
    window_clear_key_handlers(window);
    // Create the cars for the race
    list_t *cars = list_init(FAF_NUM_CARS, NULL);
    CARS_LIST = cars;
    list_t *ai_colliders = list_init(FAF_NUM_CARS - 1, NULL);
    body_t *player_car = faf_make_car(player_car_type, true, -RACE_START_DELAY);
    body_t *player_indicator = faf_make_player_indicator(player_car);
    body_register_tick_func(player_car, (body_func_t)faf_car_check_race_over);
    faf_car_set_window(player_car, window);
    list_add(cars, player_car);
    for (size_t i = 0; i < FAF_NUM_CARS - 1; i++) {
        faf_car_t ai_type = player_car_type;
        while (ai_type == player_car_type) {
            int random_type = (int)mathlib_rand_in_range(0, NUM_CAR_TYPES);
            ai_type = CAR_TYPES[random_type];
        }
        body_t *ai_car = faf_make_car(ai_type, false, -RACE_START_DELAY);
        body_t *ai_collider = faf_make_ai_car_collider(ai_car);
        list_add(cars, ai_car);
        list_add(ai_colliders, ai_collider);
    }

    // Make the race scene
    scene_t *scene = faf_make_level(level_type, cars, ai_colliders);

    // Position the cars and add them to the scene
    double step = faf_get_road_width() / (FAF_NUM_CARS + 1);
    for (size_t i = 0; i < list_size(cars); i++) {
        vector_t car_start_loc = {.x = 150 + (step * (i + 1)), .y = FAF_WINDOW_DIMENSIONS.y / 2.};
        body_set_centroid(list_get(cars, i), car_start_loc);
        scene_add_body_in_layer(scene, list_get(cars, i), FAF_OBJECT_LAYER);
    }

    for (size_t i = 0; i < list_size(ai_colliders); i++) {
        scene_add_body_in_layer(scene, list_get(ai_colliders, i), FAF_HIDDEN_LAYER);
    }

    scene_add_body_in_layer(scene, player_indicator, FAF_CAR_LAYER);

    // Set the window to the scene
    window_clear_key_handlers(window);
    window_set_scene(window, scene, VEC_ZERO);
    window_follow_body(window, player_car, RACE_FOCUS_OFFSET);
    window_add_key_handler(window, (key_handler_t)faf_car_on_key, player_car, NULL);
    window_add_key_handler(window, (key_handler_t)faf_race_on_key, window, NULL);

    // Create the HUD for the race
    hud_t *hud = faf_make_race_hud(cars);
    window_set_hud(window, hud);
    
    list_free(ai_colliders);

    // Start race sounds
    faf_audio_start_race();
}

hud_t *faf_make_loading_hud() {
    hud_t *loading_hud = hud_init(NULL, NULL);

    // Add background
    SDL_Surface *bgound_img = IMG_Load(LOADING_BGOUND_PATH);
    assert(bgound_img);
    SDL_Rect bgound_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                            .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                            .w = (int)FAF_WINDOW_DIMENSIONS.x, .h = (int)FAF_WINDOW_DIMENSIONS.y};
    widget_t *bgound = widget_init(bgound_img, bgound_rect, 0, NULL, NULL, NULL);
    hud_add_widget(loading_hud, bgound);

    // Add loading text
    TTF_Font *font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_XXXL);
    assert(font);
    SDL_Surface *message = TTF_RenderText_Solid(font, FAF_LOADING_TEXT, FAF_RED_C);
    assert(message);
    SDL_Rect text_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                          .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 50),
                          .w = message->w, .h = message->h};
    widget_t *text = widget_init(message, text_rect, 0, NULL, NULL, NULL);
    hud_add_widget(loading_hud, text);
    TTF_CloseFont(font);

    return loading_hud;
}

hud_t *faf_make_instructions_hud(faf_menu_info_t *info) {
    hud_t *hud = hud_init(info, NULL);

    // Add background
    SDL_Surface *bgound_img = IMG_Load("assets/menus/InstructionScreen.png");
    assert(bgound_img);
    SDL_Rect bgound_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                            .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                            .w = (int)FAF_WINDOW_DIMENSIONS.x, .h = (int)FAF_WINDOW_DIMENSIONS.y};
    widget_t *bgound = widget_init(bgound_img, bgound_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, bgound);

    return hud;
}

void faf_instructions_on_key(char key, key_event_type_t type, double held_time, window_t *window) {
    assert(window);
    if (type == KEY_PRESSED) {
        return;
    }
    
    hud_t *hud = window_get_hud(window);
    assert(hud);
    faf_menu_info_t *info = hud_get_aux(hud);
    assert(info);
    
    window_clear_key_handlers(window);
    window_set_hud(window, faf_make_loading_hud());
    sdl_render_window(window);
    faf_setup_race(window, info->level, info->car);
    free(info);
}

void faf_ds_arrow_tick(widget_t *arrow) {
    assert(arrow);
    faf_menu_opt_t *opt = widget_get_aux(arrow);
    assert(opt);

    if (opt->idx == opt->parent_info->curr_opt_idx) {
        SDL_Surface *img = IMG_Load("assets/menus/RightArrow.png");
        widget_set_surface(arrow, img);
    }
    else {
        widget_set_surface(arrow, NULL);
    }
}

hud_t *faf_make_diff_sel_hud(faf_menu_info_t *info) {
    assert(info);
    info->curr_opt_idx = 1;
    info->max_opt_idx = NUM_DIFFICULTIES;

    hud_t *hud = hud_init(info, NULL);

    // Add background
    SDL_Surface *bgound_img = IMG_Load("assets/menus/MenuBackground.png");
    assert(bgound_img);
    SDL_Rect bgound_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                            .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                            .w = (int)FAF_WINDOW_DIMENSIONS.x, .h = (int)FAF_WINDOW_DIMENSIONS.y};
    widget_t *bgound = widget_init(bgound_img, bgound_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, bgound);

    // Add title
    TTF_Font *font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_XXL);
    assert(font);
    SDL_Surface *message = TTF_RenderText_Solid(font, FAF_DIFFICULTY_SELECT, FAF_BLACK_C);
    assert(message);
    SDL_Rect msg_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                         .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 200),
                         .w = message->w, .h = message->h};
    widget_t *title = widget_init(message, msg_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, title);
    TTF_CloseFont(font);

    // Add difficulties
    for (size_t i = 1; i <= NUM_DIFFICULTIES; i++) {
        SDL_Color c;
        double arr_ofs;
        if (i == 1) {
            c = FAF_GREEN_C;
            arr_ofs = 125;
        }
        else if (i == 2) {
            c = FAF_DARKBLUE_C;
            arr_ofs = 175;
        }
        else{
            c = FAF_DARKRED_C;
            arr_ofs = 125;
        }

        font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_XXL);
        assert(font);
        message = TTF_RenderText_Solid(font, DIFFICULTY_DESCRIPTIONS[i - 1], c);
        assert(message);
        msg_rect = (SDL_Rect) {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                               .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 175 - 100 * i),
                               .w = message->w, .h = message->h};
        widget_t *diff_txt = widget_init(message, msg_rect, 0, NULL, NULL, NULL);
        hud_add_widget(hud, diff_txt);
        TTF_CloseFont(font);

        faf_menu_opt_t *opt = malloc(sizeof(faf_menu_opt_t));
        assert(opt);
        opt->idx = i;
        opt->parent_info = info;
        SDL_Rect arr_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. - arr_ofs),
                             .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 175 - 100 * i),
                             .w = 50, .h = 50};
        widget_t *diff_arr = widget_init(NULL, arr_rect, 0, faf_ds_arrow_tick, opt, free);
        hud_add_widget(hud, diff_arr);
    }

    return hud;
}

void faf_ds_on_key(char key, key_event_type_t type, double held_time, window_t *window) {
    assert(window);
    hud_t *hud = window_get_hud(window);
    faf_menu_info_t *info = hud_get_aux(hud);
    if (type == KEY_PRESSED) {
        return;
    }
    
    switch (key) {
        case (UP_ARROW): {
            if (info->curr_opt_idx > 1) {
                info->curr_opt_idx--;
            }
            break;
        }
        case (DOWN_ARROW): {
            if (info->curr_opt_idx < info->max_opt_idx) {
                info->curr_opt_idx++;
            }
            break;
        }
        case (SDLK_RETURN):
        case (' '): {
            DIFFICULTY = (int)info->curr_opt_idx;
            window_clear_key_handlers(window);
            window_set_hud(window, faf_make_instructions_hud(info));
            window_add_key_handler(window, (key_handler_t)faf_instructions_on_key, window, NULL);
            break;
        }
    }
}

void faf_left_arrow_tick(widget_t *arrow) {
    assert(arrow);
    faf_menu_info_t *info = widget_get_aux(arrow);
    assert(info);

    if (info->curr_opt_idx > 1) {
        widget_set_surface(arrow, IMG_Load("assets/menus/LeftArrow.png"));
    }
    else {
        widget_set_surface(arrow, NULL);
    }
}

void faf_right_arrow_tick(widget_t *arrow) {
    assert(arrow);
    faf_menu_info_t *info = widget_get_aux(arrow);
    assert(info);

    if (info->curr_opt_idx < info->max_opt_idx) {
        widget_set_surface(arrow, IMG_Load("assets/menus/RightArrow.png"));
    }
    else {
        widget_set_surface(arrow, NULL);
    }
}

void faf_car_preview_tick(widget_t *preview) {
    assert(preview);
    faf_menu_info_t *info = widget_get_aux(preview);
    assert(info);

    const char *path = CAR_PREVIEWS[info->curr_opt_idx - 1];
    widget_set_surface(preview, IMG_Load(path));
}

void faf_car_description_tick(widget_t *description) {
    assert(description);
    faf_menu_info_t *info = widget_get_aux(description);
    assert(info);

    const char *desc = CAR_NAMES[info->curr_opt_idx - 1];
    TTF_Font *font = TTF_OpenFont("assets/fonts/Sansation-Bold.ttf", FAF_FONT_XLARGE);
    assert(font);
    SDL_Surface *message = TTF_RenderText_Solid(font, desc, FAF_BLACK_C);
    widget_set_surface(description, message);
    TTF_CloseFont(font);
    
    if (!message) {
        return;
    }

    SDL_Rect text_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                          .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 60),
                          .w = message->w, .h = message->h};
    widget_set_rect(description, text_rect);
}

void faf_power_star_tick(widget_t *star) {
    assert(star);
    faf_menu_opt_t *info = widget_get_aux(star);
    assert(info);
    faf_menu_info_t *parent_info = info->parent_info;
    assert(parent_info);

    size_t power = CAR_POWERS[parent_info->curr_opt_idx - 1];
    if (power >= info->idx) {
        widget_set_surface(star, IMG_Load("assets/menus/Star.png"));
    }
    else {
        widget_set_surface(star, NULL);
    }
}

void faf_handling_star_tick(widget_t *star) {
    assert(star);
    faf_menu_opt_t *info = widget_get_aux(star);
    assert(info);
    faf_menu_info_t *parent_info = info->parent_info;
    assert(parent_info);

    size_t handling = CAR_HANDLINGS[parent_info->curr_opt_idx - 1];
    if (handling >= info->idx) {
        widget_set_surface(star, IMG_Load("assets/menus/Star.png"));
    }
    else {
        widget_set_surface(star, NULL);
    }
}

void faf_efficiency_star_tick(widget_t *star) {
    assert(star);
    faf_menu_opt_t *info = widget_get_aux(star);
    assert(info);
    faf_menu_info_t *parent_info = info->parent_info;
    assert(parent_info);

    size_t efficiency = CAR_EFFICIENCIES[parent_info->curr_opt_idx - 1];
    if (efficiency >= info->idx) {
        widget_set_surface(star, IMG_Load("assets/menus/Star.png"));
    }
    else {
        widget_set_surface(star, NULL);
    }
}

hud_t *faf_make_car_select_hud(faf_menu_info_t *info) {
    assert(info);
    info->curr_opt_idx = 1;
    info->max_opt_idx = NUM_CAR_TYPES;
    hud_t *hud = hud_init(info, NULL);

    // Add background
    SDL_Surface *bgound_img = IMG_Load("assets/menus/MenuBackground.png");
    assert(bgound_img);
    SDL_Rect bgound_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                            .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                            .w = (int)FAF_WINDOW_DIMENSIONS.x, .h = (int)FAF_WINDOW_DIMENSIONS.y};
    widget_t *bgound = widget_init(bgound_img, bgound_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, bgound);

    // Add left/right arrows
    SDL_Rect left_rect = {.x = 80,
                          .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                          .w = 125, .h = 150};
    widget_t *left_arr = widget_init(NULL, left_rect, 0, faf_left_arrow_tick, info, NULL);
    hud_add_widget(hud, left_arr);
    SDL_Rect right_rect = {.x = 920,
                           .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                           .w = 125, .h = 150};
    widget_t *right_arr = widget_init(NULL, right_rect, 0, faf_right_arrow_tick, info, NULL);
    hud_add_widget(hud, right_arr);

    // Add car preview
    SDL_Rect preview_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                             .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 50),
                             .w = 100, .h = 225};
    widget_t *preview = widget_init(NULL, preview_rect, 0, faf_car_preview_tick, info, NULL);
    hud_add_widget(hud, preview);

    // Add car description
    SDL_Rect text_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                          .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 60),
                          .w = 0, .h = 0};
    widget_t *description = widget_init(NULL, text_rect, 0, faf_car_description_tick, info, NULL);
    hud_add_widget(hud, description);

    // Add title
    TTF_Font *font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_XXL);
    assert(font);
    SDL_Surface *message = TTF_RenderText_Solid(font, FAF_CAR_SELECT, FAF_BLACK_C);
    assert(message);
    SDL_Rect msg_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                         .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 200),
                         .w = message->w, .h = message->h};
    widget_t *title = widget_init(message, msg_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, title);
    TTF_CloseFont(font);

    // Add power description
    font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_LARGE);
    assert(font);
    message = TTF_RenderText_Solid(font, CAR_POWER, FAF_DARKRED_C);
    assert(message);
    SDL_Rect pwr_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. - 65),
                         .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 125),
                         .w = message->w, .h = message->h};
    widget_t *power = widget_init(message, pwr_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, power);
    TTF_CloseFont(font);
    for (size_t i = 1; i <= 5; i++) {
        faf_menu_opt_t *star_info = malloc(sizeof(faf_menu_opt_t));
        assert(star_info);
        star_info->idx = i;
        star_info->parent_info = info;
        SDL_Rect star_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. - 20 + 40 * i),
                              .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 125),
                              .w = 40, .h = 40};
        widget_t *star = widget_init(NULL, star_rect, 0, faf_power_star_tick, star_info, free);
        hud_add_widget(hud, star);
    }

    // Add handling description
    font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_LARGE);
    assert(font);
    message = TTF_RenderText_Solid(font, CAR_HANDLING, FAF_DARKBLUE_C);
    assert(message);
    SDL_Rect hdl_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. - 90),
                         .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 175),
                         .w = message->w, .h = message->h};
    widget_t *handling = widget_init(message, hdl_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, handling);
    TTF_CloseFont(font);
    for (size_t i = 1; i <= 5; i++) {
        faf_menu_opt_t *star_info = malloc(sizeof(faf_menu_opt_t));
        assert(star_info);
        star_info->idx = i;
        star_info->parent_info = info;
        SDL_Rect star_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. - 20 + 40 * i),
                              .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 175),
                              .w = 40, .h = 40};
        widget_t *star = widget_init(NULL, star_rect, 0, faf_handling_star_tick, star_info, free);
        hud_add_widget(hud, star);
    }

    // Add efficiency description
    font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_LARGE);
    assert(font);
    message = TTF_RenderText_Solid(font, CAR_EFFICIENCY, FAF_GREEN_C);
    assert(message);
    SDL_Rect efc_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. - 105),
                         .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 225),
                         .w = message->w, .h = message->h};
    widget_t *efficiency = widget_init(message, efc_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, efficiency);
    TTF_CloseFont(font);
     for (size_t i = 1; i <= 5; i++) {
        faf_menu_opt_t *star_info = malloc(sizeof(faf_menu_opt_t));
        assert(star_info);
        star_info->idx = i;
        star_info->parent_info = info;
        SDL_Rect star_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. - 20 + 40 * i),
                              .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 225),
                              .w = 40, .h = 40};
        widget_t *star = widget_init(NULL, star_rect, 0, faf_efficiency_star_tick, star_info, free);
        hud_add_widget(hud, star);
    }
    
    return hud;
}

void faf_cs_on_key(char key, key_event_type_t type, double held_time, window_t *window) {
    assert(window);
    hud_t *hud = window_get_hud(window);
    faf_menu_info_t *info = hud_get_aux(hud);
    if (type == KEY_PRESSED) {
        return;
    }
    
    switch (key) {
        case (LEFT_ARROW): {
            if (info->curr_opt_idx > 1) {
                info->curr_opt_idx--;
            }
            break;
        }
        case (RIGHT_ARROW): {
            if (info->curr_opt_idx < info->max_opt_idx) {
                info->curr_opt_idx++;
            }
            break;
        }
        case (SDLK_RETURN):
        case (' '): {
            faf_car_t car = CAR_TYPES[info->curr_opt_idx - 1];
            info->car = car;
            window_clear_key_handlers(window);
            window_set_hud(window, faf_make_diff_sel_hud(info));
            window_add_key_handler(window, (key_handler_t)faf_ds_on_key, window, NULL);
            break;
        }
    }
}

void faf_level_preview_tick(widget_t *preview) {
    assert(preview);
    faf_menu_info_t *info = widget_get_aux(preview);
    assert(info);

    const char *path = LEVEL_PREVIEWS[info->curr_opt_idx - 1];
    widget_set_surface(preview, IMG_Load(path));
}

void faf_level_description_tick(widget_t *description) {
    assert(description);
    faf_menu_info_t *info = widget_get_aux(description);
    assert(info);

    const char *desc = LEVEL_DESCRIPTIONS[info->curr_opt_idx - 1];
    TTF_Font *font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_XLARGE);
    assert(font);
    SDL_Surface *message = TTF_RenderText_Solid(font, desc, FAF_BLACK_C);
    widget_set_surface(description, message);
    TTF_CloseFont(font);
    
    if (!message) {
        return;
    }

    SDL_Rect text_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                          .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 200),
                          .w = message->w, .h = message->h};
    widget_set_rect(description, text_rect);
}

hud_t *faf_make_level_select_hud(faf_menu_info_t *info) {
    assert(info);
    info->curr_opt_idx = 1;
    info->max_opt_idx = NUM_LEVELS;
    hud_t *hud = hud_init(info, NULL);

    // Add background
    SDL_Surface *bgound_img = IMG_Load("assets/menus/MenuBackground.png");
    assert(bgound_img);
    SDL_Rect bgound_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                            .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                            .w = (int)FAF_WINDOW_DIMENSIONS.x, .h = (int)FAF_WINDOW_DIMENSIONS.y};
    widget_t *bgound = widget_init(bgound_img, bgound_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, bgound);

    // Add left/right arrows
    SDL_Rect left_rect = {.x = 80,
                          .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                          .w = 125, .h = 150};
    widget_t *left_arr = widget_init(NULL, left_rect, 0, faf_left_arrow_tick, info, NULL);
    hud_add_widget(hud, left_arr);
    SDL_Rect right_rect = {.x = 920,
                           .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                           .w = 125, .h = 150};
    widget_t *right_arr = widget_init(NULL, right_rect, 0, faf_right_arrow_tick, info, NULL);
    hud_add_widget(hud, right_arr);

    // Add level preview
    SDL_Rect preview_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                             .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                             .w = (int)(FAF_WINDOW_DIMENSIONS.x / 1.5), .h = (int)(FAF_WINDOW_DIMENSIONS.y / 1.5)};
    widget_t *preview = widget_init(NULL, preview_rect, 0, faf_level_preview_tick, info, NULL);
    hud_add_widget(hud, preview);

    // Add level description
    SDL_Rect text_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                          .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. - 200),
                          .w = 0, .h = 0};
    widget_t *description = widget_init(NULL, text_rect, 0, faf_level_description_tick, info, NULL);
    hud_add_widget(hud, description);

    // Add title
    TTF_Font *font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_XXL);
    assert(font);
    SDL_Surface *message = TTF_RenderText_Solid(font, FAF_TRACK_SELECT, FAF_BLACK_C);
    assert(message);
    SDL_Rect msg_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                         .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 200),
                         .w = message->w, .h = message->h};
    widget_t *title = widget_init(message, msg_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, title);
    TTF_CloseFont(font);

    return hud;
}

void faf_ls_on_key(char key, key_event_type_t type, double held_time, window_t *window) {
    assert(window);
    hud_t *hud = window_get_hud(window);
    faf_menu_info_t *info = hud_get_aux(hud);
    if (type == KEY_PRESSED) {
        return;
    }
    
    switch (key) {
        case (LEFT_ARROW): {
            if (info->curr_opt_idx > 1) {
                info->curr_opt_idx--;
            }
            break;
        }
        case (RIGHT_ARROW): {
            if (info->curr_opt_idx < info->max_opt_idx) {
                info->curr_opt_idx++;
            }
            break;
        }
        case (SDLK_RETURN):
        case (' '): {
            faf_level_t level = LEVEL_TYPES[info->curr_opt_idx - 1];
            info->level = level;
            CURR_LEVEL = level;
            window_clear_key_handlers(window);
            window_set_hud(window, faf_make_car_select_hud(info));
            window_add_key_handler(window, (key_handler_t)faf_cs_on_key, window, NULL);
            break;
        }
    }
}

void faf_menu_opt_tick(widget_t *wid) {
    assert(wid);
    faf_menu_opt_t *info = widget_get_aux(wid);
    assert(info);

    if (info->parent_info->curr_opt_idx == info->idx) {
        widget_set_surface(wid, IMG_Load("assets/menus/DarkRed.png"));
    }
    else {
        widget_set_surface(wid, IMG_Load("assets/menus/Gray.png"));
    }
}

void faf_leaderboard_on_key(char key, key_event_type_t type, double held_time, window_t *window) {
    assert(window);
    hud_t *hud = window_get_hud(window);
    faf_lb_hud_t *info = hud_get_aux(hud);
    assert(info);

    if (type == KEY_PRESSED) {
        return;
    }

    switch (key) {
        case (LEFT_ARROW): {
            if (info->idx > 0) {
                info->idx--;
                faf_leaderboard_hud_update(hud);
            }
            break;
        }
        case (RIGHT_ARROW): {
            if (info->idx < NUM_LEVELS - 1) {
                info->idx++;
                faf_leaderboard_hud_update(hud);
            }
            break;
        }
        case (SDLK_ESCAPE):
        case (SDLK_RETURN):
        case (' '): {
            window_clear_key_handlers(window);
            window_set_hud(window, faf_make_main_menu_hud());
            window_add_key_handler(window, (key_handler_t)faf_mm_on_key, window, NULL);
            break;
        }
    }
}

hud_t *faf_make_main_menu_hud() {
    faf_menu_info_t *info = malloc(sizeof(faf_menu_info_t));
    assert(info);
    info->curr_opt_idx = 1;
    info->max_opt_idx = MAIN_MENU_NUM_OPTIONS;
    info->level = DESERT_LEVEL;
    info->car = LAMBORGHINI_HURACAN_EVO_SPYDER;
    hud_t *hud = hud_init(info, NULL);

    SDL_Surface *bgound_img = IMG_Load(MAIN_BGOUND_PATH);
    assert(bgound_img);
    SDL_Rect bgound_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                            .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                            .w = (int)FAF_WINDOW_DIMENSIONS.x, .h = (int)FAF_WINDOW_DIMENSIONS.y};
    widget_t *bgound = widget_init(bgound_img, bgound_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, bgound);

    faf_menu_opt_t *opt_info = malloc(sizeof(faf_menu_opt_t));
    opt_info->idx = 1;
    opt_info->parent_info = info;
    SDL_Rect bg_rect = {.x = 500, .y = 250, .w = (int)FAF_MENU_OPTION_DIMS.x, .h = (int)FAF_MENU_OPTION_DIMS.y};
    widget_t *bg = widget_init(NULL, bg_rect, 0, faf_menu_opt_tick, opt_info, free);
    hud_add_widget(hud, bg);
    TTF_Font *font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_LARGE);
    assert(font);
    SDL_Surface *message = TTF_RenderText_Solid(font, FAF_START_RACE, FAF_WHITE_C);
    assert(message);
    SDL_Rect text_rect = {.x = 500,
                          .y = 250,
                          .w = message->w, .h = message->h};
    widget_t *text = widget_init(message, text_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, text);
    TTF_CloseFont(font);

    opt_info = malloc(sizeof(faf_menu_opt_t));
    opt_info->idx = 2;
    opt_info->parent_info = info;
    bg_rect = (SDL_Rect) {.x = 500, .y = 150, .w = (int)FAF_MENU_OPTION_DIMS.x, .h = (int)FAF_MENU_OPTION_DIMS.y};
    bg = widget_init(NULL, bg_rect, 0, faf_menu_opt_tick, opt_info, free);
    hud_add_widget(hud, bg);
    font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_LARGE);
    assert(font);
    message = TTF_RenderText_Solid(font, FAF_VIEW_LDBS, FAF_WHITE_C);
    assert(message);
    text_rect = (SDL_Rect) {.x = 500,
                            .y = 150,
                            .w = message->w, .h = message->h};
    text = widget_init(message, text_rect, 0, NULL, NULL, NULL);
    hud_add_widget(hud, text);

    return hud;
}

void faf_mm_on_key(char key, key_event_type_t type, double held_time, window_t *window) {
    assert(window);
    hud_t *hud = window_get_hud(window);
    faf_menu_info_t *info = hud_get_aux(hud);
    if (type == KEY_PRESSED) {
        return;
    }
    
    switch (key) {
        case (UP_ARROW): {
            info->curr_opt_idx = info->curr_opt_idx == 1 ? info->max_opt_idx : info->curr_opt_idx - 1;
            break;
        }
        case (DOWN_ARROW): {
            info->curr_opt_idx = info->curr_opt_idx == info->max_opt_idx ? 1 : info->curr_opt_idx + 1;
            break;
        }
        case (SDLK_RETURN):
        case (' '): {
            switch(info->curr_opt_idx) {
                // Start race
                case (1): {
                    window_clear_key_handlers(window);
                    window_set_hud(window, faf_make_level_select_hud(info));
                    window_add_key_handler(window, (key_handler_t)faf_ls_on_key, window, NULL);
                    break;
                }
                // View leaderboards
                case (2): {
                    window_clear_key_handlers(window);
                    window_set_hud(window, faf_make_leaderboard_hud());
                    window_add_key_handler(window, (key_handler_t)faf_leaderboard_on_key, window, NULL);
                    break;
                }
            }
            break;
        }
    }
}

int faf_get_difficulty() {
    return DIFFICULTY;
}

void faf_set_difficulty(int difficulty) {
    DIFFICULTY = difficulty;
}

window_t *faf_game_start() {
    scene_t *loading_scene = scene_init(FAF_WINDOW_DIMENSIONS);
    hud_t *loading_hud = faf_make_loading_hud();
    window_t *window = window_init(loading_scene, VEC_ZERO, FAF_WINDOW_DIMENSIONS);
    window_set_hud(window, loading_hud);
    sdl_render_window(window);
    faf_audio_init();
    window_set_hud(window, faf_make_main_menu_hud());
    window_add_key_handler(window, (key_handler_t)faf_mm_on_key, window, NULL);

    return window;
}