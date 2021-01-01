#include "voy_dl_list.h"

voy_dl_list_node_t *_voy_dl_list_create_node(void *value)
{
    voy_dl_list_node_t *node = malloc(sizeof(voy_dl_list_node_t));

    if (!node) {
        return NULL;
    }

    node->prev = NULL;
    node->next = NULL;
    node->value = value;

    return node;
}

void _voy_dl_list_free_node(voy_dl_list_node_t *node, voy_dl_list_free_cb cb)
{
    if (!node) {
        return;
    }
    if (cb) {
        cb(node->value);
    }

    node->prev = NULL;
    node->next = NULL;
    node->value = NULL;

    free(node);
}

// create a new list
voy_dl_list_t *voy_dl_list_new()
{
    voy_dl_list_t *list = malloc(sizeof(voy_dl_list_t));

    if (!list) {
        return NULL;
    }

    list->first = NULL;

    return list;
}

// remove all the values in the list
void voy_dl_list_clear(voy_dl_list_t *list, voy_dl_list_free_cb cb)
{
    if (list->first != NULL) {
        voy_dl_list_node_t *current_node = list->first;

        while (current_node->next != NULL) {
            current_node = current_node->next;

            if (current_node->prev) {
                _voy_dl_list_free_node(current_node->prev, cb);
            }
        }

        if (current_node) {
            _voy_dl_list_free_node(current_node, cb);
        }

        list->first = NULL;
    }
}

// destroy the list
void voy_dl_list_free(voy_dl_list_t *list, voy_dl_list_free_cb cb)
{
    if (!list) {
        return;
    }

    voy_dl_list_clear(list, cb);
    free(list);
}

// get the len of the list
int voy_dl_list_len(voy_dl_list_t *list)
{
    if (!list) {
        return -1;
    }

    int len = 0;

    if (list->first != NULL) {
        voy_dl_list_node_t *current_node = list->first;
        len++;

        while (current_node->next != NULL) {
            current_node = current_node->next;
            len++;
        }
    }

    return len;
}

// insert at the end
void voy_dl_list_push(voy_dl_list_t *list, void *value)
{
    if (!list) {
        return;
    }

    voy_dl_list_node_t *node = _voy_dl_list_create_node(value);

    // list is empty, this is the first element
    if (list->first == NULL) {
        list->first = node;
        return;
    }

    voy_dl_list_node_t *current_node = list->first;

    while (current_node->next != NULL) {
        current_node = current_node->next;
    }

    node->prev = current_node;
    current_node->next = node;
}

// insert at the beginning
void voy_dl_list_shift(voy_dl_list_t *list, void *value)
{
    if (!list) {
        return;
    }

    voy_dl_list_node_t *node = _voy_dl_list_create_node(value);

    // list is empty, this is the first element
    if (list->first == NULL) {
        list->first = node;
        return;
    }

    list->first->prev = node;
    node->next = list->first;
    list->first = node;
}

// remove the first node and return it
void *voy_dl_list_unshift(voy_dl_list_t *list)
{
    if (!list) {
        return NULL;
    }

    // list is empty, return nothing
    if (list->first == NULL) {
        return NULL;
    }

    // list has just 1 node
    if (list->first->next == NULL) {
        void *value = list->first->value;
        _voy_dl_list_free_node(list->first, NULL);
        list->first = NULL;

        return value;
    }

    voy_dl_list_node_t *new_first = list->first->next;
    void *value = list->first->value;

    _voy_dl_list_free_node(list->first, NULL);
    new_first->prev = NULL;
    list->first = new_first;

    return value;
}

// remove the last node and return it
void *voy_dl_list_pop(voy_dl_list_t *list)
{
    if (!list) {
        return NULL;
    }

    // list is empty, return nothing
    if (list->first == NULL) {
        return NULL;
    }

    // list has just 1 node
    if (list->first->next == NULL) {
        void *value = list->first->value;
        _voy_dl_list_free_node(list->first, NULL);
        list->first = NULL;

        return value;
    }

    voy_dl_list_node_t *current_node = list->first;

    while (current_node->next != NULL) {
        current_node = current_node->next;
    }

    void *value = current_node->value;
    current_node->prev->next = NULL;
    _voy_dl_list_free_node(current_node, NULL);

    return value;
}

// remove node whose value is {value}
void voy_dl_list_remove(voy_dl_list_t *list, void *value, voy_dl_list_cmp cmp, voy_dl_list_free_cb cb)
{
    if (!list) {
        return;
    }

    // list is empty, return nothing
    if (list->first == NULL) {
        return;
    }

    // list has just 1 node
    if (list->first->next == NULL) {
        void *node_value = list->first->value;

        if (cmp(node_value, value) == 0) {
            _voy_dl_list_free_node(list->first, cb);
        }

        return;
    }

    voy_dl_list_node_t *current_node = list->first;

    // check the first one
    if (cmp(current_node->value, value) == 0) {
        voy_dl_list_node_t *next_node = current_node->next;
        next_node->prev = NULL;
        list->first = next_node;
        _voy_dl_list_free_node(current_node, cb);
        return;
    }

    while (current_node->next != NULL) {
        current_node = current_node->next;

        if (cmp(current_node->value, value) == 0) {
            // remove the node
            current_node->prev->next = current_node->next;
            current_node->next->prev = current_node->prev;
            _voy_dl_list_free_node(current_node, cb);
            break;
        }
    }
}

// check to see if value {value} exists in the list
bool voy_dl_list_exists(voy_dl_list_t *list, void *value, voy_dl_list_cmp cmp)
{
    if (!list) {
        return -1;
    }

    // list is empty, not found
    if (list->first == NULL) {
        return false;
    }

    voy_dl_list_node_t *current_node = list->first;

    // check the first one
    if (cmp(current_node->value, value) == 0) {
        return true;
    }

    bool found = false;

    // check the rest
    while (current_node->next != NULL) {
        current_node = current_node->next;

        if (cmp(current_node->value, value) == 0) {
            found = true;
            break;
        }
    }

    return found;
}
