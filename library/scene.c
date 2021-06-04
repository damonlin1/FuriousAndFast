#include "scene.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const size_t SCENE_INIT_MAX_BODIES = 10;
const size_t SCENE_INIT_FORCE_FUNC_COUNT = 10;
const size_t SCENE_INIT_NUM_LAYERS = 2;
const size_t SCENE_DEFAULT_LAYER = 1;

typedef struct scene {
    list_t *layers;
    size_t num_layers;
    list_t *force_funcs;
    vector_t dimensions;
    bool paused;
} scene_t;

typedef struct force_struct {
    force_creator_t forcer;
    void *aux;
    free_func_t freer;
    list_t *bodies;
} force_struct_t;

void scene_add_layer(scene_t *scene) {
    assert(scene);

    list_t *new_layer = list_init(SCENE_INIT_MAX_BODIES, (free_func_t) body_free);
    list_add(scene->layers, new_layer);
    scene->num_layers++;
}

void scene_add_n_layers(scene_t *scene, size_t n) {
    assert(scene);

    for (size_t i = 0; i < n; i++) {
        scene_add_layer(scene);
    }
}

void scene_free_force_func(force_struct_t *f) {
    assert(f);

    if (f->freer) {
        f->freer(f->aux);
    }

    list_free(f->bodies);

    free(f);
}

scene_t *scene_init(vector_t dimensions) {
    assert(dimensions.x > 0);
    assert(dimensions.y > 0);

    scene_t *new_scene = malloc(sizeof(scene_t));
    assert(new_scene);

    list_t *layers = list_init(SCENE_INIT_NUM_LAYERS, (free_func_t) list_free);
    list_t *force_funcs = list_init(SCENE_INIT_FORCE_FUNC_COUNT, 
                                   (free_func_t) scene_free_force_func);

    new_scene->layers = layers;
    new_scene->num_layers = 0;
    new_scene->force_funcs = force_funcs;
    new_scene->dimensions = dimensions;
    new_scene->paused = false;

    scene_add_n_layers(new_scene, SCENE_INIT_NUM_LAYERS);

    return new_scene;
}

void scene_free(scene_t *scene) {
    assert(scene);

    list_free(scene->layers);
    list_free(scene->force_funcs);
    free(scene);
}

size_t scene_num_layers(scene_t *scene) {
    assert(scene);

    return(scene->num_layers);
}

list_t *scene_get_layer(scene_t *scene, size_t idx) {
    assert(scene);
    assert(idx < scene->num_layers);

    return (list_t *)list_get(scene->layers, idx);
}

size_t scene_num_bodies(scene_t *scene) {
    assert(scene);
    size_t num_bodies = 0;
    for (size_t i = 0; i < scene->num_layers; i++) {
        num_bodies += list_size(scene_get_layer(scene, i));
    }
    return num_bodies;
}

void scene_add_body(scene_t *scene, body_t *body) {
    assert(scene);
    assert(body);

    list_t *default_layer = scene_get_layer(scene, SCENE_DEFAULT_LAYER);
    list_add(default_layer, body);
}

void scene_add_body_in_layer(scene_t *scene, body_t *body, size_t layer_no) {
    assert(scene);
    assert(body);
    
    while (layer_no >= scene->num_layers) {
        scene_add_layer(scene);
    }
    
    list_t *layer = scene_get_layer(scene, layer_no);
    list_add(layer, body);
}

vector_t scene_get_dimensions(scene_t *scene) {
    assert(scene);

    return scene->dimensions;
}

void scene_set_dimensions(scene_t *scene, vector_t dimensions) {
    assert(scene);
    assert(dimensions.x > 0);
    assert(dimensions.y > 0);

    scene->dimensions = dimensions;
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies, free_func_t freer) {
    assert(scene);
    assert(forcer);
    assert(aux);
    assert(bodies);

    force_struct_t *f = malloc(sizeof(force_struct_t));
    assert(f);
    f->forcer = forcer;
    f->aux = aux;
    f->bodies = bodies;
    f->freer = freer;
    list_add(scene->force_funcs, f);
}

void scene_delete_bodies_and_forces(scene_t *scene) {
    assert(scene);

    for (size_t i = 0; i < scene->num_layers; i++) {
        list_t *layer = scene_get_layer(scene, i);
        size_t body_idx = 0;
        while (body_idx < list_size(layer)) {
            body_t *body = (body_t *)list_get(layer, body_idx);
            if (body_is_removed(body)) {
                list_remove(layer, body_idx);

                size_t force_funcs_idx = 0;
                while (force_funcs_idx < list_size(scene->force_funcs)) {
                    force_struct_t *force = list_get(scene->force_funcs, force_funcs_idx);

                    bool removed = false;
                    for (size_t i = 0; i < list_size(force->bodies); i++) {
                        body_t *b = list_get(force->bodies, i);
                        if (body == b) {
                            list_remove(scene->force_funcs, force_funcs_idx);
                            scene_free_force_func(force);
                            removed = true;
                            break;
                        }
                    }
                    if (!removed) {
                        force_funcs_idx++;
                    }
                }

                body_free(body);
            }
            else {
                body_idx++;
            }
        }

    }
}

void scene_for_each(scene_t *scene, body_func_t f, void *args) {
    assert(scene);
    assert(f);

    for (size_t i = 0; i < scene->num_layers; i++) {
        list_t *layer = scene_get_layer(scene, i);
        for (size_t j = 0; j < list_size(layer); j++) {
            f((body_t *)list_get(layer, j), args);
        }
    }
}

// Helper function to use body_tick() with the scene_for_each() abstraction
void scene_helper_body_tick(body_t *body, void *dt) {
    body_tick(body, *(double *)dt);
}

void scene_tick(scene_t *scene, double dt) {
    assert(scene);
    
    if (scene->paused) {
        return;
    }
    
    for (size_t i = 0; i < list_size(scene->force_funcs); i++) {
        force_struct_t *f = list_get(scene->force_funcs, i);
        f->forcer(f->aux);
    }

    double *d = malloc(sizeof(double));
    assert(d);
    *d = dt;
    scene_for_each(scene, scene_helper_body_tick, (void *)d);
    free(d);

    scene_delete_bodies_and_forces(scene);
}

void scene_pause(scene_t *scene) {
    assert(scene);

    scene->paused = true;
}

void scene_resume(scene_t *scene) {
    assert(scene);

    scene->paused = false;
}

void scene_toggle_pause(scene_t *scene) {
    assert(scene);

    scene->paused = !scene->paused;
}