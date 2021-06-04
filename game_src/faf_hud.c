#include "faf_cars.h"
#include "faf_hud.h"
#include "list.h"
#include "mathlib.h"
#include "scene.h"
#include "shape.h"
#include <assert.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

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

const SDL_Rect SPEEDOMETER_LOC = {.x = 925, .y = 100, .w = 100, .h = 100};
const char *SPEEDOMETER_FILENAME = "assets/hud/Speedometer.png";
const double SPEEDOMETER_MAX_SPEED = 300;
const char *SPEEDOMETER_NEEDLE_FILENAME = "assets/hud/SpeedometerNeedle.png";
const SDL_Rect SPEEDOMETER_NEEDLE_LOC = {.x = 925, .y = 100, .w = 73, .h = 60};

const SDL_Rect GAS_INDICATOR_LOC = {.x = 925, .y = 200, .w = 125, .h = 60};
const char *GAS_INDICATOR_FILENAME = "assets/hud/FuelGauge.png";
const char *GAS_INDICATOR_NEEDLE_FILENAME = "assets/hud/FuelGaugeNeedle.png";
const SDL_Rect GAS_INDICATOR_NEEDLE_LOC = {.x = 925, .y = 180, .w = 100, .h = 83};

void widget_tick_speedometer(widget_t *speed_wid) {
    assert(speed_wid);

    body_t *car = widget_get_aux(speed_wid);
    assert(car);
    double speed = body_get_velocity(car).y;
    double angle = M_PI + (speed / SPEEDOMETER_MAX_SPEED * (3 * M_PI / 2));
    angle = mathlib_min(angle, 5 * M_PI / 2);
    angle = angle * 180. / M_PI;
    widget_set_angle(speed_wid, angle);
}

void faf_hud_add_speedometer(hud_t *hud, body_t *car) {
    assert(hud);
    assert(car);

    SDL_Surface *background_surface = IMG_Load(SPEEDOMETER_FILENAME);
    widget_t *background = widget_init(background_surface, SPEEDOMETER_LOC, 0, NULL, NULL, NULL);
    hud_add_widget(hud, background);


    SDL_Surface *needle_surface = IMG_Load(SPEEDOMETER_NEEDLE_FILENAME);
    widget_t *needle = widget_init(needle_surface, SPEEDOMETER_NEEDLE_LOC, (3 * M_PI / 4), widget_tick_speedometer, car, NULL);
    hud_add_widget(hud, needle);
}

void widget_tick_gas(widget_t *gas_wid) {
    assert(gas_wid);
    body_t *car = widget_get_aux(gas_wid);
    assert(car);

    double curr_gas = faf_car_get_curr_gas(car);
    double max_gas = faf_car_get_max_gas(car);

    double angle = (curr_gas / max_gas - 1) * (11 * M_PI / 12) + M_PI / 5;
    angle = angle * 180. / M_PI;
    widget_set_angle(gas_wid, angle);
}

void faf_hud_add_gastank(hud_t *hud, body_t *car) {
    assert(hud);
    assert(car);

    SDL_Surface *tank_surface = IMG_Load(GAS_INDICATOR_FILENAME);
    widget_t *background = widget_init(tank_surface, GAS_INDICATOR_LOC, 0, NULL, NULL, NULL);
    hud_add_widget(hud, background);

    SDL_Surface *needle_surface = IMG_Load(GAS_INDICATOR_NEEDLE_FILENAME);
    widget_t *needle = widget_init(needle_surface, GAS_INDICATOR_NEEDLE_LOC, 0, widget_tick_gas, car, NULL);
    hud_add_widget(hud, needle);
}

void widget_tick_place(widget_t *place_wid) {
    assert(place_wid);
    list_t *cars = widget_get_aux(place_wid);
    assert(cars);

    size_t place = list_size(cars);
    double player_y = body_get_centroid((body_t *)list_get(cars, 0)).y;
    for (size_t i = 1; i < list_size(cars); i++) {
        double car_y = body_get_centroid((body_t *)list_get(cars, i)).y;
        if (player_y > car_y) {
            place--;
        }
    }

    SDL_Color c;
    char *plc_text = malloc(sizeof(char) * 4);

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
    TTF_Font *font = TTF_OpenFont("assets/fonts/Sansation-Bold.ttf", FAF_FONT_XXL);
    assert(font);
    SDL_Surface *txt = TTF_RenderText_Solid(font, plc_text, c);
    assert(txt);
    SDL_Rect plc_rect = {.x = 75, .y = 75, .w = txt->w, .h = txt->h};
    widget_set_surface(place_wid, txt);
    widget_set_rect(place_wid, plc_rect);
    TTF_CloseFont(font);
    free(plc_text);
}

void faf_hud_add_place(hud_t *hud, list_t *cars) {
    assert(hud);
    assert(cars);

    SDL_Rect place_rect = {.x = 75, .y = 75, .w = 100, .h = 100};
    widget_t *wid = widget_init(NULL, place_rect, 0, widget_tick_place, cars, NULL);
    hud_add_widget(hud, wid);
}

void widget_tick_time(widget_t *time_wid) {
    assert(time_wid);
    body_t *car = widget_get_aux(time_wid);
    assert(car);

    SDL_Color color = FAF_DARKRED_C;
    char time_text[100];
    double time = mathlib_max(faf_car_get_time(car), 0);
    size_t mins = (size_t)(time / 60.);
    size_t secs = (size_t)(time) - (mins * 60);
    size_t msecs = (size_t)(time * 1000.) - (secs * 1000) - (mins * 1000 * 60);
    sprintf(time_text, "%zdm %zd.%03zds", mins, secs, msecs);

    TTF_Font *font = TTF_OpenFont("assets/fonts/Sansation-Bold.ttf", FAF_FONT_MEDIUM);
    assert(font);
    SDL_Surface *txt = TTF_RenderText_Solid(font, time_text, color);
    assert(txt);
    SDL_Rect plc_rect = {.x = 75, .y = 125, .w = txt->w, .h = txt->h};
    widget_set_surface(time_wid, txt);
    widget_set_rect(time_wid, plc_rect);
    TTF_CloseFont(font);
}

void faf_hud_add_time(hud_t *hud, body_t *car) {
    assert(hud);
    assert(car);

    SDL_Rect time_rect = {.x = 75, .y = 200, .w = 100, .h = 100};
    widget_t *wid = widget_init(NULL, time_rect, 0, widget_tick_time, car, NULL);
    hud_add_widget(hud, wid);
}

hud_t *faf_make_race_hud(list_t *cars) {
    assert(cars);
    body_t *player_car = (body_t *)list_get(cars, 0);

    hud_t *hud = hud_init(NULL, NULL);

    faf_hud_add_speedometer(hud, player_car);
    faf_hud_add_gastank(hud, player_car);
    faf_hud_add_place(hud, cars);
    faf_hud_add_time(hud, player_car);

    return hud;
}