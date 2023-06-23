#include "voy_htable.h"

#define NUM_OF_BUCKETS 100

static size_t voy_htable_fnv1a_hash(const char *cp)
{
    size_t hash = 0x811c9dc5;
    while (*cp) {
        hash ^= (unsigned char)*cp++;
        hash *= 0x01000193;
    }

    return hash;
}

static bool voy_htable_bucket_index_is_valid(int bucket_index)
{
    if (bucket_index >= NUM_OF_BUCKETS) {
        return false;
    }

    return true;
}

static voy_array_t *voy_htable_find_bucket(voy_htable_t *htable, void *key, size_t *bucket_hash,
                                           bool create_new)
{
    *bucket_hash     = voy_htable_fnv1a_hash(key);
    int bucket_index = *bucket_hash % NUM_OF_BUCKETS;

    if (!voy_htable_bucket_index_is_valid(bucket_index)) {
        return NULL;
    }

    voy_array_t *bucket = voy_array_get(htable->buckets, bucket_index);
    if (!bucket && create_new) {
        bucket = voy_array_new(NUM_OF_BUCKETS, sizeof(voy_htable_node_t *));
        voy_array_set(htable->buckets, bucket, bucket_index);
    }

    return bucket;
}

voy_htable_t *voy_htable_new(voy_htable_cmp cmp)
{
    voy_htable_t *htable = malloc(sizeof(voy_htable_t));

    if (!htable) {
        return NULL;
    }

    htable->cmp          = cmp;
    htable->buckets      = voy_array_new(NUM_OF_BUCKETS, sizeof(voy_array_t *));
    htable->buckets->len = htable->buckets->capacity;

    return htable;
}

void voy_htable_free(voy_htable_t *htable, voy_htable_cb cb)
{
    if (!htable) {
        return;
    }

    if (htable->buckets) {
        for (int i = 0; i < htable->buckets->len; i++) {
            voy_array_t *bucket = voy_array_get(htable->buckets, i);
            if (bucket) {
                for (int j = 0; j < bucket->len; j++) {
                    voy_htable_node_t *elem = voy_array_get(bucket, j);
                    if (elem) {
                        if (cb) {
                            cb(elem->key, elem->value);
                        }
                        free(elem);
                    }
                }
                voy_array_free(bucket, NULL);
            }
        }
        voy_array_free(htable->buckets, NULL);
    }
    free(htable);
}

void voy_htable_set(voy_htable_t *htable, void *key, void *value)
{
    if (!htable) {
        return;
    }

    size_t bucket_hash  = 0;
    voy_array_t *bucket = voy_htable_find_bucket(htable, key, &bucket_hash, true);

    if (!bucket) {
        return;
    }

    // create the node
    voy_htable_node_t *node = malloc(sizeof(voy_htable_node_t));
    if (!node) {
        return;
    }

    node->key   = key;
    node->value = value;
    node->hash  = bucket_hash;
    voy_array_push(bucket, node);
}

void *voy_htable_get(voy_htable_t *htable, void *key)
{
    if (!htable) {
        return NULL;
    }

    size_t bucket_hash  = 0;
    voy_array_t *bucket = voy_htable_find_bucket(htable, key, &bucket_hash, false);
    if (!bucket) {
        return NULL;
    }

    for (int i = 0; i < bucket->len; i++) {
        voy_htable_node_t *node = voy_array_get(bucket, i);
        if (node && node->hash == bucket_hash && htable->cmp(node->key, key) == 0) {
            return node->value;
        }
    }

    return NULL;
}

void *voy_htable_remove(voy_htable_t *htable, void *key, voy_htable_cb cb)
{
    if (!htable) {
        return NULL;
    }

    void *value         = NULL;
    size_t bucket_hash  = 0;
    voy_array_t *bucket = voy_htable_find_bucket(htable, key, &bucket_hash, false);

    if (!bucket) {
        return value;
    }

    for (int i = 0; i < bucket->len; i++) {
        voy_htable_node_t *node = voy_array_get(bucket, i);
        if (node && node->hash == bucket_hash && htable->cmp(node->key, key) == 0) {
            value      = node->value;
            void *last = voy_array_pop(bucket);
            if (last != node) {
                voy_array_set(bucket, last, i);
            }

            if (cb) {
                cb(node->key, node->value);
            }

            free(node);
            break;
        }
    }

    return value;
}

void voy_htable_traverse(voy_htable_t *htable, voy_htable_cb cb)
{
    if (!htable) {
        return;
    }

    for (int i = 0; i < htable->buckets->len; i++) {
        voy_array_t *bucket = voy_array_get(htable->buckets, i);
        if (bucket) {
            for (int j = 0; j < bucket->len; j++) {
                voy_htable_node_t *node = voy_array_get(bucket, j);

                if (node) {
                    if (cb) {
                        cb(node->key, node->value);
                    }
                }
            }
        }
    }
}
