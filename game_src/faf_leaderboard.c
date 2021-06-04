#include "faf_leaderboard.h"
#include "faf_cars.h"
#include "list.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

extern const vector_t FAF_WINDOW_DIMENSIONS;

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

const size_t FAF_NUM_RECORDS = 5;
const size_t FAF_NUM_LEVELS = 3;
const char LB_LEVEL_DESCRIPTIONS[3][100] = {
    "Desert Dunes Leaderboard",
    "Frozen Tundra Leaderboard",
    "Overgrown Forest Leaderboard"
};

const size_t FAF_LEADERBOARD_PATH_SIZE = 50;

typedef struct faf_record {
    size_t minutes;
    size_t seconds;
    size_t mseconds;
} faf_record_t;

typedef struct faf_leaderboard {
    faf_record_t records[5];
    faf_level_t level;
} faf_leaderboard_t;

char *faf_get_level_fname(faf_level_t level) {
    char *buff = malloc(sizeof(char) * FAF_LEADERBOARD_PATH_SIZE);
    assert(buff);
    sprintf(buff, "./data/leaderboards/%d.ldb", (int)level);
    return buff;
}

void faf_save_leaderboard(faf_leaderboard_t *leaderboard) {
    assert(leaderboard);

    char *fpath = faf_get_level_fname(leaderboard->level);
    FILE *fp = fopen(fpath, "w");
    assert(fp);

    for (size_t i = 0; i < FAF_NUM_RECORDS; i++) {
        faf_record_t r = leaderboard->records[i];
        fprintf(fp, "%zu\n", r.minutes);
        fprintf(fp, "%zu\n", r.seconds);
        fprintf(fp, "%zu\n", r.mseconds);
    }

    free(fpath);
    fclose(fp);
}

faf_leaderboard_t *faf_get_leaderboard(faf_level_t level) {
    char *fpath = faf_get_level_fname(level);
    FILE *fp = fopen(fpath, "r");
    assert(fp);

    faf_leaderboard_t *lb = malloc(sizeof(faf_leaderboard_t));
    assert(lb);
    lb->level = level;

    char buff[4];
    for (size_t i = 0; i < FAF_NUM_RECORDS; i++) {
        faf_record_t r;
        fscanf(fp, "%s", buff);
        r.minutes = atoi(buff);
        fscanf(fp, "%s", buff);
        r.seconds = atoi(buff);
        fscanf(fp, "%s", buff);
        r.mseconds = atoi(buff);

        lb->records[i] = r;
    }

    fclose(fp);
    return lb;
}

bool faf_record_less_than(faf_record_t r1, faf_record_t r2) {
    if (r1.minutes != r2.minutes) {
        return r1.minutes < r2.minutes;
    }

    if (r1.seconds != r2.seconds) {
        return r1.seconds < r2.seconds;
    }

    if (r1.mseconds != r2.mseconds) {
        return r1.mseconds < r2.mseconds;
    }

    return false;
}

bool faf_update_leaderboard(faf_level_t level, double time) {
    faf_leaderboard_t *lb = faf_get_leaderboard(level);
    
    size_t mins = (size_t)(time / 60.);
    size_t secs = (size_t)(time) - (mins * 60);
    size_t msecs = (size_t)(time * 1000.) - (secs * 1000) - (mins * 1000 * 60);
    faf_record_t new_rec = {.minutes = mins, .seconds = secs, .mseconds = msecs};

    size_t place = FAF_NUM_RECORDS;
    while (place > 0 && faf_record_less_than(new_rec, lb->records[place - 1])) {
        place--;
    }

    if (place == FAF_NUM_RECORDS) {
        free(lb);
        return false;
    }

    for (size_t i = FAF_NUM_RECORDS - 1; i > place; i--) {
        lb->records[i] = lb->records[i - 1];
    }

    lb->records[place] = new_rec;

    faf_save_leaderboard(lb);
    free(lb);
    return true;
}

void faf_lb_left_arrow_tick(widget_t *arrow) {
    assert(arrow);
    faf_lb_hud_t *info = widget_get_aux(arrow);
    assert(info);

    if (info->idx > 0) {
        widget_set_surface(arrow, IMG_Load("assets/menus/LeftArrow.png"));
    }
    else {
        widget_set_surface(arrow, NULL);
    }
}

void faf_lb_right_arrow_tick(widget_t *arrow) {
    assert(arrow);
    faf_lb_hud_t *info = widget_get_aux(arrow);
    assert(info);

    if (info->idx < FAF_NUM_LEVELS - 1) {
        widget_set_surface(arrow, IMG_Load("assets/menus/RightArrow.png"));
    }
    else {
        widget_set_surface(arrow, NULL);
    }
}

void faf_lb_level_description_tick(widget_t *description) {
    assert(description);
    faf_lb_hud_t *info = widget_get_aux(description);
    assert(info);

    const char *desc = LB_LEVEL_DESCRIPTIONS[info->idx];
    TTF_Font *font = TTF_OpenFont("assets/fonts/Freedom.ttf", FAF_FONT_XLARGE);
    assert(font);
    SDL_Surface *message = TTF_RenderText_Solid(font, desc, FAF_BLACK_C);
    widget_set_surface(description, message);
    TTF_CloseFont(font);
    
    if (!message) return;

    SDL_Rect text_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                          .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 200),
                          .w = message->w, .h = message->h};
    widget_set_rect(description, text_rect);
}

void faf_leaderboard_hud_update(hud_t *hud) {
    assert(hud);
    faf_lb_hud_t *info = hud_get_aux(hud);
    assert(info);

    faf_level_t level = (faf_level_t)info->idx;
    faf_leaderboard_t *lb = faf_get_leaderboard(level);

    for (size_t i = 0; i < FAF_NUM_RECORDS; i++) {
        faf_record_t rec = lb->records[i];
        TTF_Font *font = TTF_OpenFont("assets/fonts/Sansation-Bold.ttf", FAF_FONT_LARGE);
        assert(font);

        font = TTF_OpenFont("assets/fonts/Sansation-Bold.ttf", FAF_FONT_LARGE);
        assert(font);
        widget_t *time_wid = info->times[i];
        char time[100];
        sprintf(time, "%zdm %zd.%03zds", rec.minutes, rec.seconds, rec.mseconds);
        SDL_Surface *message = TTF_RenderText_Solid(font, time, FAF_WHITE_C);
        widget_set_surface(time_wid, message);
        SDL_Rect time_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. + 25),
                              .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 125 - 75 * i),
                              .w = message->w, .h = message->h};
        widget_set_rect(time_wid, time_rect);
        TTF_CloseFont(font);
    }

    free(lb);
}

hud_t *faf_make_leaderboard_hud() {
    faf_lb_hud_t *info = malloc(sizeof(faf_lb_hud_t));
    assert(info);
    info->idx = 0;
    hud_t *hud = hud_init(info, free);

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
    widget_t *left_arr = widget_init(NULL, left_rect, 0, faf_lb_left_arrow_tick, info, NULL);
    hud_add_widget(hud, left_arr);
    SDL_Rect right_rect = {.x = 920,
                           .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2.),
                           .w = 125, .h = 150};
    widget_t *right_arr = widget_init(NULL, right_rect, 0, faf_lb_right_arrow_tick, info, NULL);
    hud_add_widget(hud, right_arr);

    // Add level description
    SDL_Rect text_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2.),
                          .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 200),
                          .w = 0, .h = 0};
    widget_t *description = widget_init(NULL, text_rect, 0, faf_lb_level_description_tick, info, NULL);
    hud_add_widget(hud, description);

    // Add leaderboard
    for (size_t i = 0; i < FAF_NUM_RECORDS; i++) {

        // Add place
        SDL_Color place_c;
        if (i == 0) {
            place_c = FAF_GOLD_C;
        }
        else if (i == 1) {
            place_c = FAF_SILVER_C;
        }
        else {
            place_c = FAF_BRONZE_C;
        }
        TTF_Font *font = TTF_OpenFont("assets/fonts/Sansation-Bold.ttf", FAF_FONT_LARGE);
        assert(font);
        char place[2];
        sprintf(place, "%zd", i + 1);
        SDL_Surface *message = TTF_RenderText_Solid(font, place, place_c);
        TTF_CloseFont(font);
        SDL_Rect place_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. - 100),
                               .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 125 - 75 * i),
                               .w = message->w, .h = message->h};
        widget_t *place_wid = widget_init(message, place_rect, 0, NULL, NULL, NULL);
        hud_add_widget(hud, place_wid);

        // Add time
        SDL_Rect time_rect = {.x = (int)(FAF_WINDOW_DIMENSIONS.x / 2. + 25),
                              .y = (int)(FAF_WINDOW_DIMENSIONS.y / 2. + 125 - 75 * i),
                              .w = message->w * 9, .h = message->h};
        widget_t *time_wid = widget_init(NULL, time_rect, 0, NULL, NULL, NULL);
        hud_add_widget(hud, time_wid);
        info->times[i] = time_wid;
    }

    faf_leaderboard_hud_update(hud);
    return hud;
}