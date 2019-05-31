
#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Vector_init(Vector *vector, int elem_size) {
    vector->elem_size = elem_size;
    vector->size = 0;
    vector->capacity = VECTOR_INITIAL_CAPACITY;
    vector->data = malloc(elem_size * vector->capacity);
}

void Vector_delete(Vector *vector, int index) {
    char *data = (char*)vector->data;
    memmove((void*)data[index * vector->elem_size],
        (void*)data[(index + 1) * vector->elem_size],
        (vector->size - index - 1) * vector->elem_size);

    vector->size = vector->size - 1;
}

void Vector_set_size(Vector *vector, int size)
{
    int oldSize = vector->size;
    vector->size = size;
    Vector_resize(vector);
    // set all new elements to zero
    if (size > oldSize) {
        char *data = vector->data;
        memset(&data[oldSize * vector->elem_size], 0, size - oldSize);
    }
}

void Vector_resize(Vector *vector) {
    if (vector->size >= vector->capacity) {
        vector->capacity *= 2;
        vector->data = realloc(vector->data, vector->elem_size * vector->capacity);
    }
}

void Vector_clear(Vector *vector) {
    vector->size = 0;
}

void Vector_swap(Vector *first, Vector *second)
{
    Vector tmp = *first;
    *first = *second;
    *second = tmp;
}

int Vector_size(Vector *vector) {
    return vector->size;
}

int Vector_capacity(Vector *vector) {
    return vector->capacity;
}

bool Vector_is_empty(Vector *vector) {
    return vector->size == 0;
}

void Vector_free(Vector *vector) {
    free(vector->data);
}
