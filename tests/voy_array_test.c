#include "voy_unittest.h"
#include "voy_array.h"
#include <stdio.h>
#include <string.h>

// utility method to print the contents of an array
// in this test we'll be using an array of numbers
void voy_array_print(voy_array_t *arr, char type)
{
    if (!arr) {
        fputs("Must provide an array.", stderr);
        exit(EXIT_FAILURE);
    }

    printf("[");
    VOY_ARRAY_FOREACH(arr) {
        void *val = NULL;
        switch (type) {
            case 'i':
                val = (int*)voy_array_get(arr, i);
                printf("%d,", *(int*)val);
            break;

            case 's':
                val = voy_array_get(arr, i);
                printf("%s,", (char*)val);
            break;
        }
    }
    printf("]\n");
}

void voy_array_free_cb_func(void *value)
{
    if (value) {
        free(value);
    }
}

char *test_create()
{
    voy_array_t *arr = voy_array_new(10, sizeof(int*));

    voy_assert(arr->len == 0, "Array len should be 0");
    voy_assert(arr->capacity == 10, "Array capacity should be 10");
    voy_assert(arr->expand_rate == 100, "Array expand_rate should be 100");
    voy_assert(arr->item_size == sizeof(int*), "Array item_size is not correct, it should be sizeof(int*)");
    voy_assert(arr->contents != NULL, "Array contents should not be NULL");

    voy_array_free(arr, NULL);

    return NULL;
}

char *test_free()
{
    voy_array_t *arr = voy_array_new(100, sizeof(int*));

    for (unsigned int i = 0; i < 100; i++) {
        int *value = malloc(sizeof(int));
        if (value != NULL) {
            *value = i * 5;
            voy_array_push(arr, value);
        }
    }

    voy_assert(arr->len == 100, "Array len must be 100");
    voy_assert(arr->capacity == 200, "Array capacity must be 200");
    voy_assert(arr->contents != NULL, "Array contents should not be NULL");
    voy_array_free(arr, voy_array_free_cb_func);

    return NULL;
}

char *test_clear()
{
    voy_array_t *arr = voy_array_new(100, sizeof(int*));

    for (unsigned int i = 0; i < 100; i++) {
        int *value = malloc(sizeof(int));
        if (value != NULL) {
            *value = i * 5;
            voy_array_push(arr, value);
        }
    }

    voy_assert(arr->len == 100, "Array len must be 100");
    voy_assert(arr->capacity == 200, "Array capacity should be 200");
    voy_assert(arr->contents != NULL, "Array contents should not be NULL");

    voy_array_clear(arr, voy_array_free_cb_func);
    voy_assert(arr->len == 0, "Array len must be 0");
    voy_array_free(arr, NULL);

    return NULL;
}

char *test_push()
{
    voy_array_t *arr = voy_array_new(10, sizeof(int*));

    for (unsigned int i = 0; i < 100; i++) {
        int *value = malloc(sizeof(int));
        if (value != NULL) {
            *value = i * 5;
            voy_array_push(arr, value);
        }
    }

    voy_assert(arr->capacity == 110, "Array capacity should be 110");
    voy_assert(arr->len == 100, "Array len should be 100");
    voy_assert(arr->contents != NULL, "Array contents should not be NULL");
    voy_array_free(arr, voy_array_free_cb_func);

    return NULL;
}

char *test_pop()
{
    voy_array_t *arr = voy_array_new(10, sizeof(int*));

    for (unsigned int i = 0; i < 5; i++) {
        int *value = malloc(sizeof(int));
        if (value != NULL) {
            *value = i * 5;
            voy_array_push(arr, value);
        }
    }

    voy_assert(arr->len == 5, "Array len should be 5");
    int *last_num = voy_array_pop(arr);
    voy_assert(*last_num == 20, "Last element's value should be 20");
    voy_assert(arr->len == 4, "Array len should be 4");
    voy_assert(arr->contents != NULL, "Array contents should not be NULL");
    voy_array_free(arr, voy_array_free_cb_func);
    free(last_num);

    return NULL;
}

char *test_set()
{
    voy_array_t *arr = voy_array_new(10, sizeof(int*));

    for (unsigned int i = 0; i < 5; i++) {
        int *value = malloc(sizeof(int));
        if (value != NULL) {
            *value = i * 5;
            voy_array_set(arr, value, i);
        }
    }

    voy_assert(arr->capacity == 10, "Array capacity should be 10");
    voy_assert(arr->len == 5, "Array len should be 5");
    voy_assert(arr->contents != NULL, "Array contents should not be NULL");
    voy_array_free(arr, voy_array_free_cb_func);

    return NULL;
}

char *test_get()
{
    voy_array_t *arr = voy_array_new(10, sizeof(int*));

    for (unsigned int i = 0; i < 5; i++) {
        int *value = malloc(sizeof(int));
        if (value != NULL) {
            *value = i * 5;
            voy_array_push(arr, value);
        }
    }

    int *number = voy_array_get(arr, 4);
    void *off_number = voy_array_get(arr, 100);
    voy_assert(*number == 20, "Element's value should be 20");
    voy_assert(off_number == NULL, "The off number should be NULL");
    voy_array_free(arr, voy_array_free_cb_func);

    return NULL;
}

char *test_remove()
{
    voy_array_t *arr = voy_array_new(10, sizeof(int*));

    for (unsigned int i = 0; i < 5; i++) {
        int *value = malloc(sizeof(int));
        if (value != NULL) {
            *value = i * 5;
            voy_array_push(arr, value);
        }
    }

    voy_assert(arr->len == 5, "Array len should be 5");
    int *number = voy_array_remove(arr, 1);
    voy_assert(*number == 5, "Element's value should be 5");
    voy_assert(arr->len == 4, "Array len should be 4");
    voy_array_free(arr, voy_array_free_cb_func);
    free(number);

    return NULL;
}

char *test_shift()
{
    voy_array_t *arr = voy_array_new(10, sizeof(int*));

    for (unsigned int i = 0; i < 5; i++) {
        int *value = malloc(sizeof(int));
        if (value != NULL) {
            *value = i * 5;
            voy_array_push(arr, value);
        }
    }

    voy_assert(arr->len == 5, "Array len should be 5");
    int *new_value = malloc(sizeof(int));
    if (new_value != NULL) {
        *new_value = 200;
        voy_array_shift(arr, new_value);
    }
    int *val = voy_array_get(arr, 0);
    voy_assert(*val == 200, "Value of new element should be 200");
    voy_assert(arr->len == 6, "Array len should be 6");
    voy_array_free(arr, voy_array_free_cb_func);

    return NULL;
}

char *test_unshift()
{
    voy_array_t *arr = voy_array_new(10, sizeof(int*));

    for (unsigned int i = 0; i < 5; i++) {
        int *value = malloc(sizeof(int));
        if (value != NULL) {
            *value = i * 5;
            voy_array_push(arr, value);
        }
    }

    voy_assert(arr->len == 5, "Array len should be 5");
    int *first_num = voy_array_unshift(arr);
    voy_assert(*first_num == 0, "Value of removed element should be 0");
    voy_assert(arr->len == 4, "Array len should be 4");
    voy_array_free(arr, voy_array_free_cb_func);
    free(first_num);

    return NULL;
}

char *test_array_of_strings()
{
    voy_array_t *arr = voy_array_new(10, sizeof(char**));
    char *strings[5] = {
        "foo",
        "bar",
        "baz",
        "hello",
        "again",
    };

    for (unsigned int i = 0; i < 5; i++) {
        char *string = malloc(10);
        if (string != NULL) {
            strcpy(string, strings[i]);
            voy_array_push(arr, string);
        }
    }

    voy_assert(arr->contents != NULL, "Array contents should not be NULL");
    voy_assert(arr->len == 5, "Array len should be 5");
    char *last = voy_array_pop(arr);
    voy_assert(strcmp(last, strings[4]) == 0, "Strings 'again' should be equal");
    voy_assert(arr->len == 4, "Array len should be 4");
    char *first = voy_array_unshift(arr);
    voy_assert(strcmp(first, strings[0]) == 0, "Strings 'foo' should be equal");
    voy_assert(arr->len == 3, "Array len should be 3");

    voy_array_free(arr, voy_array_free_cb_func);
    free(last);
    free(first);

    return NULL;
}

char *test_array_stack_items()
{
    voy_array_t *arr = voy_array_new(10, sizeof(char*));

    voy_array_push(arr, "John");
    voy_array_push(arr, "Jonathan");
    voy_array_push(arr, "George");

    voy_assert(arr->contents != NULL, "Array contents should not be NULL");
    voy_assert(arr->len == 3, "Array len should be 3");
    char *last = voy_array_pop(arr);
    voy_assert(strcmp(last, "George") == 0, "String 'George' should be equal");
    voy_assert(arr->len == 2, "Array len should be 2");
    char *first = voy_array_unshift(arr);
    voy_assert(strcmp(first, "John") == 0, "String 'John' should be equal");
    voy_assert(arr->len == 1, "Array len should be 1");
    voy_array_free(arr, NULL);

    return NULL;
}

typedef struct person_dum {
    char *first_name;
    char *last_name;
    int age;
} person_dum_t;

char *test_array_struct_pointers()
{
    voy_array_t *arr = voy_array_new(10, sizeof(person_dum_t*));
    person_dum_t *p = malloc(sizeof(person_dum_t));
    p->first_name = "Jonathan";
    p->last_name = "Torres";
    p->age = 33;

    voy_array_push(arr, p);
    voy_assert(arr->contents != NULL, "Array contents should not be NULL");
    voy_assert(arr->len == 1, "Array len should be 1");
    person_dum_t *per = voy_array_get(arr, 0);
    voy_assert(strcmp(per->first_name, "Jonathan") == 0, "String 'Jonathan' should be equal");
    voy_array_free(arr, voy_array_free_cb_func);

    return NULL;
}

char *test_voy_array_swap()
{
    voy_array_t *arr = voy_array_new(10, sizeof(int*));

    for (unsigned int i = 0; i < 5; i++) {
        int *value = malloc(sizeof(int));
        if (value != NULL) {
            *value = i * 5;
            voy_array_push(arr, value);
        }
    }

    voy_assert(arr->len == 5, "Array len should be 5");
    voy_assert(*(int*)voy_array_get(arr, 0) == 0, "Index 0 of array should be zero");
    voy_assert(*(int*)voy_array_get(arr, 1) == 5, "Index 0 of array should be five");
    voy_array_swap(arr, 0, 1);
    voy_assert(*(int*)voy_array_get(arr, 0) == 5, "Index 0 of array should be five");
    voy_assert(*(int*)voy_array_get(arr, 1) == 0, "Index 0 of array should be zero");
    voy_assert(arr->len == 5, "Array len should be 5");
    voy_array_free(arr, voy_array_free_cb_func);

    return NULL;
}

int main()
{
    voy_start_tests("voy array tests");
    voy_run_test(test_create);
    voy_run_test(test_free);
    voy_run_test(test_clear);
    voy_run_test(test_push);
    voy_run_test(test_pop);
    voy_run_test(test_set);
    voy_run_test(test_get);
    voy_run_test(test_remove);
    voy_run_test(test_shift);
    voy_run_test(test_unshift);
    voy_run_test(test_array_of_strings);
    voy_run_test(test_array_stack_items);
    voy_run_test(test_array_struct_pointers);
    voy_run_test(test_voy_array_swap);
    voy_end_tests();

    return 0;
}
