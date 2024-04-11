#ifndef _voy_array_h
#define _voy_array_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct voy_array {
    int len;
    int capacity;
    int expand_rate;
    size_t item_size;
    void **contents;
} voy_array_t;

typedef void (*voy_array_free_cb)(void *value);

voy_array_t *voy_array_new(int capacity, size_t item_size);
void voy_array_free(voy_array_t *arr, voy_array_free_cb cb);
void voy_array_clear(voy_array_t *arr, voy_array_free_cb cb);
void voy_array_push(voy_array_t *arr, void *value);
void *voy_array_pop(voy_array_t *arr);
void voy_array_set(voy_array_t *arr, void *elem, int index);
void *voy_array_get(voy_array_t *arr, int index);
void *voy_array_remove(voy_array_t *arr, int index);
void voy_array_shift(voy_array_t *arr, void *value);
void *voy_array_unshift(voy_array_t *arr);
void voy_array_swap(voy_array_t *arr, int a, int b);

// Macro usage:
// VOY_ARRAY_FOREACH(arr, i) {
// the variable "i" will contain the current index
// }
#define VOY_ARRAY_FOREACH(arr, i) for (int(i) = 0; (i) < (arr)->len; (i)++)

#endif
