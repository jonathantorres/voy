#ifndef _voy_str_h
#define _voy_str_h

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "voy_array.h"

typedef struct voy_str {
    char *string;
    char *start_p;
    int len;
} voy_str_t;

voy_str_t *voy_str_new(char *chars);
void voy_str_free(voy_str_t *str);
int voy_str_len(voy_str_t *str);
char *voy_str_get_string(voy_str_t *str);
char *voy_str_dup(char *str);
voy_str_t *voy_str_dup_str(voy_str_t *str);
voy_str_t *voy_str_concat(voy_str_t *str, char *chars);
voy_str_t *voy_str_concat_voy_str(voy_str_t *str, voy_str_t *chars);
voy_array_t *voy_str_split_by_char(voy_str_t *str, char delim);
void voy_str_trim(voy_str_t *str);

bool voy_str_contains(voy_str_t *str, char *substr);
bool voy_str_contains_voy_str(voy_str_t *str, voy_str_t *substr);
bool voy_str_has_prefix(voy_str_t *str, char *prefix);
bool voy_str_has_prefix_voy_str(voy_str_t *str, voy_str_t *prefix);
bool voy_str_has_suffix(voy_str_t *str, char *suffix);
bool voy_str_has_suffix_voy_str(voy_str_t *str, voy_str_t *suffix);

#endif
