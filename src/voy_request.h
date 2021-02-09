#ifndef _voy_request_h
#define _voy_request_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/socket.h>
#include "voy_array.h"
#include "voy_htable.h"

#define VOY_REQUEST_METHOD_GET "GET"
#define VOY_REQUEST_METHOD_HEAD "HEAD"
#define VOY_REQUEST_METHOD_POST "POST"
#define VOY_REQUEST_METHOD_PUT "PUT"
#define VOY_REQUEST_METHOD_DELETE "DELETE"
#define VOY_REQUEST_METHOD_OPTIONS "OPTIONS"
#define VOY_REQUEST_METHOD_TRACE "TRACE"
#define VOY_REQUEST_METHOD_PATCH "PATCH"

typedef struct {
    char *method;
    char *uri;
    voy_htable_t *headers;
    voy_htable_t *uri_params;
    char *body;
    bool done_reading;
    bool line_is_read;
    bool headers_are_read;
    bool body_is_read;
    int total_body_bytes_read;
} voy_request_t;

voy_request_t *voy_request_new();
void voy_request_free(voy_request_t *req);
bool voy_request_parse_start_line(voy_request_t *req, char *buffer);
bool voy_request_parse_headers(voy_request_t *req, char *buffer);
bool voy_request_parse_body(voy_request_t *req, char *buffer);
void voy_request_get_method(); // GET or POST
void voy_request_get_uri(); // / or /some or /some/another
void voy_request_get_query(); // get's the foo=bar&bar=baz in a data structure
void voy_request_get_headers();
void voy_request_get_header(); // provide the header name?
void voy_request_set_header();
void voy_request_get_body(); // a string with the body

#endif
