#include "voy_str.h"

static char *_create_string(char *chars)
{
    int len = strlen(chars);
    char *string = malloc(len+1);
    if (!string) {
        return NULL;
    }
    memset(string, 0, len+1);
    char *str_p = string;
    for (char *c = chars; *c != '\0'; c++) {
        *str_p = *c;
        str_p++;
    }
    return string;
}

static char *_concat_chars(char *cur_str, char *chars)
{
    unsigned long chars_len = strlen(chars);
    if (chars_len == 0) {
        return cur_str;
    }

    char *new_str = realloc(cur_str, chars_len+1);
    if (!new_str) {
        return cur_str;
    }
    char *new_str_p = new_str;
    char *chars_p = chars;
    for (; *new_str_p != '\0'; new_str_p++) {
        // moving the pointer to the end of the current string
    }
    memset(new_str_p, 0, chars_len+1);
    for (; *chars_p != '\0'; chars_p++) {
        *new_str_p = *chars_p;
        new_str_p++;
    }
    *new_str_p = '\0';

    return new_str;
}

voy_str_t *voy_str_new(char *chars)
{
    voy_str_t *str = malloc(sizeof(voy_str_t));
    if (!str) {
        return NULL;
    }

    str->len = 0;
    str->string = NULL;
    str->start_p = NULL;
    if (!chars) {
        char *empty_str = _create_string("");
        if (!empty_str) {
            free(str);
            return NULL;
        }
        str->string = empty_str;
        str->start_p = empty_str;
        return str;
    }

    str->len = strlen(chars);
    str->string = _create_string(chars);
    str->start_p = str->string;
    return str;
}

void voy_str_free(voy_str_t *str)
{
    if (!str) {
        return;
    }
    if (str->start_p) {
        free(str->start_p);
    }
    str->start_p = NULL;
    str->string = NULL;
    free(str);
}

int voy_str_len(voy_str_t *str)
{
    if (!str) {
        return 0;
    }
    return str->len;
}

char *voy_str_get_string(voy_str_t *str)
{
    if (!str) {
        return NULL;
    }
    if (!str->string) {
        return NULL;
    }
    return str->string;
}

voy_str_t *voy_str_concat_voy_str(voy_str_t *str, voy_str_t *chars)
{
    if (!str || !chars) {
        return str;
    }
    unsigned long chars_len = strlen(chars->string);

    str->string = _concat_chars(str->string, chars->string);
    str->start_p = str->string;
    str->len += chars_len;

    return str;
}

voy_str_t *voy_str_concat(voy_str_t *str, char *chars)
{
    if (!str || !chars) {
        return str;
    }
    unsigned long chars_len = strlen(chars);

    str->string = _concat_chars(str->string, chars);
    str->start_p = str->string;
    str->len += chars_len;

    return str;
}

char *voy_str_dup(char *str)
{
    if (!str) {
        return NULL;
    }
    size_t len = strlen(str);
    if (len == 0) {
        return NULL;
    }
    return _create_string(str);
}

voy_str_t *voy_str_dup_str(voy_str_t *str)
{
    if (!str) {
        return NULL;
    }
    if (str->len == 0 || !str->string) {
        return NULL;
    }
    return voy_str_new(str->string);
}

voy_array_t *voy_str_split_by_char(voy_str_t *str, char delim)
{
    if (!str) {
        return NULL;
    }
    if (str->len == 0 || !str->string) {
        return NULL;
    }

    // find out how many delimiters are in the string
    int delim_count = 0;
    for (int i = 0; i < (int)strlen(str->string); i++) {
        if (str->string[i] == delim) {
            delim_count++;
        }
    }
    if (delim_count == 0) {
        return NULL;
    }

    voy_array_t *splits = voy_array_new(delim_count, sizeof(voy_str_t*));
    if (!splits) {
        return NULL;
    }
    char *buf = malloc(strlen(str->string));
    if (!buf) {
        voy_array_free(splits, NULL);
        return NULL;
    }
    memset(buf, 0, strlen(str->string));
    char *buf_p = buf;

    for (int j = 0; j < (int)strlen(str->string); j++) {
        if (str->string[j] == delim || j == (int)(strlen(str->string) - 1)) {
            // in the current delimiter or the end of the string,
            // push the string to the array and reset the buffer
            if (j == (int)(strlen(str->string) - 1)) {
                *buf_p = str->string[j];
                buf_p++;
            }

            *buf_p = '\0';
            voy_str_t *part = voy_str_new(buf);
            voy_array_push(splits, part);
            buf_p = buf;
            continue;
        }

        *buf_p = str->string[j];
        buf_p++;
    }
    free(buf);

    return splits;
}

bool voy_str_contains(voy_str_t *str, char *substr)
{
    if (!str || !substr || !str->string) {
        return false;
    }
    if (strlen(substr) == 0) {
        return true;
    }

    char *found = strstr(str->string, substr);
    if (found) {
        return true;
    }
    return false;
}

bool voy_str_contains_voy_str(voy_str_t *str, voy_str_t *substr)
{
    if (!str || !substr || !str->string || !substr->string) {
        return false;
    }
    if (strlen(substr->string) == 0) {
        return true;
    }

    char *found = strstr(str->string, substr->string);
    if (found) {
        return true;
    }
    return false;
}

bool voy_str_has_prefix(voy_str_t *str, char *prefix)
{
    if (!str || !prefix || !str->string) {
        return false;
    }
    if (strlen(prefix) == 0) {
        return true;
    }

    char *pos = strstr(str->string, prefix);
    if (!pos) {
        return false;
    }
    return (pos - str->string) == 0;
}

bool voy_str_has_prefix_voy_str(voy_str_t *str, voy_str_t *prefix)
{
    if (!str || !prefix || !str->string || !prefix->string) {
        return false;
    }
    if (strlen(prefix->string) == 0) {
        return true;
    }

    char *pos = strstr(str->string, prefix->string);
    if (!pos) {
        return false;
    }

    return (pos - str->string) == 0;
}

bool voy_str_has_suffix(voy_str_t *str, char *suffix)
{
    if (!str || !suffix || !str->string) {
        return false;
    }
    if (strlen(suffix) == 0) {
        return true;
    }
    if (strlen(suffix) > strlen(str->string)) {
        return false;
    }

    char *str_p = (str->string + strlen(str->string)) - strlen(suffix);
    if (strcmp(str_p, suffix) == 0) {
        return true;
    }
    return false;
}

bool voy_str_has_suffix_voy_str(voy_str_t *str, voy_str_t *suffix)
{
    if (!str || !suffix || !str->string || !suffix->string) {
        return false;
    }
    if (strlen(suffix->string) == 0) {
        return true;
    }
    if (strlen(suffix->string) > strlen(str->string)) {
        return false;
    }

    char *str_p = (str->string + strlen(str->string)) - strlen(suffix->string);
    if (strcmp(str_p, suffix->string) == 0) {
        return true;
    }
    return false;
}
