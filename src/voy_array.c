#include "voy_array.h"

#define EXPAND_RATE 100

void *_voy_array_remove_element_at(voy_array_t *arr, unsigned int index)
{
    if (arr->contents[index] != NULL) {
        void *element = arr->contents[index];
        arr->contents[index] = NULL;
        arr->len--;

        return element;
    }

    return NULL;
}

// creates a new empty array
voy_array_t *voy_array_new(unsigned int capacity, size_t item_size)
{
    voy_array_t *arr = malloc(sizeof(voy_array_t));

    if (!arr) {
        return NULL;
    }

    arr->len = 0;
    arr->capacity = capacity;
    arr->expand_rate = EXPAND_RATE;
    arr->item_size = item_size;
    arr->contents = calloc(arr->capacity, arr->item_size);

    if (!arr->contents) {
        return NULL;
    }

    return arr;
}

// empties and frees the voy_array_t completely
void voy_array_free(voy_array_t *arr, voy_array_free_cb cb)
{
    if (!arr) {
        return;
    }

    voy_array_clear(arr, cb);
    free(arr->contents);
    free(arr);
}

// removes all the elements on the array, leaving it empty
void voy_array_clear(voy_array_t *arr, voy_array_free_cb cb)
{
    if (!arr) {
        return;
    }

    unsigned int array_len = arr->len;

    for (unsigned int i = 0; i < array_len; i++) {
        if (arr->contents[i]) {
            if (cb) {
                cb(arr->contents[i]);
            }
            arr->contents[i] = NULL;
        }
        arr->len--;
    }
}

void voy_array_expand(voy_array_t *arr)
{
    int new_capacity = arr->capacity + EXPAND_RATE;
    void *contents = realloc(arr->contents, new_capacity * arr->item_size);

    if (!contents) {
        return;
    }

    arr->contents = contents;
    arr->capacity = new_capacity;
}

// add element to the end
void voy_array_push(voy_array_t *arr, void *value)
{
    if (!arr) {
        return;
    }

    arr->contents[arr->len] = value;
    arr->len++;

    // expand if necessary
    if (arr->len >= arr->capacity) {
        voy_array_expand(arr);
    }
}

// remove last element and return it
void *voy_array_pop(voy_array_t *arr)
{
    if (!arr) {
        return NULL;
    }

    void *elem = NULL;
    if (arr->len > 0) {
        elem = _voy_array_remove_element_at(arr, arr->len - 1);
    }

    return elem;
}

// add/set element at index
void voy_array_set(voy_array_t *arr, void *elem, unsigned int i)
{
    if (!arr) {
        return;
    }

    // index is too large
    if (i >= arr->capacity) {
        return;
    }
    if (i >= arr->len) {
        arr->len = i + 1;
    }

    arr->contents[i] = elem;
}

// get element at index
void *voy_array_get(voy_array_t *arr, unsigned int i)
{
    if (!arr) {
        return NULL;
    }

    // index is too large
    if (i >= arr->len) {
        return NULL;
    }

    return arr->contents[i];
}

// remove element at index and return it
void *voy_array_remove(voy_array_t *arr, unsigned int i)
{
    if (!arr) {
        return NULL;
    }

    // index is too large
    if (i >= arr->len) {
        return NULL;
    }

    void *element = _voy_array_remove_element_at(arr, i);

    if (element != NULL && arr->contents[i + 1] != NULL) {
        memmove(
            &arr->contents[i],
            &arr->contents[i + 1],
            sizeof(arr->item_size) * (arr->len - i)
        );
    }

    return element;
}

// add element to the beginning
void voy_array_shift(voy_array_t *arr, void *value)
{
    if (!arr) {
        return;
    }

    if (arr->len > 0) {
        memmove(
            &arr->contents[1],
            arr->contents,
            sizeof(arr->item_size) * arr->len
        );
    }

    arr->contents[0] = value;
    arr->len++;

    // expand if necessary
    if (arr->len >= arr->capacity) {
        voy_array_expand(arr);
    }
}

// remove first element and return it
void *voy_array_unshift(voy_array_t *arr)
{
    if (!arr) {
        return NULL;
    }

    void *elem = NULL;
    if (arr->len > 0) {
        elem = _voy_array_remove_element_at(arr, 0);

        memmove(
            arr->contents,
            &arr->contents[1],
            sizeof(arr->item_size) * arr->len
        );
    }

    return elem;
}

void voy_array_swap(voy_array_t *arr, unsigned int a, unsigned int b)
{
    if (!arr) {
        return;
    }
    if (a == b) {
        return;
    }
    if (a > arr->len-1 || b > arr->len-1) {
        return;
    }

    void *a_tmp = voy_array_get(arr, a);
    void *b_tmp = voy_array_get(arr, b);
    if (!a_tmp || !b_tmp) {
        return;
    }
    voy_array_set(arr, b_tmp, a);
    voy_array_set(arr, a_tmp, b);
}
