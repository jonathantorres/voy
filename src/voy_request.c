#include "voy_request.h"

static int voy_htable_compare_fn(void *a, void *b)
{
    return strcmp((char *)a, (char *)b);
}

bool voy_request_parse_start_line(voy_request_t *req, char *buffer)
{
    char *buf;
    char *req_method, *req_method_p;
    char *req_uri, *req_uri_p;
    int num_of_spaces  = 0;
    int req_method_len = 24; // arbitrary
    int req_uri_len    = 0;

    req_method = malloc(req_method_len);
    memset(req_method, 0, req_method_len);
    req_method_p = req_method;

    for (buf = buffer; *buf != '\r'; buf++) {
        if (*buf == ' ') {
            num_of_spaces++;
        }
        // getting the request method
        if (num_of_spaces == 0) {
            memcpy(req_method_p, buf, 1);
            req_method_p++;
        }
        // get the length of the request uri
        if (num_of_spaces == 1) {
            req_uri_len++;
        }
    }
    *req_method_p = '\0';

    req_uri = malloc(req_uri_len);
    memset(req_uri, 0, req_uri_len);
    req_uri_p = req_uri;

    num_of_spaces = 0;
    for (buf = buffer; *buf != '\r'; buf++) {
        if (*buf == ' ') {
            num_of_spaces++;
        }
        // get the request uri
        if (num_of_spaces == 1 && *buf != ' ') {
            memcpy(req_uri_p, buf, 1);
            req_uri_p++;
        }
    }
    *req_uri_p = '\0';

    req->method       = req_method;
    req->uri          = req_uri;
    req->line_is_read = true;

    if (strcmp(req->method, "GET") == 0 || strcmp(req->method, "HEAD") == 0) {
        req->body_is_read = true;
    }
    return true;
}

bool voy_request_parse_headers(voy_request_t *req, char *buffer)
{
    if (req && !req->line_is_read) {
        return true;
    }

    char *buf;
    int i                     = 0;
    int num_of_newlines       = 0;
    voy_array_t *header_lines = voy_array_new(20, sizeof(char *));
    int line_length           = 256; // arbitrary
    char *cur_line            = malloc(line_length);

    if (!cur_line) {
        return false;
    }

    memset(cur_line, 0, line_length);
    voy_array_push(header_lines, cur_line);

    for (buf = buffer; *buf != '\0'; buf++, i++) {
        // we are in a header line
        if (num_of_newlines > 0) {
            if (*buf != '\n') {
                if (*buf != '\r') {
                    memcpy(cur_line, buf, 1);
                    cur_line++;
                }
            } else {
                *cur_line = '\0';
                // reached new line
                if (isalnum(*(buf + 1))) {
                    cur_line = malloc(line_length);
                    if (!cur_line) {
                        return false;
                    }
                    memset(cur_line, 0, line_length);
                    voy_array_push(header_lines, cur_line);
                }
            }

            // Once we reach the body, we break
            if (*buf == '\n' && *(buf + 1) == '\n') {
                break;
            }
        }

        if (*buf == '\n') {
            num_of_newlines++;
        }
    }

    for (unsigned int j = 0; j < header_lines->len; j++) {
        char *header_line = (char *)voy_array_get(header_lines, j);
        int key_len       = 0;
        int value_len     = 0;
        bool count_keys   = true;

        // get the lengths of the key and the value
        for (size_t k = 0; k < strlen(header_line); k++) {
            if (header_line[k] == ':') {
                count_keys = false;
            }
            if (count_keys && (header_line[k] != ':' || header_line[k] != ' ')) {
                key_len++;
            } else if (count_keys == false) {
                value_len++;
            }
        }

        char *key = malloc(key_len + 1);
        char *val = malloc(value_len + 1);

        if (!key) {
            return false;
        }
        if (!val) {
            return false;
        }
        memset(key, 0, key_len + 1);
        memset(val, 0, value_len + 1);

        bool found_colon = false;
        char *key_p      = key;
        char *val_p      = val;

        for (size_t l = 0; l < strlen(header_line); l++) {
            if (!found_colon && header_line[l] == ':') {
                found_colon = true;
            }

            if (!found_colon) {
                memcpy(key_p, &header_line[l], 1);
                key_p++;
            } else if (found_colon && l > (size_t)(key_len + 1)) {
                memcpy(val_p, &header_line[l], 1);
                val_p++;
            }
        }

        *key_p = '\0';
        *val_p = '\0';

        voy_htable_set(req->headers, key, val);
        free(key);
    }

    VOY_ARRAY_FOREACH (header_lines) {
        char *cur_l = voy_array_get(header_lines, i);
        if (cur_l) {
            free(cur_l);
        }
    }
    voy_array_free(header_lines, NULL);

    req->headers_are_read = true;

    if (strcmp(req->method, "GET") == 0 || strcmp(req->method, "HEAD") == 0) {
        req->headers_are_read = true;
        req->body_is_read     = true;
        req->done_reading     = true;
    }
    return true;
}

bool voy_request_parse_body(voy_request_t *req, char *buffer)
{
    if (req &&
        (!req->line_is_read || !req->headers_are_read || req->body_is_read || req->done_reading)) {
        return true;
    }

    size_t i;
    size_t body_start_i = 0;
    char *body          = NULL;

    for (i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == '\n' && buffer[i + 1] == '\r') {
            body_start_i = i + 3;
            break;
        }
    }

    // we have a body in the request
    if (body_start_i > 0) {
        char *content_len_str = voy_htable_get(req->headers, "Content-Length");

        if (content_len_str != NULL) {
            int content_len = atoi(content_len_str) + 1;
            body            = malloc(content_len);
            if (body != NULL) {
                memset(body, 0, content_len);
                memcpy(body, &buffer[body_start_i], content_len - 1);
                body[content_len - 1] = '\0';
                req->body             = body;
                req->body_is_read     = true;
                req->done_reading     = true;
            }
        }
    }
    return true;
}

voy_request_t *voy_request_new()
{
    voy_request_t *req = malloc(sizeof(voy_request_t));

    if (req == NULL) {
        return NULL;
    }

    req->method                = NULL;
    req->uri                   = NULL;
    req->body                  = NULL;
    req->headers               = NULL;
    req->uri_params            = NULL;
    req->done_reading          = false;
    req->line_is_read          = false;
    req->headers_are_read      = false;
    req->body_is_read          = false;
    req->total_body_bytes_read = 0;

    voy_htable_t *headers_p = voy_htable_new(voy_htable_compare_fn);
    if (headers_p == NULL) {
        free(req);
        return NULL;
    }

    voy_htable_t *uri_params_p = voy_htable_new(voy_htable_compare_fn);
    if (uri_params_p == NULL) {
        free(req);
        return NULL;
    }

    req->headers    = headers_p;
    req->uri_params = uri_params_p;

    return req;
}

void voy_request_free(voy_request_t *req)
{
    if (!req) {
        return;
    }

    if (req->headers) {
        VOY_HTABLE_FOREACH (req->headers) {
            free(elem->value);
        }
        VOY_HTABLE_FOREACH_END
        voy_htable_free(req->headers, NULL);
    }

    if (req->uri_params) {
        VOY_HTABLE_FOREACH (req->uri_params) {
            free(elem->value);
        }
        VOY_HTABLE_FOREACH_END
        voy_htable_free(req->uri_params, NULL);
    }

    if (req->method) {
        free(req->method);
    }

    if (req->uri) {
        free(req->uri);
    }

    if (req->body) {
        free(req->body);
    }
    free(req);
}
