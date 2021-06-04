#include "color.h"
#include "faf_audio.h"
#include "faf_cars.h"
#include "faf_levels.h"
#include "faf_menu.h"
#include "faf_objects.h"
#include "forces.h"
#include "mathlib.h"
#include "shape.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Properties of all cars
const double FAF_CAR_DENSITY = 1;
const rgb_color_t FAF_CAR_DEBUG_COLOR = {.r = 1, .g = 0, .b = 0};
const size_t CAR_INIT_NUM_EFFECTS = 3;
const double FAF_CAR_GRAV_CONST = 100;
const double FAF_CAR_EFFECT_TIME = 5;
const vector_t FAF_CAR_DIMENSIONS = {.x = 50, .y = 125};

// Properties of FERRARI 488 GTE
const double FERRARI_488_GTE_ACCEL_dS = 125;
const double FERRARI_488_GTE_BRAKE_dS = -150;
const double FERRARI_488_GTE_TOP_SPEED = 175;
const double FERRARI_488_GTE_SPEED_BOOST = 1.2;
const double FERRARI_488_GTE_TURN_SENSITIVITY = 0.7;
const double FERRARI_488_GTE_GAS_MILAGE = 17;
const double FERRARI_488_GTE_GAS_MAX = 1625;
const char *FERRARI_488_GTE_FILENAME = "assets/car/Ferrari488GTE.png";
const char *FERRARI_488_GTE_FILENAME_FLAMES = "assets/car/Ferrari488GTEFlames.png";

// Properties of PORSCHE 911
const double PORSCHE_911_ACCEL_dS = 95;
const double PORSCHE_911_BRAKE_dS = -120;
const double PORSCHE_911_TOP_SPEED = 160;
const double PORSCHE_911_SPEED_BOOST = 1.8;
const double PORSCHE_911_TURN_SENSITIVITY = 0.95;
const double PORSCHE_911_GAS_MILAGE = 14;
const double PORSCHE_911_GAS_MAX = 1550;
const char *PORSCHE_911_FILENAME = "assets/car/Porsche911.png";
const char *PORSCHE_911_FILENAME_FLAMES = "assets/car/Porsche911Flames.png";

// Properties of BUGATTI CHIRON
const double BUGATTI_CHIRON_ACCEL_dS = 120;
const double BUGATTI_CHIRON_BRAKE_dS = -145;
const double BUGATTI_CHIRON_TOP_SPEED = 190;
const double BUGATTI_CHIRON_SPEED_BOOST = 1.3;
const double BUGATTI_CHIRON_TURN_SENSITIVITY = 0.85;
const double BUGATTI_CHIRON_GAS_MILAGE = 29;
const double BUGATTI_CHIRON_GAS_MAX = 1600;
const char *BUGATTI_CHIRON_FILENAME = "assets/car/BugattiChiron.png";
const char *BUGATTI_CHIRON_FILENAME_FLAMES = "assets/car/BugattiChironFlames.png";

// Properties of MERCEDES SLS AMG
const double MERCEDES_SLS_AMG_ACCEL_dS = 115;
const double MERCEDES_SLS_AMG_BRAKE_dS = -140;
const double MERCEDES_SLS_AMG_TOP_SPEED = 170;
const double MERCEDES_SLS_AMG_SPEED_BOOST = 1.4;
const double MERCEDES_SLS_AMG_TURN_SENSITIVITY = 0.8;
const double MERCEDES_SLS_AMG_GAS_MILAGE = 32;
const double MERCEDES_SLS_AMG_GAS_MAX = 1650;
const char *MERCEDES_SLS_AMG_FILENAME = "assets/car/MercedesSLSAMG.png";
const char *MERCEDES_SLS_AMG_FILENAME_FLAMES = "assets/car/MercedesSLSAMGFlames.png";

// Properties of BMW I8
const double BMW_I8_ACCEL_dS = 100;
const double BMW_I8_BRAKE_dS = -125;
const double BMW_I8_TOP_SPEED = 185;
const double BMW_I8_SPEED_BOOST = 1.5;
const double BMW_I8_TURN_SENSITIVITY = 1.0;
const double BMW_I8_GAS_MILAGE = 23;
const double BMW_I8_GAS_MAX = 1500;
const char *BMW_I8_FILENAME = "assets/car/BMWI8.png";
const char *BMW_I8_FILENAME_FLAMES = "assets/car/BMWI8Flames.png";

// Properties of LAMBORGHINI HURACAN EVO SPYDER
const double LAMBORGHINI_HURACAN_EVO_SPYDER_ACCEL_dS = 110;
const double LAMBORGHINI_HURACAN_EVO_SPYDER_BRAKE_dS = -135;
const double LAMBORGHINI_HURACAN_EVO_SPYDER_TOP_SPEED = 180;
const double LAMBORGHINI_HURACAN_EVO_SPYDER_SPEED_BOOST = 1.6;
const double LAMBORGHINI_HURACAN_EVO_SPYDER_TURN_SENSITIVITY = 0.75;
const double LAMBORGHINI_HURACAN_EVO_SPYDER_GAS_MILAGE = 26;
const double LAMBORGHINI_HURACAN_EVO_SPYDER_GAS_MAX = 1575;
const char *LAMBORGHINI_HURACAN_EVO_SPYDER_FILENAME = "assets/car/LamborghiniHuracanEvoSpyder.png";
const char *LAMBORGHINI_HURACAN_EVO_SPYDER_FILENAME_FLAMES = "assets/car/LamborghiniHuracanEvoSpyderFlames.png";

// Properties of ASTON_MARTON_VANQUISH
const double ASTON_MARTON_VANQUISH_ACCEL_dS = 105;
const double ASTON_MARTON_VANQUISH_BRAKE_dS = -130;
const double ASTON_MARTON_VANQUISH_TOP_SPEED = 165;
const double ASTON_MARTON_VANQUISH_SPEED_BOOST = 1.7;
const double ASTON_MARTON_VANQUISH_TURN_SENSITIVITY = 0.9;
const double ASTON_MARTON_VANQUISH_GAS_MILAGE = 20;
const double ASTON_MARTON_VANQUISH_GAS_MAX = 1525;
const char *ASTON_MARTON_VANQUISH_FILENAME = "assets/car/AstonMartinVanquish.png";
const char *ASTON_MARTON_VANQUISH_FILENAME_FLAMES = "assets/car/AstonMartinVanquishFlames.png";

typedef struct surface_info {
    faf_object_t type;
    double surf_coefficient;
} surface_info_t;

typedef struct car_effect {
    body_func_t f;
    double total_time;
    double time_elapsed;
} car_effect_t;

typedef struct car_info {
    faf_object_t obj_type;
    faf_car_t car_type;
    double accel_dS;
    double brake_dS;
    double top_speed;
    double default_top_speed;
    double speed_boost;
    double turn_sensitivity;
    double surf_coef;
    double gas_curr;
    double gas_max;
    double gas_milage;
    double default_gas_milage;
    double time;
    bool is_player_car;
    bool strength_enabled;
    bool control_enabled;

    bool accelerating;
    bool braking;
    bool turning_right;
    bool turning_left;

    const char *filename;
    vector_t dimensions;
    SDL_Surface *normal;
    SDL_Surface *accelerated;

    list_t *effects;

    window_t *window;
} faf_car_info_t;

surface_info_t *faf_surface_init(double surf_coef) {
    surface_info_t *info = malloc(sizeof(surface_info_t));
    assert(info);

    info->type = FAF_SURFACE_OBJ;
    info->surf_coefficient = surf_coef;

    return info;
}

void free_car_info(faf_car_info_t *info) {
    assert(info);

    free(info);
}

faf_car_info_t *make_car_info(faf_car_t car_type, bool is_player_car, double start_time) {
    faf_car_info_t *info = malloc(sizeof(faf_car_info_t));
    assert(info);

    info->car_type = car_type;
    info->obj_type = FAF_CAR_OBJ;
    info->is_player_car = is_player_car;
    info->strength_enabled = false;
    info->control_enabled = true;
    info->accelerating = false;
    info->braking = false;
    info->turning_right = false;
    info->turning_left = false;
    info->time = start_time;
    info->dimensions = FAF_CAR_DIMENSIONS;
    info->effects = list_init(CAR_INIT_NUM_EFFECTS, free);
    info->window = NULL;

    switch (car_type) {
        case FERRARI_488_GTE: {
            info->accel_dS = FERRARI_488_GTE_ACCEL_dS;
            info->brake_dS = FERRARI_488_GTE_BRAKE_dS;
            info->top_speed = FERRARI_488_GTE_TOP_SPEED;
            info->default_top_speed = FERRARI_488_GTE_TOP_SPEED;
            info->speed_boost = FERRARI_488_GTE_SPEED_BOOST;
            info->turn_sensitivity = FERRARI_488_GTE_TURN_SENSITIVITY;
            info->gas_max = FERRARI_488_GTE_GAS_MAX;
            info->gas_curr = FERRARI_488_GTE_GAS_MAX;
            info->gas_milage = FERRARI_488_GTE_GAS_MILAGE;
            info->default_gas_milage = FERRARI_488_GTE_GAS_MILAGE;
            info->filename = FERRARI_488_GTE_FILENAME;
            info->accelerated = IMG_Load(FERRARI_488_GTE_FILENAME_FLAMES);
            info->normal = IMG_Load(FERRARI_488_GTE_FILENAME);
            return info;
        }
        case PORSCHE_911: {
            info->accel_dS = PORSCHE_911_ACCEL_dS;
            info->brake_dS = PORSCHE_911_BRAKE_dS;
            info->top_speed = PORSCHE_911_TOP_SPEED;
            info->default_top_speed = PORSCHE_911_TOP_SPEED;
            info->speed_boost = PORSCHE_911_SPEED_BOOST;
            info->turn_sensitivity = PORSCHE_911_TURN_SENSITIVITY;
            info->gas_max = PORSCHE_911_GAS_MAX;
            info->gas_curr = PORSCHE_911_GAS_MAX;
            info->gas_milage = PORSCHE_911_GAS_MILAGE;
            info->default_gas_milage = PORSCHE_911_GAS_MILAGE;
            info->filename = PORSCHE_911_FILENAME;
            info->accelerated = IMG_Load(PORSCHE_911_FILENAME_FLAMES);
            info->normal = IMG_Load(PORSCHE_911_FILENAME);
            return info;
        }
        case BUGATTI_CHIRON: {
            info->accel_dS = BUGATTI_CHIRON_ACCEL_dS;
            info->brake_dS = BUGATTI_CHIRON_BRAKE_dS;
            info->top_speed = BUGATTI_CHIRON_TOP_SPEED;
            info->default_top_speed = BUGATTI_CHIRON_TOP_SPEED;
            info->speed_boost = BUGATTI_CHIRON_SPEED_BOOST;
            info->turn_sensitivity = BUGATTI_CHIRON_TURN_SENSITIVITY;
            info->gas_max = BUGATTI_CHIRON_GAS_MAX;
            info->gas_curr = BUGATTI_CHIRON_GAS_MAX;
            info->gas_milage = BUGATTI_CHIRON_GAS_MILAGE;
            info->default_gas_milage = BUGATTI_CHIRON_GAS_MILAGE;
            info->filename = BUGATTI_CHIRON_FILENAME;
            info->accelerated = IMG_Load(BUGATTI_CHIRON_FILENAME_FLAMES);
            info->normal = IMG_Load(BUGATTI_CHIRON_FILENAME);
            return info;
        }
        case MERCEDES_SLS_AMG: {
            info->accel_dS = MERCEDES_SLS_AMG_ACCEL_dS;
            info->brake_dS = MERCEDES_SLS_AMG_BRAKE_dS;
            info->top_speed = MERCEDES_SLS_AMG_TOP_SPEED;
            info->default_top_speed = MERCEDES_SLS_AMG_TOP_SPEED;
            info->speed_boost = MERCEDES_SLS_AMG_SPEED_BOOST;
            info->turn_sensitivity = MERCEDES_SLS_AMG_TURN_SENSITIVITY;
            info->gas_max = MERCEDES_SLS_AMG_GAS_MAX;
            info->gas_curr = MERCEDES_SLS_AMG_GAS_MAX;
            info->gas_milage = MERCEDES_SLS_AMG_GAS_MILAGE;
            info->default_gas_milage = MERCEDES_SLS_AMG_GAS_MILAGE;
            info->filename = MERCEDES_SLS_AMG_FILENAME;
            info->accelerated = IMG_Load(MERCEDES_SLS_AMG_FILENAME_FLAMES);
            info->normal = IMG_Load(MERCEDES_SLS_AMG_FILENAME);
            return info;
        }
        case BMW_I8: {
            info->accel_dS = BMW_I8_ACCEL_dS;
            info->brake_dS = BMW_I8_BRAKE_dS;
            info->top_speed = BMW_I8_TOP_SPEED;
            info->default_top_speed = BMW_I8_TOP_SPEED;
            info->speed_boost = BMW_I8_SPEED_BOOST;
            info->turn_sensitivity = BMW_I8_TURN_SENSITIVITY;
            info->gas_max = BMW_I8_GAS_MAX;
            info->gas_curr = BMW_I8_GAS_MAX;
            info->gas_milage = BMW_I8_GAS_MILAGE;
            info->default_gas_milage = BMW_I8_GAS_MILAGE;
            info->filename = BMW_I8_FILENAME;
            info->accelerated = IMG_Load(BMW_I8_FILENAME_FLAMES);
            info->normal = IMG_Load(BMW_I8_FILENAME);
            return info;
        }
        case LAMBORGHINI_HURACAN_EVO_SPYDER: {
            info->accel_dS = LAMBORGHINI_HURACAN_EVO_SPYDER_ACCEL_dS;
            info->brake_dS = LAMBORGHINI_HURACAN_EVO_SPYDER_BRAKE_dS;
            info->top_speed = LAMBORGHINI_HURACAN_EVO_SPYDER_TOP_SPEED;
            info->default_top_speed = LAMBORGHINI_HURACAN_EVO_SPYDER_TOP_SPEED;
            info->speed_boost = LAMBORGHINI_HURACAN_EVO_SPYDER_SPEED_BOOST;
            info->turn_sensitivity = LAMBORGHINI_HURACAN_EVO_SPYDER_TURN_SENSITIVITY;
            info->gas_max = LAMBORGHINI_HURACAN_EVO_SPYDER_GAS_MAX;
            info->gas_curr = LAMBORGHINI_HURACAN_EVO_SPYDER_GAS_MAX;
            info->gas_milage = LAMBORGHINI_HURACAN_EVO_SPYDER_GAS_MILAGE;
            info->default_gas_milage = LAMBORGHINI_HURACAN_EVO_SPYDER_GAS_MILAGE;
            info->filename = LAMBORGHINI_HURACAN_EVO_SPYDER_FILENAME;
            info->accelerated = IMG_Load(LAMBORGHINI_HURACAN_EVO_SPYDER_FILENAME_FLAMES);
            info->normal = IMG_Load(LAMBORGHINI_HURACAN_EVO_SPYDER_FILENAME);
            return info;
        }
        case ASTON_MARTON_VANQUISH: {
            info->accel_dS = ASTON_MARTON_VANQUISH_ACCEL_dS;
            info->brake_dS = ASTON_MARTON_VANQUISH_BRAKE_dS;
            info->top_speed = ASTON_MARTON_VANQUISH_TOP_SPEED;
            info->default_top_speed = ASTON_MARTON_VANQUISH_TOP_SPEED;
            info->speed_boost = ASTON_MARTON_VANQUISH_SPEED_BOOST;
            info->turn_sensitivity = ASTON_MARTON_VANQUISH_TURN_SENSITIVITY;
            info->gas_max = ASTON_MARTON_VANQUISH_GAS_MAX;
            info->gas_curr = ASTON_MARTON_VANQUISH_GAS_MAX;
            info->gas_milage = ASTON_MARTON_VANQUISH_GAS_MILAGE;
            info->default_gas_milage = ASTON_MARTON_VANQUISH_GAS_MILAGE;
            info->filename = ASTON_MARTON_VANQUISH_FILENAME;
            info->accelerated = IMG_Load(ASTON_MARTON_VANQUISH_FILENAME_FLAMES);
            info->normal = IMG_Load(ASTON_MARTON_VANQUISH_FILENAME);
            return info;
        }
    }
}

void faf_car_on_key(char key, key_event_type_t type, double held_time, body_t *car) {
    assert(car);
    faf_car_info_t *car_info = (faf_car_info_t *) body_get_info(car);
    assert(car_info);

    switch (key) {
        case RIGHT_ARROW: {
            car_info->turning_right = type == KEY_PRESSED;
            break;
        }
        case LEFT_ARROW: {
            car_info->turning_left = type == KEY_PRESSED;
            break;
        } 
        case UP_ARROW: {
            car_info->accelerating = type == KEY_PRESSED;
            break;
        }
        case DOWN_ARROW: {
            car_info->braking = type == KEY_PRESSED;
            break;
        }
    }
}

void car_speed(body_t* car, void *dt) {
    faf_car_info_t *car_info = body_get_info(car);
    car_info->top_speed *= car_info->speed_boost;
}

void car_strength(body_t* car, void *dt) {
    faf_car_info_t *car_info = body_get_info(car);
    car_info->strength_enabled = true;
}

void car_green_energy(body_t* car, void *dt) {
    faf_car_info_t *car_info = body_get_info(car);
    car_info->gas_milage = 0;
}

void car_slowdown(body_t* car, void *dt) {
    faf_car_info_t *car_info = body_get_info(car);
    car_info->top_speed /= 2;
}

void car_gasleak(body_t* car, void *dt) {
    faf_car_info_t *car_info = body_get_info(car);
    car_info->gas_milage *= 2;
}

void car_lose_control(body_t* car, void *dt) {
    faf_car_info_t *car_info = body_get_info(car);
    car_info->control_enabled = false;
}

void car_turn_right(body_t *ai_car, void *dt) {
    vector_t v = body_get_velocity(ai_car);
    faf_car_info_t *info = (faf_car_info_t *)body_get_info(ai_car);

    if (info->control_enabled) {
        if (!info->turning_right) {
            body_set_rotation(ai_car, -M_PI / 12);
            // How much turning should depend on vertical velocity
            v.x += info->turn_sensitivity * fabs(v.y);
            body_set_velocity(ai_car, v);
        }
        info->turning_right = true;  
    } 
}

void car_turn_left(body_t *ai_car, void *dt) {
    vector_t v = body_get_velocity(ai_car);
    faf_car_info_t *info = (faf_car_info_t *)body_get_info(ai_car);

    if (info->control_enabled) {
        if (!info->turning_left) {
            body_set_rotation(ai_car, M_PI / 12);
            // How much turning should depend on vertical velocity
            v.x -= info->turn_sensitivity * fabs(v.y);
            body_set_velocity(ai_car, v);
        }
        info->turning_left = true;
    }
}

void faf_ai_collider_tick(body_t *ai_collider, void *dt) {
    assert(ai_collider);

    body_t *ai_car = body_get_info(ai_collider);
    double displacement = 2.5;
    if (faf_get_difficulty() == 2) {
        displacement = 1.5;
    }
    else if (faf_get_difficulty() == 3) {
        displacement = 1.1;
    }
    
    vector_t centroid = {
        .x = body_get_centroid(ai_car).x,
        .y = body_get_centroid(ai_car).y + (displacement * body_get_dimensions(ai_car).y)
    };
    body_set_centroid(ai_collider, centroid);
}

body_t *faf_make_ai_car_collider(body_t *ai_car) {
    assert(ai_car);

    body_t *body = shape_init_ai_collider(ai_car);
    body_register_tick_func(body, (body_func_t)faf_ai_collider_tick);
    return body;
}

void faf_indicator_tick(body_t *indicator, void *dt) {
    assert(indicator);

    body_t *player_car = body_get_info(indicator);
    assert(player_car);
    faf_car_info_t *info = body_get_info(player_car);
    assert(info);

    vector_t centroid = {
        .x = body_get_centroid(player_car).x,
        .y = body_get_centroid(player_car).y - body_get_dimensions(player_car).y + 50
    };
    body_set_centroid(indicator, centroid);

    if (info->strength_enabled) {
        rgb_color_t c = {.r = 1, .g = 0, .b = 0};
        body_set_color(indicator, c);
    }
    else {
        rgb_color_t c = {.r = 0, .g = 0, .b = 0.8};
        body_set_color(indicator, c);
    }
}

body_t *faf_make_player_indicator(body_t *player_car) {
    assert(player_car);

    body_t *body = shape_init_player_indicator(player_car);
    
    body_register_tick_func(body, (body_func_t)faf_indicator_tick);
    return body;
}

void faf_car_add_effect(body_t *car, body_func_t f, double total_time) {
    assert(car);

    car_effect_t *effect = malloc(sizeof(car_effect_t));
    effect->f = f;
    effect->total_time = total_time;
    effect->time_elapsed = 0.;

    faf_car_info_t *info = body_get_info(car);
    list_add(info->effects, effect);
}

void faf_car_tick_AI(body_t *ai_car, void *dt) {
    assert(ai_car);
    faf_car_info_t *info = (faf_car_info_t *)body_get_info(ai_car);
    assert(info);
    assert(!info->is_player_car);

    double target_speed = info->top_speed;
    if (faf_get_difficulty() == 1) {
        target_speed *= 0.90;
    }
    else if (faf_get_difficulty() == 2) {
        target_speed *= 1.10;
    }
    else if (faf_get_difficulty() == 3) {
        target_speed *= 1.25;
    }
    
    double curr_speed = body_get_velocity(ai_car).y;
    if (curr_speed < target_speed) {
        info->accelerating = true;
        info->braking = false;
    }
    else {
        info->accelerating = false;
        info->braking = true;
    }

    // Make sure the AI car doesn't go off the road
    double x_pos = body_get_centroid(ai_car).x;
    if (x_pos > (faf_get_scene_dimensions().x - faf_get_road_width()) / 3.5 + faf_get_road_width()) {
        faf_car_add_effect(ai_car, (body_func_t)car_turn_left, 1.2);
    }
    if (x_pos < (faf_get_scene_dimensions().x - faf_get_road_width()) / 3.5) {
        faf_car_add_effect(ai_car, (body_func_t)car_turn_right, 1.2);
    }
}

void faf_car_tick(body_t *car, void *dt) {
    assert(car);
    faf_car_info_t *info = (faf_car_info_t *)body_get_info(car);
    assert(info);

    info->time += *((double *)dt);
    if (info->time < 0) {
        return;
    }

    vector_t v = body_get_velocity(car);

    // Make sure the car stays within the race
    double x_pos = body_get_centroid(car).x;
    x_pos = mathlib_min(mathlib_max(x_pos, info->dimensions.x / 2),
                        faf_get_scene_dimensions().x - info->dimensions.x / 2);
    body_set_centroid(car, (vector_t){.x = x_pos, .y = body_get_centroid(car).y});

    if (!info->is_player_car) {
        faf_car_tick_AI(car, dt);
    }

    body_set_rotation(car, 0);
    if (info->is_player_car) {
        faf_audio_set_volume(CAR_SOUND_CHANNEL, 25);
    }

    if (info->accelerating) {
        vector_t force = {.x = 0, .y = info->accel_dS * body_get_mass(car)};
        body_add_force(car, force);
        // Increase car engine volume when accelerating
        if (info->is_player_car) {
            body_set_surface(car, info->accelerated);
            faf_audio_set_volume(CAR_SOUND_CHANNEL, 50);
            // Accelerating drains the gas twice as quickly
            info->gas_curr -= info->gas_milage * *((double *)dt);
        }
    }
    else {
        body_set_surface(car, info->normal);
    }

    if (info->braking) {
        vector_t force = {.x = 0, .y = info->brake_dS * body_get_mass(car)};
        body_add_force(car, force);
    }

    if (info->control_enabled) {
        v.x = 0;
        if (info->is_player_car) {
            if (info->turning_right) {
                body_set_rotation(car, -M_PI / 12);
                // How much turning should depend on vertical velocity
                v.x += info->turn_sensitivity * fabs(v.y);
            }

            if (info->turning_left) {
                body_set_rotation(car, M_PI / 12);
                // How much turning should depend on vertical velocity
                v.x -= info->turn_sensitivity * fabs(v.y);
            }
        }
    }

    // This avoids the problem with velocity oscillating between positive and negative values
    if (vec_magnitude(v) > 1) {
        // Magnitude of frictional force = mu * mass * grav_const
        double friction_magnitude = info->surf_coef * body_get_mass(car) * FAF_CAR_GRAV_CONST;
        vector_t friction_direction = vec_negate(vec_unit(v));
        vector_t friction = vec_multiply(friction_magnitude, friction_direction);
        friction.x *= 20;  // Want to damp out horizontal velocity quickly
        body_add_force(car, friction);
    }

    if (info->surf_coef > 0.05) {
        info->top_speed /= 2;
    }

    double top_speed = info->top_speed;
    v.x = mathlib_min(mathlib_max(v.x, -top_speed), top_speed);
    v.y = mathlib_min(mathlib_max(v.y, 0), top_speed);
    body_set_velocity(car, v);

    // Reset car to default settings
    if (info->is_player_car || faf_get_difficulty() == 1) {
        info->top_speed = info->default_top_speed;
    }
    else if (faf_get_difficulty() == 2) {
        info->top_speed = info->default_top_speed * 1.1;
    }
    else if (faf_get_difficulty() == 3) {
        info->top_speed = info->default_top_speed * 1.25;
    }
    info->strength_enabled = false;
    info->control_enabled = true;
    info->gas_curr -= info->gas_milage * *((double *)dt);
    info->gas_milage = info->default_gas_milage;
    if (!info->is_player_car) {
        info->turning_left = false;
        info->turning_right = false;
    }
}

void faf_car_register_tick(body_t *car, void *dt) {
    assert(car);
    faf_car_info_t *info = (faf_car_info_t *)body_get_info(car);
    assert(info);

    faf_car_tick(car, dt);

    size_t e_idx = 0;
    while (e_idx < list_size(info->effects)) {
        car_effect_t *effect = list_get(info->effects, e_idx);
        effect->time_elapsed += *((double *)dt);
        if (effect->time_elapsed < effect->total_time) {
            effect->f(car, dt);
            e_idx++;
        }
        else {
            list_remove(info->effects, e_idx);
        }
    }
}

body_t *faf_make_car(faf_car_t type, bool is_player_car, double start_time) {
    faf_car_info_t *info = make_car_info(type, is_player_car, start_time);
    assert(info);
    body_t *body = shape_init_rectangle_with_sprite(info->dimensions.x, info->dimensions.y - 25, FAF_CAR_DEBUG_COLOR,
                                                    FAF_CAR_DENSITY, info, (free_func_t)free_car_info,
                                                    info->filename, info->dimensions);
    body_register_tick_func(body, (body_func_t)faf_car_register_tick);
    return body;
}

double faf_car_get_curr_gas(body_t *car) {
    assert(car);
    faf_car_info_t *info = (faf_car_info_t *)body_get_info(car);
    assert(info);
    assert(info->is_player_car);

    return info->gas_curr;
}

double faf_car_get_max_gas(body_t *car) {
    assert(car);
    faf_car_info_t *info = (faf_car_info_t *)body_get_info(car);
    assert(info);
    assert(info->is_player_car);

    return info->gas_max;
}

void faf_ai_car_avoid(body_t *ai_car, body_t *other) {
    double random_chance = mathlib_rand_in_range(0, 1);
    double hit_rate = 1.0;
    if (faf_get_difficulty() == 1) {
        hit_rate = 0.95;
    }
    if (random_chance <= hit_rate) {
        if (body_get_centroid(ai_car).x < body_get_centroid(other).x) {
            faf_car_add_effect(ai_car, (body_func_t)car_turn_left, 0.65);
        }
        else {
            faf_car_add_effect(ai_car, (body_func_t)car_turn_right, 0.65);
        }
    }
}

void faf_ai_car_seek(body_t *ai_car, body_t *other) {
    double random_chance = mathlib_rand_in_range(0, 1);
    double hit_rate = 1.0;
    if (faf_get_difficulty() == 1) {
        hit_rate = 0.85;
    }
    if (random_chance <= hit_rate) {
        if (body_get_centroid(ai_car).x > body_get_centroid(other).x) {
            faf_car_add_effect(ai_car, (body_func_t)car_turn_left, 0.65);
        }
        else {
            faf_car_add_effect(ai_car, (body_func_t)car_turn_right, 0.65);
        }
    }
}

void faf_ai_collider_on_hit(body_t *ai_collider, body_t *other, vector_t axis, void *aux) {
    assert(ai_collider);
    faf_car_info_t *car_info = body_get_info(body_get_info(ai_collider));
    assert(car_info->obj_type == FAF_CAR_OBJ);
    assert(other);
    faf_object_t *other_info = body_get_info(other);
    assert(other_info);
    assert(car_info);
    assert(!car_info->is_player_car);
    body_t *ai_car = body_get_info(ai_collider);

    switch (*other_info) {
        case FAF_CAR_OBJ:
        case FAF_OBSTACLE_OBJ: {
            faf_ai_car_avoid(ai_car, other);
            break;
        }
        case FAF_GAS_OBJ: {
            if (faf_get_difficulty() == 3) {
                faf_ai_car_seek(ai_car, other);
            }
            break;
        }
        case FAF_EFFECT_OBJ: {
            faf_object_info_t *object_info = (faf_object_info_t *)other_info;
            faf_effect_t effect = faf_objects_get_effect_type(object_info);
            switch (effect) {
                case FAF_SPEED:
                case FAF_STRENGTH:
                case FAF_GREEN_ENERGY: {
                    if (faf_get_difficulty() != 1) {
                        faf_ai_car_seek(ai_car, other);
                    }
                    break;
                }
                case FAF_SLOWDOWN:
                case FAF_GASLEAK:
                case FAF_LOSE_CONTROL: {
                    faf_ai_car_avoid(ai_car, other);
                    break;
                }
                default: {
                    break;
                }
            }
        }
        default: {
            return;
        }
    }
}

void faf_car_on_hit(body_t *car, body_t *other, vector_t axis, void *aux) {
    assert(car);
    faf_car_info_t *car_info = body_get_info(car);
    assert(car_info->obj_type == FAF_CAR_OBJ);
    assert(other);
    faf_object_t *other_info = body_get_info(other);
    assert(other_info);

    switch (*other_info) {
        case FAF_SURFACE_OBJ: {
            surface_info_t *surface_info = (surface_info_t *)other_info;
            car_info->surf_coef = surface_info->surf_coefficient;
            break;
        }
        case FAF_CAR_OBJ: {
            vector_t impulse = body_calculate_impulse(car, other, axis, *(double *)aux);
            if (car != other && car_info->is_player_car) {
                faf_audio_honk();
            }
            if (car_info->strength_enabled) {
                body_add_impulse(other, vec_multiply(-5, impulse));
                faf_car_add_effect(other, (body_func_t)car_lose_control, 0.25);
            }
            else {
                body_add_impulse(car, impulse);
                body_add_impulse(other, vec_negate(impulse));
                faf_car_add_effect(car, (body_func_t)car_lose_control, 0.25);
                faf_car_add_effect(other, (body_func_t)car_lose_control, 0.25);
            }
            break;
        }
        case FAF_OBSTACLE_OBJ: {
            if (!car_info->strength_enabled) {
                body_set_velocity(car, VEC_ZERO);
            }
            body_remove(other);
            break;
        }
        case FAF_EFFECT_OBJ: {
            faf_object_info_t *object_info = (faf_object_info_t *)other_info;
            faf_effect_t effect = faf_objects_get_effect_type(object_info);
            switch (effect) {
                case FAF_SPEED: {
                    faf_car_add_effect(car, (body_func_t)car_speed, FAF_CAR_EFFECT_TIME);
                    break;
                }
                case FAF_STRENGTH: {
                    faf_car_add_effect(car, (body_func_t)car_strength, FAF_CAR_EFFECT_TIME);
                    break;
                }
                case FAF_GREEN_ENERGY: {
                    faf_car_add_effect(car, (body_func_t)car_green_energy, FAF_CAR_EFFECT_TIME);
                    break;
                }
                case FAF_SLOWDOWN: {
                    faf_car_add_effect(car, (body_func_t)car_slowdown, FAF_CAR_EFFECT_TIME);
                    break;
                }
                case FAF_GASLEAK: {
                    faf_car_add_effect(car, (body_func_t)car_gasleak, FAF_CAR_EFFECT_TIME);
                    break;
                }
                case FAF_LOSE_CONTROL: {
                    faf_car_add_effect(car, (body_func_t)car_lose_control, FAF_CAR_EFFECT_TIME);
                    break;
                }
                case FAF_NULL: {
                    break;
                }
            }
            body_remove(other);
            break;
        }
        case FAF_GAS_OBJ: {
            body_remove(other);
            double gas_boost = car_info->gas_max / 3.;
            car_info->gas_curr = mathlib_min(car_info->gas_curr + gas_boost, car_info->gas_max);
            break;
        }
        case FAF_DECORATION_OBJ: {
            body_remove(other);
            break;
        }
        case FAF_OTHER_OBJ: {
            break;
        }
    }
}

void faf_car_set_window(body_t *car, window_t *window) {
    assert(car);
    faf_car_info_t *info = body_get_info(car);
    assert(info);

    info->window = window;
}

window_t *faf_car_get_window(body_t *car) {
    assert(car);
    faf_car_info_t *info = body_get_info(car);
    assert(info);

    return info->window;
}

double faf_car_get_time(body_t *car) {
    assert(car);
    faf_car_info_t *info = body_get_info(car);
    assert(info);

    return info->time;
}