#include "voy_conf.h"

#define VOY_CONF_LINE_BUF_SIZE 1024
#define VOY_CONF_LINES 50

voy_array_t *voy_open_and_strip_comments(char *conf_file_path);
voy_array_t *voy_parse_includes(voy_array_t *conf_arr);
void voy_check_for_syntax_errors(voy_array_t *conf_arr);
void voy_build_server_conf(voy_array_t *conf_arr);
char *voy_strip_comment_from_line(char *line);

voy_conf_t *voy_conf_load(char *conf_file_path)
{
    voy_array_t *conf_arr = voy_open_and_strip_comments(conf_file_path);
    if (!conf_arr) {
        // TODO: log the error
        return NULL;
    }

    conf_arr = voy_parse_includes(conf_arr); // TODO
    voy_check_for_syntax_errors(conf_arr); // TODO
    voy_build_server_conf(conf_arr); // TODO

    VOY_ARRAY_FOREACH(conf_arr) {
        voy_str_t *cur_line = voy_array_get(conf_arr, i);
        printf("%s", cur_line->string);
    }
    return NULL;
}

voy_array_t *voy_open_and_strip_comments(char *conf_file_path)
{
    FILE *fp = fopen(conf_file_path, "r");
    if (!fp) {
        // TODO: log this
        return NULL;
    }

    char *buf = malloc(VOY_CONF_LINE_BUF_SIZE);
    if (!buf) {
        // TODO: log this
        fclose(fp);
        return NULL;
    }
    memset(buf, 0, VOY_CONF_LINE_BUF_SIZE);

    voy_array_t *conf_file_lines = voy_array_new(VOY_CONF_LINES, sizeof(voy_str_t*));
    if (!conf_file_lines) {
        // TODO: log this
        free(buf);
        fclose(fp);
        return NULL;
    }

    while (fgets(buf, VOY_CONF_LINE_BUF_SIZE, fp) != NULL) {
        // ignore if line starts as a comment
        if (*buf == VOY_COMMENT_SIGN) {
            continue;
        }
        buf = voy_strip_comment_from_line(buf);
        voy_str_t *cur_line = voy_str_new(buf);
        if (!cur_line) {
            // TODO: log this
            // TODO: clean up the array (conf_file_lines) and the buffer (buf)
            free(buf);
            fclose(fp);
            return NULL;
        }
        voy_array_push(conf_file_lines, cur_line);
    }

    fclose(fp);
    free(buf);

    return conf_file_lines;
}

char *voy_strip_comment_from_line(char *line)
{
    size_t len = strlen(line);
    int comment_index = -1;

    for (int i = 0; i < (int)len; i++) {
        if (line[i] == VOY_COMMENT_SIGN) {
            comment_index = i;
            break;
        }
    }
    if (comment_index >= 0) {
        line[comment_index] = '\n';
        line[comment_index+1] = '\0';
    }
    return line;
}

voy_array_t *voy_parse_includes(voy_array_t *conf_arr)
{
    return conf_arr;
}

void voy_check_for_syntax_errors(voy_array_t *conf_arr)
{
    if (conf_arr) {}
    return;
}

void voy_build_server_conf(voy_array_t *conf_arr)
{
    if (conf_arr) {}
    return;
}
