#include "voy_dl_list.h"
#include "voy_unittest.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// utility method to print the contents of a list
// the list used in this test, will be a list of strings
void voy_dl_list_print(voy_dl_list_t *list)
{
    if (!list) {
        fputs("Must provide a voy_dl_list.", stderr);
        exit(EXIT_FAILURE);
    }

    printf("{");
    VOY_DL_LIST_FOREACH (list) {
        printf("%s,", (char *)cur->value);
    }
    printf("}\n");
}

int cmp_func(void *a, void *b)
{
    return strcmp((char *)a, (char *)b);
}

void voy_dl_list_free_func(void *value)
{
    if (value) {
        free(value);
    }
}

char *test_create()
{
    voy_dl_list_t *list = voy_dl_list_new();

    voy_assert(list != NULL, "Failed creating the list");
    voy_assert(voy_dl_list_len(list) == 0, "List should have 0 nodes");
    voy_assert(list->first == NULL, "First item in the list should be NULL");
    voy_dl_list_free(list, NULL);

    return NULL;
}

char *test_push()
{
    voy_dl_list_t *list = voy_dl_list_new();

    char *john  = "John";
    char *luis  = "Luis";
    char *chris = "Chris";
    char *mar   = "Marjorie";

    voy_dl_list_push(list, john);
    voy_dl_list_push(list, luis);
    voy_dl_list_push(list, chris);
    voy_dl_list_push(list, mar);

    voy_assert(list->first->value == john, "John must be the first node in the list");
    voy_assert(voy_dl_list_len(list) == 4, "len of the list must be 4");
    voy_dl_list_free(list, NULL);

    return NULL;
}

char *test_destroy()
{
    voy_dl_list_t *list = voy_dl_list_new();

    voy_dl_list_push(list, "one");
    voy_dl_list_push(list, "two");
    voy_dl_list_push(list, "three");
    voy_dl_list_free(list, NULL);

    return NULL;
}

char *test_clear()
{
    voy_dl_list_t *list = voy_dl_list_new();

    voy_dl_list_push(list, "one");
    voy_dl_list_push(list, "two");
    voy_dl_list_push(list, "three");
    voy_dl_list_clear(list, NULL);
    voy_assert(voy_dl_list_len(list) == 0, "List len must be 0");
    voy_dl_list_free(list, NULL);

    return NULL;
}

char *test_shift()
{
    voy_dl_list_t *list = voy_dl_list_new();

    voy_dl_list_push(list, "one");
    voy_dl_list_push(list, "two");
    voy_dl_list_push(list, "three");

    voy_assert(voy_dl_list_len(list) == 3, "List len must be 3");
    voy_dl_list_shift(list, "zero");
    voy_assert(voy_dl_list_len(list) == 4, "List len must be 4");
    voy_dl_list_free(list, NULL);

    return NULL;
}

char *test_unshift()
{
    voy_dl_list_t *list = voy_dl_list_new();
    char *zero          = "zero";
    char *one           = "one";
    char *two           = "two";
    char *three         = "three";

    voy_dl_list_push(list, zero);
    voy_dl_list_push(list, one);
    voy_dl_list_push(list, two);
    voy_dl_list_push(list, three);

    voy_assert(voy_dl_list_len(list) == 4, "List len must be 4");
    char *value = (char *)voy_dl_list_unshift(list);
    voy_assert(voy_dl_list_len(list) == 3, "List len must be 3");
    voy_assert(strcmp(zero, value) == 0, "List should be equal");
    voy_dl_list_free(list, NULL);

    return NULL;
}

char *test_pop()
{
    voy_dl_list_t *list = voy_dl_list_new();
    char *zero          = "zero";
    char *one           = "one";
    char *two           = "two";
    char *three         = "three";

    voy_dl_list_push(list, zero);
    voy_dl_list_push(list, one);
    voy_dl_list_push(list, two);
    voy_dl_list_push(list, three);

    voy_assert(voy_dl_list_len(list) == 4, "List len must be 4");
    char *value = (char *)voy_dl_list_pop(list);
    voy_assert(voy_dl_list_len(list) == 3, "List len must be 3");
    voy_assert(strcmp(three, value) == 0, "List should be equal");
    voy_dl_list_free(list, NULL);

    return NULL;
}

char *test_remove()
{
    voy_dl_list_t *list = voy_dl_list_new();
    voy_dl_list_push(list, "zero");
    voy_dl_list_push(list, "one");
    voy_dl_list_push(list, "two");
    voy_dl_list_push(list, "three");

    voy_assert(voy_dl_list_len(list) == 4, "List len must be 4");
    voy_dl_list_remove(list, "zero", cmp_func, NULL);
    voy_assert(voy_dl_list_len(list) == 3, "List len must be 3");
    voy_dl_list_remove(list, "two", cmp_func, NULL);
    voy_assert(voy_dl_list_len(list) == 2, "List len must be 2");
    voy_dl_list_remove(list, "one", cmp_func, NULL);
    voy_assert(voy_dl_list_len(list) == 1, "List len must be 1");
    voy_dl_list_free(list, NULL);

    return NULL;
}

char *test_exists()
{
    voy_dl_list_t *list = voy_dl_list_new();
    voy_dl_list_push(list, "zero");
    voy_dl_list_push(list, "one");
    voy_dl_list_push(list, "two");
    voy_dl_list_push(list, "three");

    voy_assert(voy_dl_list_len(list) == 4, "List len must be 4");
    voy_assert(voy_dl_list_exists(list, "zero", cmp_func) == true, "zero should exist in the list");
    voy_assert(voy_dl_list_exists(list, "one", cmp_func) == true, "one should exist in the list");
    voy_assert(voy_dl_list_exists(list, "two", cmp_func) == true, "two should exist in the list");
    voy_assert(voy_dl_list_exists(list, "three", cmp_func) == true,
               "three should exist in the list");
    voy_assert(voy_dl_list_exists(list, "foo", cmp_func) == false,
               "foo shouldn't exist in the list");
    voy_assert(voy_dl_list_len(list) == 4, "List len must be 4");
    voy_dl_list_free(list, NULL);

    return NULL;
}

char *test_len()
{
    voy_dl_list_t *list = voy_dl_list_new();
    voy_dl_list_push(list, "zero");
    voy_dl_list_push(list, "one");
    voy_dl_list_push(list, "two");
    voy_dl_list_push(list, "three");
    voy_assert(voy_dl_list_len(list) == 4, "List len must be 4");
    voy_dl_list_free(list, NULL);

    return NULL;
}

int main()
{
    voy_start_tests("voy doubly linked list tests");
    voy_run_test(test_create);
    voy_run_test(test_push);
    voy_run_test(test_destroy);
    voy_run_test(test_clear);
    voy_run_test(test_shift);
    voy_run_test(test_unshift);
    voy_run_test(test_pop);
    voy_run_test(test_remove);
    voy_run_test(test_exists);
    voy_run_test(test_len);
    voy_end_tests();

    return 0;
}
