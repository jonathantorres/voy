#ifndef _voy_dl_list_h
#define _voy_dl_list_h

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct voy_dl_list_node {
    void *value;
    struct voy_dl_list_node *next;
    struct voy_dl_list_node *prev;
} voy_dl_list_node_t;

typedef struct voy_dl_list {
    voy_dl_list_node_t *first;
} voy_dl_list_t;

typedef int (*voy_dl_list_cmp)(void *a, void *b);
typedef void (*voy_dl_list_free_cb)(void *value);

voy_dl_list_t *voy_dl_list_new();
void voy_dl_list_clear(voy_dl_list_t *list, voy_dl_list_free_cb cb);
void voy_dl_list_free(voy_dl_list_t *list, voy_dl_list_free_cb cb);
int voy_dl_list_len(voy_dl_list_t *list);
void voy_dl_list_push(voy_dl_list_t *list, void *value);
void voy_dl_list_shift(voy_dl_list_t *list, void *value);
void *voy_dl_list_unshift(voy_dl_list_t *list);
void *voy_dl_list_pop(voy_dl_list_t *list);
void voy_dl_list_remove(voy_dl_list_t *list, void *value, voy_dl_list_cmp cmp,
                        voy_dl_list_free_cb cb);
bool voy_dl_list_exists(voy_dl_list_t *list, void *value, voy_dl_list_cmp cmp);

// Macro usage:
// VOY_DL_LIST_FOREACH(list) {
// your code here
// you can use the variable "cur"
// inside of it that references the current item of the list
// }
#define VOY_DL_LIST_FOREACH(list)                                                                  \
    for (voy_dl_list_node_t *cur = (list)->first; cur != NULL; cur = cur->next)

#endif
