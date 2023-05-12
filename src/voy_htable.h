#ifndef voy_htable_h
#define voy_htable_h

#include "voy_array.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef int (*voy_htable_cmp)(void *a, void *b);
typedef void (*voy_htable_cb)(void *key, void *value);

typedef struct voy_htable {
    voy_array_t *buckets;
    voy_htable_cmp cmp;
} voy_htable_t;

typedef struct voy_htable_node {
    void *key;
    void *value;
    size_t hash;
} voy_htable_node_t;

voy_htable_t *voy_htable_new(voy_htable_cmp cmp);
void voy_htable_free(voy_htable_t *htable, voy_htable_cb cb);
void *voy_htable_get(voy_htable_t *htable, void *key);
void voy_htable_set(voy_htable_t *htable, void *key, void *value);
void *voy_htable_remove(voy_htable_t *htable, void *key, voy_htable_cb cb);
void voy_htable_traverse(voy_htable_t *htable, voy_htable_cb cb);

// Macro Usage:
// VOY_HTABLE_FOREACH(htable) {
// use elem
// elem is the current voy_htable_node_t
// } VOY_HTABLE_FOREACH_END
#define VOY_HTABLE_FOREACH(htable)                                                                 \
    if ((htable)->buckets) {                                                                       \
        for (unsigned int i = 0; i < (htable)->buckets->len; i++) {                                \
            voy_array_t *bucket = voy_array_get((htable)->buckets, i);                             \
            if (bucket) {                                                                          \
                for (unsigned int j = 0; j < bucket->len; j++) {                                   \
                    voy_htable_node_t *elem = voy_array_get(bucket, j);                            \
                    if (elem)

#define VOY_HTABLE_FOREACH_END                                                                     \
    }                                                                                              \
    }                                                                                              \
    }                                                                                              \
    }

#endif
