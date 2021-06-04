#include "body.h"
#include "collision.h"
#include "mathlib.h"
#include "polygon.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

// Computes the min and max projection of a shape onto a line
vector_t min_and_max_projection(list_t *shape, vector_t line) {
    double max = -INFINITY;
    double min = INFINITY;

    for (size_t i = 0; i < list_size(shape); i++) { 
        double dot = vec_dot(*(vector_t *)list_get(shape, i), line);
        if (dot < min) {
            min = dot;
        }
        if (dot > max) {
            max = dot;
        }
    }

    return (vector_t){.x = min, .y = max};
}

// Finds if the projections of shape1 and shape2 onto any perpendicular of shape1's edge overlap 
bool find_projection_overlap(list_t *shape1, list_t *shape2, collision_info_t *info) {
    assert(shape1);
    assert(shape2);
    assert(info);

    for (size_t i = 0; i < list_size(shape1); i++) {
        vector_t vertex1 = *(vector_t *)list_get(shape1, i);
        vector_t vertex2 = *(vector_t *)list_get(shape1, (i+1) % list_size(shape1));
        vector_t edge = vec_unit(vec_subtract(vertex1, vertex2));
        // Create a line that is perpendicular to that edge
        vector_t perp = vec_rotate(edge, M_PI / 2);

        // go through every point and dot it with the edge
        vector_t shape1_proj = min_and_max_projection(shape1, perp);
        vector_t shape2_proj = min_and_max_projection(shape2, perp);

        // Finds if the two projections overlap
        // x = min projection and y = max projection
        if (shape2_proj.x > shape1_proj.y || shape1_proj.x > shape2_proj.y) {
            return false;
        }

        double overlap1 = shape2_proj.y - shape1_proj.x;
        double overlap2 = shape1_proj.y - shape2_proj.x;

        double min = mathlib_min(overlap1, overlap2);
        if (min < info->min_overlap) {
            info->min_overlap = min;

            vector_t centroid1 = polygon_centroid(shape1);
            vector_t centroid2 = polygon_centroid(shape2);
            double og_dist = vec_distance(centroid1, centroid2);
            double new_dist = vec_distance(vec_add(centroid1, perp), centroid2);
            if (og_dist > new_dist) {
                info->axis = perp;
            }
            else {
                info->axis = vec_negate(perp);
            }
        }
    }
    return true;
}

collision_info_t *find_collision(list_t *shape1, list_t *shape2) {
    collision_info_t *info = malloc(sizeof(collision_info_t));
    info->min_overlap = INFINITY;
    if (find_projection_overlap(shape1, shape2, info) && find_projection_overlap(shape2, shape1, info)) {
        return info;
    }
    free(info);
    return NULL;
}