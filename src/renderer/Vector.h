#pragma once

#include <stdbool.h>

#define VECTOR_INITIAL_CAPACITY 16

typedef struct {
    int elem_size;
    int size;
    int capacity;
    void *data;
} Vector;

void Vector_init(Vector *vector, int elem_size);
void Vector_free(Vector *vector);
void Vector_delete(Vector *vector, int index);
void Vector_set_size(Vector *vector, int size);
void Vector_resize(Vector *vector);
void Vector_clear(Vector *vector);
void Vector_swap(Vector *first, Vector *second);

int Vector_size(Vector *vector);
int Vector_capacity(Vector *vector);

bool Vector_is_empty(Vector *vector);

#define Vector_element(vector,index,type) ((type*)((Vector*)vector)->data)[index]

#define Vector_set(vector,index,value,type) Vector_element(vector,index,type) = value

#define Vector_append(vector,value,type) \
    Vector_resize(vector); \
    Vector_set(vector,((Vector*)vector)->size++,value,type)

#define Vector_prepend(vector,value,type) \
    Vector_set(vector,0,value,type); \
    vector->size++;

#define Vector_pop(vector,type) \
    Vector_element(vector, --((Vector*)vector)->size, type) \

#define Vector_front(vector,type) \
    Vector_element(vector, 0, type)

#define Vector_back(vector,type) \
    Vector_element(vector, ((Vector*)vector)->size - 1, type)