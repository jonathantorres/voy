#include "voy_htable.h"
#include "voy_unittest.h"
#include <string.h>

int gen_random_number(int lower, int upper)
{
    return (rand() % (upper - lower + 1)) + lower;
}

char *alloc_rand_str()
{
    char *str = malloc(20);
    memset(str, '\0', 20);
    for (int i = 0; i < 19; i++) {
        char c = gen_random_number(97, 122);
        str[i] = c;
    }
    return str;
}

int voy_htable_compare_fn(void *a, void *b)
{
    return strcmp((char *)a, (char *)b);
}

int voy_htable_int_compare_fn(void *a, void *b)
{
    int val1 = *(int *)a;
    int val2 = *(int *)b;
    if (val1 > val2) {
        return 1;
    } else if (val1 < val2) {
        return -1;
    }
    return 0;
}

void voy_htable_traverse_fn(void *key, void *value)
{
    if (key) {
        // nothing to do here
    }
    if (value) {
        // nothing to do here
    }
    // printf("%s:%s,", (char*)key, (char*)value);
}

void voy_htable_free_fn(void *key, void *value)
{
    if (key) {
        // nothing to free here
    }
    if (value) {
        // nothing to free here
    }
}

void voy_htable_free_cb_fn(void *key, void *value)
{
    if (key) {
        free(key);
    }
    if (value) {
        free(value);
    }
}

char *test_create()
{
    voy_htable_t *htable = voy_htable_new(voy_htable_compare_fn);
    voy_assert(htable != NULL, "Hasvoy_htable cannot be NULL");
    voy_assert(htable->cmp == voy_htable_compare_fn,
               "Hasvoy_htable must use the correct compare function");
    voy_assert(htable->buckets != NULL, "Hasvoy_htable buckets cannot be NULL");
    voy_htable_free(htable, NULL);
    return NULL;
}

char *test_free()
{
    voy_htable_t *htable = voy_htable_new(voy_htable_compare_fn);
    voy_htable_free(htable, NULL);
    return NULL;
}

char *test_set()
{
    voy_htable_t *htable = voy_htable_new(voy_htable_compare_fn);
    voy_htable_set(htable, "john", "Jonathan Torres");
    voy_htable_set(htable, "luis", "Jorge L Torres");
    voy_htable_set(htable, "mar", "Marjory Torres");
    voy_htable_set(htable, "chris", "Christopher Torres");
    voy_htable_set(htable, "chris", "Christopher Torres");

    voy_assert(voy_htable_get(htable, "john") != NULL, "Item john should be on the table");
    voy_assert(voy_htable_get(htable, "luis") != NULL, "Item luis should be on the table");
    voy_assert(voy_htable_get(htable, "mar") != NULL, "Item mar should be on the table");
    voy_assert(voy_htable_get(htable, "chris") != NULL, "Item chris should be on the table");
    voy_htable_free(htable, NULL);

    return NULL;
}

char *test_get()
{
    char *first_key    = "john";
    char *second_key   = "luis";
    char *first_value  = "Jonathan Torres";
    char *second_value = "Jorge L Torres";

    voy_htable_t *htable = voy_htable_new(voy_htable_compare_fn);
    voy_htable_set(htable, first_key, first_value);
    voy_htable_set(htable, second_key, second_value);

    char *value = voy_htable_get(htable, first_key);
    voy_assert(value != NULL, "Value for key 'john' shouldn't be NULL");
    voy_assert(strcmp(value, first_value) == 0, "Value for key 'john' should be 'Jonathan Torres'");
    value = voy_htable_get(htable, second_key);
    voy_assert(value != NULL, "Value for key 'luis' shouldn't be NULL");
    voy_assert(strcmp(value, second_value) == 0, "Value for key 'luis' should be 'Jorge L Torres'");
    value = voy_htable_get(htable, "foo");
    voy_assert(value == NULL, "Value for key 'foo' should be NULL");
    voy_htable_free(htable, NULL);

    return NULL;
}

char *test_remove()
{
    char *first_key    = "john";
    char *second_key   = "luis";
    char *first_value  = "Jonathan Torres";
    char *second_value = "Jorge L Torres";

    voy_htable_t *htable = voy_htable_new(voy_htable_compare_fn);
    voy_htable_set(htable, first_key, first_value);
    voy_htable_set(htable, second_key, second_value);
    char *value = voy_htable_remove(htable, first_key, NULL);
    voy_assert(value != NULL, "Value for key 'john' could not be removed");
    voy_assert(strcmp(value, first_value) == 0,
               "Value for the removed key 'john' should be 'Jonathan Torres'");
    value = voy_htable_get(htable, first_key);
    voy_assert(value == NULL, "Value for the removed key 'john' should be NULL");
    voy_htable_free(htable, NULL);

    return NULL;
}

char *test_traverse()
{
    char *first_key    = "john";
    char *second_key   = "luis";
    char *first_value  = "Jonathan Torres";
    char *second_value = "Jorge L Torres";

    voy_htable_t *htable = voy_htable_new(voy_htable_compare_fn);
    voy_htable_set(htable, first_key, first_value);
    voy_htable_set(htable, second_key, second_value);
    voy_htable_traverse(htable, voy_htable_traverse_fn);
    voy_htable_free(htable, NULL);

    return NULL;
}

char *test_benchmark()
{
    voy_htable_t *htable = voy_htable_new(voy_htable_compare_fn);

    for (int i = 0; i < 1000; i++) {
        char *key = alloc_rand_str();
        char *val = alloc_rand_str();
        voy_htable_set(htable, key, val);
    }
    voy_htable_free(htable, voy_htable_free_cb_fn);
    return NULL;
}

char *test_int_keys()
{
    int key1   = 30;
    int key2   = 40;
    int key3   = 41;
    char *val1 = "Jonathan Torres";
    char *val2 = "Jorge L Torres";
    char *val3 = "This is another thing";

    voy_htable_t *htable = voy_htable_new(voy_htable_int_compare_fn);
    voy_htable_set(htable, &key1, val1);
    voy_htable_set(htable, &key2, val2);
    voy_htable_set(htable, &key3, val3);
    char *value = voy_htable_get(htable, &key2);
    voy_assert(value != NULL, "Value for key '40' shouldn't be NULL");
    voy_assert(strcmp(value, val2) == 0, "Value for key '40' should be 'Jorge L Torres'");
    value = voy_htable_get(htable, &key3);
    voy_assert(value != NULL, "Value for key '41' shouldn't be NULL");
    voy_assert(strcmp(value, val3) == 0, "Value for key '41' should be 'This is another thing'");

    voy_htable_remove(htable, &key1, NULL);
    value = voy_htable_remove(htable, &key2, NULL);
    voy_assert(value != NULL, "Value for key '40' shouldn't be NULL");
    voy_assert(strcmp(value, val2) == 0, "Value for key '40' should be 'Jorge L Torres'");
    voy_htable_free(htable, NULL);
    return NULL;
}

int main()
{
    voy_start_tests("voy hash table tests");
    voy_run_test(test_create);
    voy_run_test(test_free);
    voy_run_test(test_set);
    voy_run_test(test_get);
    voy_run_test(test_remove);
    voy_run_test(test_traverse);
    voy_run_test(test_benchmark);
    voy_run_test(test_int_keys);
    voy_end_tests();

    return 0;
}
