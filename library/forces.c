#include "collision.h"
#include "forces.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

const double FORCES_MIN_GRAVITY_DISTANCE = 50;

typedef struct gravity_aux {
    double G;
    body_t *body1;
    body_t *body2;
} gravity_aux_t;

typedef struct spring_aux {
    double k;
    body_t *body1;
    body_t *body2;
} spring_aux_t;

typedef struct drag_aux {
    double gamma;
    body_t *body;
} drag_aux_t;

typedef struct collision_aux {
    body_t *body1;
    body_t *body2;
    collision_handler_t handler;
    bool handled_collision;
    void *aux;
    free_func_t aux_freer;
} collision_aux_t;

void force_creator_gravity(gravity_aux_t *aux) {
    assert(aux);

    body_t *body1 = aux->body1;
    body_t *body2 = aux->body2;

    assert(body1);
    assert(body2);

    vector_t r12 = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
    vector_t r21 = vec_negate(r12);
    double r = vec_magnitude(r12);

    if (r > FORCES_MIN_GRAVITY_DISTANCE) {
        double force_mag = aux->G * body_get_mass(body1) * body_get_mass(body2) / (r * r);
        body_add_force(body1, vec_multiply(force_mag, vec_unit(r12)));
        body_add_force(body2, vec_multiply(force_mag, vec_unit(r21)));
    }
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2) {
    assert(scene);
    assert(body1);
    assert(body2);

    gravity_aux_t *aux = malloc(sizeof(gravity_aux_t));
    assert(aux);

    aux->G = G;
    aux->body1 = body1;
    aux->body2 = body2;

    list_t *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);

    scene_add_bodies_force_creator(scene, (force_creator_t)force_creator_gravity, aux, bodies, free);
}

void force_creator_spring(spring_aux_t *aux) {
    assert(aux);

    body_t *body1 = aux->body1;
    body_t *body2 = aux->body2;

    assert(body1);
    assert(body2);

    vector_t r12 = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
    vector_t r21 = vec_negate(r12);
    double r = vec_magnitude(r12);

    double force_mag = aux-> k * r;
    body_add_force(body1, vec_multiply(force_mag, vec_unit(r12)));
    body_add_force(body2, vec_multiply(force_mag, vec_unit(r21)));
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2) {
    assert(scene);
    assert(body1);
    assert(body2);

    spring_aux_t *aux = malloc(sizeof(spring_aux_t));
    assert(aux);
    aux->k = k;
    aux->body1 = body1;
    aux->body2 = body2;

    list_t *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);

    scene_add_bodies_force_creator(scene, (force_creator_t)force_creator_spring, aux, bodies, free);
}

void force_creator_drag(drag_aux_t *aux) {
    assert(aux);

    body_t *body = aux->body;

    assert(body);

    vector_t drag_vec = vec_negate(body_get_velocity(body));
    drag_vec = vec_multiply(aux->gamma, drag_vec);

    body_add_force(body, drag_vec);
}

void create_drag(scene_t *scene, double gamma, body_t *body) {
    assert(scene);
    assert(body);
    assert(gamma > 0);

    drag_aux_t *aux = malloc(sizeof(drag_aux_t));
    assert(aux);
    aux->gamma = gamma;
    aux->body = body;

    list_t *bodies = list_init(2, NULL);
    list_add(bodies, body);

    scene_add_bodies_force_creator(scene, (force_creator_t)force_creator_drag, aux, bodies, free);
}

void force_creator_collision(collision_aux_t *aux) {
    assert(aux);

    body_t *body1 = aux->body1;
    body_t *body2 = aux->body2;

    assert(body1);
    assert(body2);

    double distance = vec_distance(body_get_centroid(body1), body_get_centroid(body2));
    if (distance > body_get_bounding_radius(body1) + body_get_bounding_radius(body2)) {
        aux->handled_collision = false;
        return;
    }

    collision_info_t* c_info = find_collision(body_get_shape_nocpy(body1), body_get_shape_nocpy(body2));
    if (c_info && !aux->handled_collision) { 
        vector_t axis = c_info->axis;
        if (aux->handler) {
            aux->handler(body1, body2, axis, aux->aux);
        }
        aux->handled_collision = true;
    }
    else if (!c_info) {
        aux->handled_collision = false;
    }
}

void create_collision(scene_t *scene, body_t *body1, body_t *body2, collision_handler_t handler, void *aux, free_func_t freer) {
    assert(scene);
    assert(body1);
    assert(body2);

    collision_aux_t *collision_aux = malloc(sizeof(collision_aux_t));
    assert(collision_aux);
    collision_aux->body1 = body1;
    collision_aux->body2 = body2;
    collision_aux->handler = handler;
    collision_aux->aux = aux;
    collision_aux->aux_freer = freer;
    collision_aux->handled_collision = false;

    list_t *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);

    scene_add_bodies_force_creator(scene, (force_creator_t)force_creator_collision, collision_aux, bodies, freer);
}

void collision_handler_destructive_collision(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    body_remove(body1);
    body_remove(body2);
}

void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2) {
    assert(scene);
    assert(body1);
    assert(body2);

    create_collision(scene, body1, body2, (collision_handler_t) collision_handler_destructive_collision, NULL, NULL);
}

void collision_handler_physics_collision(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    vector_t vec_impulse = body_calculate_impulse(body1, body2, axis, *(double *)aux);
    body_add_impulse(body2, vec_negate(vec_impulse));
    body_add_impulse(body1, vec_impulse);
}

void create_physics_collision(scene_t *scene, double elasticity, body_t *body1, body_t *body2) {
    assert(scene);
    assert(body1);
    assert(body2);

    double *aux = malloc(sizeof(double));
    *aux = elasticity;

    create_collision(scene, body1, body2, (collision_handler_t) collision_handler_physics_collision, aux, free);
}