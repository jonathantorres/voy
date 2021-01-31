#include "voy_conf.h"

#define VOY_CONF_LINE_BUF_SIZE 1024
#define VOY_CONF_LINES 50

voy_array_t *voy_parse_name_options(voy_str_t *names);
voy_array_t *voy_parse_port_options(voy_str_t *ports);
voy_array_t *voy_open_and_strip_comments(char *conf_file_path);
voy_array_t *voy_parse_includes(voy_array_t *conf_arr);
voy_conf_t *voy_build_server_conf(voy_array_t *conf_arr);
voy_server_conf_t *voy_server_conf_new();
char *voy_strip_comment_from_line(char *line);
void voy_check_for_syntax_errors(voy_array_t *conf_arr);
void voy_server_conf_add_option(voy_server_conf_t *conf, voy_str_t *op_name, voy_str_t *op_value);
void voy_conf_add_option(voy_conf_t *conf, voy_str_t *op_name, voy_str_t *op_value);
void voy_conf_add_vhost(voy_conf_t *conf, voy_server_conf_t *vhost);
void voy_array_strs_free_cb(void *value);

voy_conf_t *voy_conf_load(char *conf_file_path)
{
    voy_array_t *conf_arr = voy_open_and_strip_comments(conf_file_path);
    if (!conf_arr) {
        // TODO: log the error
        return NULL;
    }

    conf_arr = voy_parse_includes(conf_arr); // TODO
    voy_check_for_syntax_errors(conf_arr); // TODO

    voy_conf_t *conf = voy_build_server_conf(conf_arr);
    if (!conf) {
        // TODO: log the error
        return NULL;
    }

    return conf;
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

voy_conf_t *voy_build_server_conf(voy_array_t *conf_arr)
{
    if (!conf_arr) {
        return NULL;
    }

    voy_conf_t *conf = malloc(sizeof(voy_conf_t));
    if (!conf) {
        // TODO: log the error
        return NULL;
    }
    memset(conf, 0, sizeof(voy_conf_t));

    conf->user           = NULL;
    conf->group          = NULL;
    conf->default_server = NULL;
    conf->vhosts         = NULL;

    voy_server_conf_t *cur_vhost = NULL;
    bool inside_vhost = false;

    VOY_ARRAY_FOREACH(conf_arr) {
        voy_str_t *line = voy_array_get(conf_arr, i);

        if (voy_str_contains_char(line, VOY_EQUAL_SIGN)) {
            // this is a line with an option
            voy_array_t *ops = voy_str_split_by_char(line, VOY_EQUAL_SIGN);
            if (!ops) {
                // log the error here
                continue;
            }
            voy_str_t *op_name = voy_array_get(ops, 0);
            voy_str_t *op_value = voy_array_get(ops, 1);
            voy_str_trim(op_name);
            voy_str_trim(op_value);
            if (inside_vhost) {
                // option for the current virtual host
                voy_server_conf_add_option(cur_vhost, op_name, op_value);
            } else {
                // top level or global option
                voy_conf_add_option(conf, op_name, op_value);
            }
            voy_array_free(ops, voy_array_strs_free_cb);
        } else if (voy_str_contains(line, VOY_VHOST_OPTION)) {
            // this is a line with a vhost command
            inside_vhost = true;
            cur_vhost    = voy_server_conf_new();
        } else if (voy_str_contains_char(line, VOY_CLOSING_BRACKET)) {
            // closing bracket for a vhost command
            if (inside_vhost) {
                voy_conf_add_vhost(conf, cur_vhost);
                cur_vhost    = NULL;
                inside_vhost = false;
            }
        }
        printf("%s", line->string);
    }
    return conf;
}

void voy_server_conf_add_option(voy_server_conf_t *conf, voy_str_t *op_name, voy_str_t *op_value)
{
    if (!conf) {
        return;
    }

    if (voy_str_equals(op_name, VOY_NAME_OPTION)) {
        conf->names = voy_parse_name_options(op_value);
    } else if (voy_str_equals(op_name, VOY_ROOT_OPTION)) {
        conf->root = voy_str_new(op_value->string);
    } else if (voy_str_equals(op_name, VOY_PORT_OPTION)) {
        conf->ports = voy_parse_port_options(op_value);
    } else if (voy_str_equals(op_name, VOY_INDEX_OPTION)) {
        // todo
    } else if (voy_str_equals(op_name, VOY_ERROR_LOG_OPTION)) {
        conf->error_log = voy_str_new(op_value->string);
    } else if (voy_str_equals(op_name, VOY_ACCESS_LOG_OPTION)) {
        conf->access_log = voy_str_new(op_value->string);
    }

    if (voy_str_contains(op_name, VOY_ERROR_PAGE_OPTION)) {
        // todo
    }
}

void voy_conf_add_option(voy_conf_t *conf, voy_str_t *op_name, voy_str_t *op_value)
{
    if (!conf) {
        return;
    }
    if (conf->default_server == NULL) {
        conf->default_server = voy_server_conf_new();
    }

    if (voy_str_equals(op_name, VOY_USER_OPTION)) {
        conf->user = voy_str_new(op_value->string);
    } else if (voy_str_equals(op_name, VOY_GROUP_OPTION)) {
        conf->group = voy_str_new(op_value->string);
    } else {
        voy_server_conf_add_option(conf->default_server, op_name, op_value);
    }
}

voy_server_conf_t *voy_server_conf_new()
{
    voy_server_conf_t *conf = malloc(sizeof(voy_server_conf_t));
    if (!conf) {
        // TODO: log this
        return NULL;
    }

    conf->root        = NULL;
    conf->names       = NULL;
    conf->ports       = NULL;
    conf->index_pages = NULL;
    conf->error_pages = NULL;
    conf->error_log   = NULL;
    conf->access_log  = NULL;

    return conf;
}

void voy_conf_add_vhost(voy_conf_t *conf, voy_server_conf_t *vhost)
{
    if (!conf || !vhost) {
        return;
    }
    if (conf->vhosts == NULL) {
        conf->vhosts = voy_array_new(10, sizeof(voy_server_conf_t*));
    }

    voy_array_push(conf->vhosts, vhost);
}

voy_array_t *voy_parse_name_options(voy_str_t *names)
{
    voy_array_t *found_names = voy_str_split_by_char(names, ',');
    if (!found_names) {
        return NULL;
    }

    voy_array_t *server_names = voy_array_new(10, sizeof(voy_str_t*));
    if (!server_names) {
        voy_array_free(found_names, voy_array_strs_free_cb);
        return NULL;
    }

    VOY_ARRAY_FOREACH(found_names) {
        bool name_found = false;
        voy_str_t *cur_name = voy_array_get(found_names, i);

        // make sure that there are no duplicates
        for (unsigned int j = 0; j < server_names->len; j++) {
            voy_str_t *stored_name = voy_array_get(server_names, j);

            if (voy_str_equals_voy_str(cur_name, stored_name)) {
                name_found = true;
                break;
            }
        }

        if (!name_found) {
            voy_str_t *new_name = voy_str_new(cur_name->string);
            if (new_name) {
                voy_str_trim(new_name);
                voy_array_push(server_names, new_name);
            }
        }
    }

    voy_array_free(found_names, voy_array_strs_free_cb);

    return server_names;
}

voy_array_t *voy_parse_port_options(voy_str_t *ports)
{
    voy_array_t *found_ports = voy_str_split_by_char(ports, ',');
    if (!found_ports) {
        return NULL;
    }

    voy_array_t *server_ports = voy_array_new(10, sizeof(int*));
    if (!server_ports) {
        voy_array_free(found_ports, voy_array_strs_free_cb);
        return NULL;
    }

    VOY_ARRAY_FOREACH(found_ports) {
        bool port_found = false;
        voy_str_t *cur_port = voy_array_get(found_ports, i);
        voy_str_trim(cur_port);
        int port_int = atoi(cur_port->string);

        // make sure that there are no duplicates
        for (unsigned int j = 0; j < server_ports->len; j++) {
            int *stored_port = voy_array_get(server_ports, j);
            if (*stored_port == port_int) {
                port_found = true;
                break;
            }
        }

        if (!port_found) {
            int *new_port = malloc(sizeof(int));
            if (new_port) {
                memset(new_port, 0, sizeof(int));
                *new_port = port_int;
                voy_array_push(server_ports, new_port);
            }
        }
    }

    voy_array_free(found_ports, voy_array_strs_free_cb);

    return server_ports;
}

void voy_array_strs_free_cb(void *value)
{
    if (value) {
        voy_str_free((voy_str_t*)value);
    }
}
