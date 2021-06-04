#include "window.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const size_t WINDOW_INIT_KEY_HANDLERS = 1;

typedef struct window {
    scene_t *scene;
    vector_t center;
    vector_t dims;
    vector_t velocity;
    body_t *focused_body;
    vector_t focus_offset;
    list_t *key_handlers;
    hud_t *hud;
    bool clear_scene;
} window_t;

typedef struct key_handler_info {
    key_handler_t f;
    void *aux;
    free_func_t aux_freer;
} key_handler_info_t;

void free_key_handler_info(key_handler_info_t *info) {
    assert(info);

    if (info->aux_freer && info->aux) {
        info->aux_freer(info->aux);
    }

    free(info);
}

window_t *window_init(scene_t *scene, vector_t center, vector_t dims) {
    assert(scene);
    assert(dims.x > 0);
    assert(dims.y > 0);

    window_t *window = malloc(sizeof(window_t));
    assert(window);
    window->scene = scene;
    window->center = center;
    window->dims = dims;
    window->velocity = VEC_ZERO;
    window->focused_body = NULL;
    window->key_handlers = list_init(WINDOW_INIT_KEY_HANDLERS,
                                     (free_func_t)free_key_handler_info);
    window->hud = NULL;
    window->clear_scene = false;

    return window;
}

void window_free(window_t *window) {
    assert(window);
    scene_free(window->scene);
    list_free(window->key_handlers);
    if (window->hud) {
        hud_free(window->hud);
    }
    free(window);
}

scene_t *window_get_scene(window_t *window) {
    assert(window);

    return window->scene;
}

void window_set_scene(window_t *window, scene_t *new_scene, vector_t new_center) {
    assert(window);
    assert(new_scene);

    scene_free(window->scene);
    window->scene = new_scene;
    window->center = new_center;
    window->velocity = VEC_ZERO;
    window->focused_body = NULL;
}

vector_t window_get_center(window_t *window) {
    assert(window);

    return window->center;
}

void window_set_center(window_t *window, vector_t new_center) {
    assert(window);

    window->center = new_center;
}

vector_t window_get_dims(window_t *window) {
    assert(window);

    return window->dims;
}

vector_t window_get_velocity(window_t *window) {
    assert(window);

    return window->velocity;
}

void window_set_velocity(window_t *window, vector_t new_velocity) {
    assert(window);

    window->velocity = new_velocity;
}

void window_follow_body(window_t *window, body_t *body, vector_t focus_offset) {
    assert(window);
    assert(body);

    window->focused_body = body;
    window->focus_offset = focus_offset;
}

void window_tick(window_t *window, double dt) {
    assert(window);
    
    scene_t *scene = window->scene;
    scene_tick(window->scene, dt);

    if(window->hud){
        hud_tick(window->hud);
    }

    if (window->focused_body) {
        window->center = vec_add(body_get_centroid(window->focused_body), window->focus_offset);
    }
    else {
        window->center = vec_add(window->center, vec_multiply(dt, window->velocity));
    }

    vector_t scene_dims = scene_get_dimensions(scene);
    if (window->center.x + window->dims.x / 2. > scene_dims.x) {
        window->center.x = scene_dims.x - window->dims.x / 2.;
    }
    if (window->center.x - window->dims.x / 2. < 0) {
        window->center.x = window->dims.x / 2.;
    }
    if (window->center.y + window->dims.y / 2. > scene_dims.y) {
        window->center.y = scene_dims.y - window->dims.y / 2.;
    }
    if (window->center.y - window->dims.y / 2. < 0) {
        window->center.y = window->dims.y / 2.;
    }

    if (window->clear_scene) {
        window->clear_scene = false;
        scene_t *clear = scene_init(window->dims);
        window_set_scene(window, clear, VEC_ZERO);
    }
}

void window_on_key(window_t *window, char key, key_event_type_t type, double held_time) {
    assert(window);

    for (size_t i = 0; i < list_size(window->key_handlers); i++) {
        key_handler_info_t *info = (key_handler_info_t *)list_get(window->key_handlers, i);
        info->f(key, type, held_time, info->aux);
    }
}

void window_add_key_handler(window_t *window, key_handler_t f, void *aux, free_func_t aux_freer) {
    assert(window);

    key_handler_info_t *info = malloc(sizeof(key_handler_info_t));
    assert(info);
    info->f = f;
    info->aux = aux;
    info->aux_freer = aux_freer;
    list_add(window->key_handlers, info);
}

void window_clear_key_handlers(window_t *window) {
    assert(window);

    if (list_size(window->key_handlers) > 0) {
        list_free(window->key_handlers);
        window->key_handlers = list_init(WINDOW_INIT_KEY_HANDLERS, 
                                         (free_func_t)free_key_handler_info);
    }
}

void window_clear_key_handlers_no_free(window_t *window) {
    assert(window);

    window->key_handlers = list_init(WINDOW_INIT_KEY_HANDLERS, 
                                     (free_func_t)free_key_handler_info);
}

vector_t scene_to_window_space(window_t *window, vector_t v) {
    assert(window);

    vector_t window_botl = vec_subtract(window->center, vec_multiply(1. / 2., window->dims));

    return vec_subtract(v, window_botl);
}

void window_set_hud(window_t *window, hud_t *hud) {
    assert(window);

    if (window->hud) {
        hud_free(window->hud);
    }
    window->hud = hud;
}

void window_set_hud_no_free(window_t *window, hud_t *hud) {
    assert(window);

    window->hud = hud;
}

list_t *window_get_key_handlers(window_t *window) {
    assert(window);

    return window->key_handlers;
}

void window_set_key_handlers(window_t *window, list_t *handlers) {
    assert(window);
    assert(handlers);

    list_free(window->key_handlers);
    window->key_handlers = handlers;
}

hud_t *window_get_hud(window_t *window) {
    assert(window);

    return window->hud;
}

void window_clear_scene(window_t *window) {
    assert(window);

    window->clear_scene = true;
}