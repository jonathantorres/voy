#ifndef _voy_response_h
#define _voy_response_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "voy_htable.h"

#define HTTP_VERSION 1.1

typedef struct voy_http_status_code {
    int code;
    char *description;
} voy_http_status_code_t;

typedef struct {
    int status_code;
    char *status_message;
    voy_htable_t *headers;
    char *raw_headers;
    char *body;
    int body_len;
} voy_response_t;

voy_response_t *voy_response_new(void);
void voy_response_set_status_code(voy_response_t *res, int code);
char *voy_response_get_start_line(voy_response_t *res);
void voy_response_set_header(voy_response_t *res, char *key, char *value);
char *voy_response_get_headers(voy_response_t *res);
void voy_response_set_body(voy_response_t *res, char *body);
void voy_response_set_body_len(voy_response_t *res, size_t body_len);
char *voy_response_get_body(voy_response_t *res);
void voy_response_free(voy_response_t *res);

#endif
