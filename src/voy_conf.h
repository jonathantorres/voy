#ifndef _voy_conf_h
#define _voy_conf_h

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "voy_array.h"
#include "voy_str.h"

// This is supposed to validate, test and parse the configuration file

#define VOY_EQUAL_SIGN      '='
#define VOY_OPEN_BRACKET    '{'
#define VOY_CLOSING_BRACKET '}'
#define VOY_COMMENT_SIGN    '#'

#define VOY_NAME_OPTION       "name"
#define VOY_ROOT_OPTION       "root"
#define VOY_PORT_OPTION       "port"
#define VOY_USER_OPTION       "user"
#define VOY_GROUP_OPTION      "group"
#define VOY_INDEX_OPTION      "index"
#define VOY_ERROR_PAGE_OPTION "error_page"
#define VOY_ERROR_LOG_OPTION  "error_log"
#define VOY_ACCESS_LOG_OPTION "access_log"
#define VOY_INCLUDE_OPTION    "include"
#define VOY_VHOST_OPTION      "vhost"

typedef struct {
    voy_str_t   *root;
    voy_array_t *names; // array of strings
    voy_array_t *ports; // array of int's
    voy_array_t *index_pages; // array of strings
    voy_array_t *error_pages; // array of voy_error_page_t's
    voy_str_t   *error_log;
    voy_str_t   *access_log;
} voy_server_conf_t;

typedef struct {
    int       *code;
    voy_str_t *page;
} voy_error_page_t;

typedef struct {
    voy_str_t         *user;
    voy_str_t         *group;
    voy_server_conf_t *default_server;
    voy_array_t       *vhosts; // an array of voy_server_conf_t's
} voy_conf_t;

voy_conf_t *voy_conf_load(char *conf_file_path);
#endif
