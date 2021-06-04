#include "list.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const int SIZE_SCALE = 2;

typedef struct list {
    void **data;
    size_t max_size;
    size_t num_elems;
    free_func_t deallocator;
} list_t;

list_t *list_init(size_t initial_size, free_func_t freer) {
    list_t *new_list = malloc(sizeof(list_t));
    assert(new_list);

    void **data = malloc(sizeof(void *) * initial_size);
    assert(data);

    new_list->data = data;
    new_list->max_size = initial_size;
    new_list->num_elems = 0;
    new_list->deallocator = freer;

    return new_list;
}

void list_free(list_t *list) {
    assert(list);

    for (size_t i = 0; i < list->num_elems; i++) {
        if (list->deallocator) {
            list->deallocator(list_get(list, i));
        }
    }

    free(list->data);
    free(list);
}

size_t list_size(list_t *list) {
    assert(list);

    return list->num_elems;
}

void *list_get(list_t *list, size_t index) {
    assert(list);
    assert(index < list->num_elems);

    return list->data[index];
}

void *list_remove(list_t *list, size_t index) {
    assert(list);
    assert(index < list->num_elems);

    void *elem = list_get(list, index);
    list->num_elems--;

    for (size_t i = index; i < list->num_elems; i++) {
        list->data[i] = list->data[i + 1];
    }

    return elem;
}

void list_ensure_capacity(list_t *list) {
    if (list->num_elems == list->max_size) {
        if (list->max_size == 0) {
            list->max_size = 1;
        }
        size_t new_size = list->max_size * SIZE_SCALE;
        list->max_size = new_size;
        list->data = (void **)realloc(list->data, sizeof(void *) * new_size);
        assert(list->data);
    }
}

void list_add(list_t *list, void *value) {
    assert(value);
    assert(list);

    list_ensure_capacity(list);

    list->data[list->num_elems] = value;
    list->num_elems++;
}