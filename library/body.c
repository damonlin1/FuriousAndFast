#include "body.h"
#include "collision.h"
#include "forces.h"
#include "polygon.h"
#include <assert.h>
#include <stdlib.h>

const size_t BODY_INIT_TICK_FUNC_COUNT = 10;
const size_t BODY_INIT_FORCES_COUNT = 10;
const size_t BODY_INIT_SURFACE_COUNT = 10;

typedef struct body {
    list_t *shape;
    vector_t velocity;
    double mass;
    rgb_color_t color;
    vector_t centroid;
    double curr_rotation;
    list_t *tick_funcs;
    vector_t pending_force;
    vector_t pending_impulse;
    void *info;
    free_func_t info_freer;
    bool removed;
    double bounding_radius;
    SDL_Surface *surface;
    list_t *surface_list;
    vector_t dimensions;
    bool debug_mode;
} body_t;

void body_do_nothing(void *arg) {
    return;
}

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
    return body_init_with_info(shape, mass, color, NULL, NULL);
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer) {
    return body_init_with_info_and_sprite(shape, mass, color, info, info_freer, NULL, VEC_ZERO);
}

body_t *body_init_with_info_and_sprite(list_t *shape, double mass, rgb_color_t color, void *info,
                                       free_func_t info_freer, const char *filename, vector_t dimensions) {
    body_t *new_body = malloc(sizeof(body_t));
    assert(new_body);
    assert(mass > 0);

    new_body->shape = shape;
    new_body->velocity = VEC_ZERO;
    new_body->mass = mass;
    new_body->color = color;
    new_body->centroid = polygon_centroid(shape);
    new_body->curr_rotation = 0;
    new_body->tick_funcs = list_init(BODY_INIT_TICK_FUNC_COUNT, 
                                     (free_func_t)body_do_nothing);

    new_body->pending_force = VEC_ZERO;
    new_body->pending_impulse = VEC_ZERO;

    double bounding_radius = 0;
    for (size_t i = 0; i < list_size(shape); i++) {
        double d = vec_distance(new_body->centroid, *((vector_t *)list_get(shape, i)));
        if (d > bounding_radius) {
            bounding_radius = d;
        }
    }
    new_body->bounding_radius = bounding_radius;

    new_body->info = info;
    new_body->info_freer = info_freer;
    new_body->removed = false;
    new_body->debug_mode = false;

    if (filename) {
        new_body->surface = IMG_Load(filename);
        new_body->dimensions = dimensions;
    }
    else {
        new_body->surface = NULL;
    }

    new_body->surface_list = list_init(BODY_INIT_SURFACE_COUNT, (free_func_t)SDL_FreeSurface);

    return new_body;
}

void body_free(body_t *body) {
    assert(body);

    list_free(body->shape);
    list_free(body->tick_funcs);

    if (body->info_freer && body->info) {
        body->info_freer(body->info);
    }

    list_free(body->surface_list);

    free(body);
}

list_t *body_get_shape(body_t *body) {
    assert(body);

    list_t *shape_cpy = list_init(list_size(body->shape), (free_func_t)free);
    for (size_t i = 0; i < list_size(body->shape); i++) {
        vector_t *v = malloc(sizeof(vector_t));
        *v = *(vector_t *)(list_get(body->shape, i));
        list_add(shape_cpy, v);
    }

    return shape_cpy;
}

list_t *body_get_shape_nocpy(body_t *body) {
    assert(body);

    return body->shape;
}

vector_t body_get_centroid(body_t *body) {
    assert(body);

    return body->centroid;
}

vector_t body_get_velocity(body_t *body) {
    assert(body);

    return body->velocity;
}

double body_get_mass(body_t *body) {
    assert(body);

    return body->mass;
}

rgb_color_t body_get_color(body_t *body) {
    assert(body);

    return body->color;
}

void *body_get_info(body_t *body) {
    assert(body);

    return body->info;
}

double body_get_rotation(body_t *body) {
    assert(body);

    return body->curr_rotation;
}

double body_get_bounding_radius(body_t *body) {
    assert(body);

    return body->bounding_radius;
}

SDL_Surface *body_get_surface(body_t *body) {
    assert(body);

    return body->surface;
}

vector_t body_get_dimensions(body_t *body) {
    assert(body);

    return body->dimensions;
}

bool body_get_debug_mode(body_t *body) {
    assert(body);

    return body->debug_mode;
}

void body_set_centroid(body_t *body, vector_t x) {
    assert(body);

    vector_t translation = vec_subtract(x, body->centroid);
    polygon_translate(body->shape, translation);
    body->centroid = x;
}

void body_set_velocity(body_t *body, vector_t v) {
    assert(body);

    body->velocity = v;
}

void body_set_rotation(body_t *body, double angle) {
    assert(body);

    vector_t c = body->centroid;
    double delta_angle = angle - body->curr_rotation;
    polygon_rotate(body->shape, delta_angle, c);
    body->curr_rotation = angle;
}

void body_set_color(body_t *body, rgb_color_t color) {
    assert(body);

    body->color = color;
}

void body_set_surface(body_t *body, SDL_Surface *surface) {
    assert(body);

    if (surface != body->surface) {
        body->surface = surface;
    }
    for (size_t i = 0; i < list_size(body->surface_list); i++) {
        if (list_get(body->surface_list, i) == surface) {
            return;
        }
    }
    if (surface) {
        list_add(body->surface_list, surface);
    }
}

void body_set_debug_mode(body_t *body, bool mode) {
    assert(body);

    body->debug_mode = mode;
}

void body_add_force(body_t *body, vector_t force) {
    assert(body);

    body->pending_force = vec_add(body->pending_force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
    assert(body);

    body->pending_impulse = vec_add(body->pending_impulse, impulse);
}

vector_t body_calculate_impulse(body_t *body1, body_t *body2, vector_t axis, double elasticity) {
    assert(body1);
    assert(body2);
    assert(0 <= elasticity && elasticity <= 1);
    double mass1 = body_get_mass(body1);
    double mass2 = body_get_mass(body2);
    double velo1 = vec_dot(axis, body_get_velocity(body1));
    double velo2 = vec_dot(axis, body_get_velocity(body2));
    double impulse;
    if (mass1 == INFINITY && mass2 == INFINITY) {
        impulse = 0.;
    }
    else if (mass2 == INFINITY) {
        impulse = mass1 * (1 + elasticity) * (velo2 - velo1);
    }
    else if (mass1 == INFINITY) {
        impulse = mass2 * (1 + elasticity) * (velo2 - velo1);
    }
    else {
        impulse = ((mass1 * mass2) / (mass1 + mass2)) * (1 + elasticity) * (velo2 - velo1);
    }
    return vec_multiply(impulse, axis);
}

void body_tick(body_t *body, double dt) {
    assert(body);

    vector_t old_v = body->velocity;

    // Add acceleration from forces
    vector_t accel = vec_multiply(1. / body->mass, body->pending_force);
    vector_t new_v = vec_add(old_v, vec_multiply(dt, accel));
    // Add acceleration from impules
    vector_t dv = vec_multiply(1. / body->mass, body->pending_impulse);
    new_v = vec_add(new_v, dv);
    body->pending_force = VEC_ZERO;
    body->pending_impulse = VEC_ZERO;

    body_set_velocity(body, new_v);

    double *d = malloc(sizeof(double));
    assert(d);
    *d = dt;
    for (size_t i = 0; i < list_size(body->tick_funcs); i++) {
        body_func_t f = list_get(body->tick_funcs, i);
        f(body, d);
    }
    free(d);

    // Take average velocity for movement
    vector_t avg_v = vec_multiply(1. / 2., vec_add(old_v, new_v));
    vector_t movement = vec_multiply(dt, avg_v);
    polygon_translate(body->shape, movement);
    body->centroid = vec_add(body->centroid, movement);
}

bool body_is_on_screen(body_t *body, vector_t lower_bounds, vector_t upper_bounds) {
    assert(body);

    for (size_t i = 0; i < list_size(body->shape); i++) {
        vector_t *v = list_get(body->shape, i);

        if ((v->x >= lower_bounds.x && v->x <= upper_bounds.x)
            && (v->y >= lower_bounds.y && v->y <= upper_bounds.y)) {
            return true;
        }
    }

    return false;
}

void body_register_tick_func(body_t *body, body_func_t f) {
    assert(body);
    assert(f);

    list_add(body->tick_funcs, f);
}

void body_unregister_tick_func(body_t *body, body_func_t f) {
    assert(body);

    for (size_t i = 0; i < list_size(body->tick_funcs); i++) {
        if (f == list_get(body->tick_funcs, i)) {
            list_remove(body->tick_funcs, i);
            return;
        }
    }
}

bool body_are_overlapping(body_t *body1, body_t *body2) {
    assert(body1);
    assert(body2);

    collision_info_t *c_info = find_collision(body_get_shape_nocpy(body1),
                                              body_get_shape_nocpy(body2));
    if (c_info) {
        free(c_info);
        return true;
    }
    return false;
}

void body_remove(body_t *body) {
    assert(body);
    body->removed = true;
}

bool body_is_removed(body_t *body) {
    assert(body);

    return body->removed;
}