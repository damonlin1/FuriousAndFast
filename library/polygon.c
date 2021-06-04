#include "polygon.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double const POLYGON_INF_VAL = 10000;

double polygon_area(list_t *polygon) {
    double area = 0;
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *vector1 = list_get(polygon, i);
        vector_t *vector2 = list_get(polygon, (i + 1) % list_size(polygon));
        area += vec_cross(*vector1, *vector2);
    }
    return 0.5 * fabs(area);
}

vector_t polygon_centroid(list_t *polygon) {
    double c_x = 0;
    double c_y = 0;

    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *vector1 = list_get(polygon, i);
        vector_t *vector2 = list_get(polygon, (i + 1) % list_size(polygon));

        c_x += (vector1->x + vector2->x) * vec_cross(*vector1, *vector2);
        c_y += (vector1->y + vector2->y) * vec_cross(*vector1, *vector2);       
    }

    vector_t centroid = {.x = 1 / (6 * polygon_area(polygon)) * c_x, 
                         .y = 1 / (6 * polygon_area(polygon)) * c_y};
    return centroid;
}

void polygon_translate(list_t *polygon, vector_t translation) {
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *v = list_get(polygon, i);
        *v = vec_add(*v, translation);
    }
}

void polygon_rotate(list_t *polygon, double angle, vector_t point) {
    // Translate all vertices so that point is the origin
    polygon_translate(polygon, vec_negate(point));
    // Rotate about the point
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *v = list_get(polygon, i);
        *v = vec_rotate(*v, angle);
    }
    // Return to (0, 0) origin
    polygon_translate(polygon, point);
}