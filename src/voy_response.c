#include "voy_response.h"

static voy_http_status_code_t http_status_codes[] = {
    {
        100,
        "Continue",
    },
    {
        101,
        "Switching Protocols",
    },
    {
        102,
        "Processing",
    },
    {
        200,
        "OK",
    },
    {
        201,
        "Created",
    },
    {
        202,
        "Accepted",
    },
    {
        203,
        "Non-authoritative Information",
    },
    {
        204,
        "No Content",
    },
    {
        205,
        "Reset Content",
    },
    {
        206,
        "Partial Content",
    },
    {
        207,
        "Multi-Status",
    },
    {
        208,
        "Already Reported",
    },
    {
        226,
        "IM Used",
    },
    {
        300,
        "Multiple Choices",
    },
    {
        301,
        "Moved Permanently",
    },
    {
        302,
        "Found",
    },
    {
        303,
        "See Other",
    },
    {
        304,
        "Not Modified",
    },
    {
        305,
        "Use Proxy",
    },
    {
        307,
        "Temporary Redirect",
    },
    {
        308,
        "Permanent Redirect",
    },
    {
        400,
        "Bad Request",
    },
    {
        401,
        "Unauthorized",
    },
    {
        402,
        "Payment Required",
    },
    {
        403,
        "Forbidden",
    },
    {
        404,
        "Not Found",
    },
    {
        405,
        "Method Not Allowed",
    },
    {
        406,
        "Not Acceptable",
    },
    {
        407,
        "Proxy Authentication Required",
    },
    {
        408,
        "Request Timeout",
    },
    {
        409,
        "Conflict",
    },
    {
        410,
        "Gone",
    },
    {
        411,
        "Length Required",
    },
    {
        412,
        "Precondition Failed",
    },
    {
        413,
        "Payload Too Large",
    },
    {
        414,
        "Request-URI Too Long",
    },
    {
        415,
        "Unsupported Media Type",
    },
    {
        416,
        "Request Range Not Satisfiable",
    },
    {
        417,
        "Expectation Failed",
    },
    {
        418,
        "I'm a teapot",
    },
    {
        421,
        "Misdirected Request",
    },
    {
        422,
        "Unprocessable Entity",
    },
    {
        423,
        "Locked",
    },
    {
        424,
        "Failed Dependency",
    },
    {
        426,
        "Upgrade Required",
    },
    {
        428,
        "Precondition Required",
    },
    {
        429,
        "Too Many Requests",
    },
    {
        431,
        "Request Header Fields Too Large",
    },
    {
        444,
        "Connection Closed Without Response",
    },
    {
        451,
        "Unavailable For Legal Reasons",
    },
    {
        499,
        "Client Closed Request",
    },
    {
        500,
        "Internal Server Error",
    },
    {
        501,
        "Not Implemented",
    },
    {
        502,
        "Bad Gateway",
    },
    {
        503,
        "Service Unavailable",
    },
    {
        504,
        "Gateway Timeout",
    },
    {
        505,
        "HTTP Version Not Supported",
    },
    {
        506,
        "Variant Also Negotiates",
    },
    {
        507,
        "Insufficient Storage",
    },
    {
        508,
        "Loop Detected",
    },
    {
        510,
        "Not Extended",
    },
    {
        511,
        "Network Authentication Required",
    },
    {
        599,
        "Network Connect Timeout Error",
    },
};

static char *raw_headers   = NULL;
static int raw_headers_len = 0;
static int raw_headers_i   = 0;

static int voy_htable_compare_func(void *a, void *b)
{
    return strcmp((char *)a, (char *)b);
}

void voy_response_set_status_code(voy_response_t *res, int code)
{
    if (!res) {
        return;
    }
    res->status_code = code;
}

void voy_response_set_body(voy_response_t *res, char *body)
{
    if (!res) {
        return;
    }
    res->body = body;
}

void voy_response_set_body_len(voy_response_t *res, size_t body_len)
{
    if (!res) {
        return;
    }
    res->body_len = body_len;
}

char *voy_response_get_body(voy_response_t *res)
{
    if (!res) {
        return NULL;
    }
    return res->body;
}

char *voy_response_get_start_line(voy_response_t *res)
{
    int status_str_len                         = 0;
    voy_http_status_code_t *http_status_code_p = http_status_codes;

    if (!res) {
        return NULL;
    }

    while (true) {
        if (http_status_code_p->code == res->status_code) {
            status_str_len += (16 + strlen(http_status_code_p->description));
            break;
        }
        http_status_code_p++;
    }

    char *http_status = malloc(status_str_len);
    if (!http_status) {
        return NULL;
    }
    memset(http_status, 0, status_str_len);
    sprintf(http_status, "HTTP/%1.1f %d %s\r\n", HTTP_VERSION, http_status_code_p->code,
            http_status_code_p->description);

    res->status_message = http_status;

    return res->status_message;
}

void voy_response_set_header(voy_response_t *res, char *key, char *value)
{
    if (!res) {
        return;
    }

    if (!res->headers) {
        return;
    }
    voy_htable_set(res->headers, key, value);
}

void voy_set_raw_headers_str_len(void *k, void *v)
{
    if (k && v) {
        raw_headers_len += (strlen((char *)k) + strlen((char *)v)) + 4;
    }
}

void voy_set_raw_headers_str(void *k, void *v)
{
    char *raw_headers_p = (raw_headers + raw_headers_i);

    if (k && v) {
        char *key = k;
        char *val = v;
        for (; *key != '\0'; key++) {
            *raw_headers_p = *key;
            raw_headers_p++;
            raw_headers_i++;
        }

        *raw_headers_p = ':';
        raw_headers_p++;
        raw_headers_i++;
        *raw_headers_p = ' ';
        raw_headers_p++;
        raw_headers_i++;

        for (; *val != '\0'; val++) {
            *raw_headers_p = *val;
            raw_headers_p++;
            raw_headers_i++;
        }
        *raw_headers_p = '\r';
        raw_headers_p++;
        raw_headers_i++;
        *raw_headers_p = '\n';
        raw_headers_p++;
        raw_headers_i++;
    }
}

char *voy_response_get_headers(voy_response_t *res)
{
    if (!res) {
        return NULL;
    }

    voy_htable_traverse(res->headers, voy_set_raw_headers_str_len);

    // add the final newline and terminator
    raw_headers_len += 3;

    raw_headers = malloc(raw_headers_len);
    if (!raw_headers) {
        return NULL;
    }

    voy_htable_traverse(res->headers, voy_set_raw_headers_str);
    raw_headers[raw_headers_i++] = '\r';
    raw_headers[raw_headers_i++] = '\n';
    raw_headers[raw_headers_i]   = '\0';
    res->raw_headers             = raw_headers;

    return res->raw_headers;
}

voy_response_t *voy_response_new(void)
{
    voy_response_t *res = malloc(sizeof(voy_response_t));
    if (!res) {
        return NULL;
    }

    voy_htable_t *headers = voy_htable_new(voy_htable_compare_func);
    if (!headers) {
        return NULL;
    }

    res->status_code    = 200;
    res->status_message = NULL;
    res->raw_headers    = NULL;
    res->headers        = headers;
    res->body           = NULL;
    res->body_len       = 0;

    return res;
}

void voy_response_free(voy_response_t *res)
{
    if (!res) {
        return;
    }

    if (res->headers) {
        VOY_HTABLE_FOREACH (res->headers, elem, i, j) {
            free(elem->value);
        }
        VOY_HTABLE_FOREACH_END
        voy_htable_free(res->headers, NULL);
    }

    if (res->raw_headers) {
        free(res->raw_headers);
    }

    if (res->status_message) {
        free(res->status_message);
    }

    if (res->body) {
        free(res->body);
    }

    raw_headers_len = 0;
    raw_headers_i   = 0;
    raw_headers     = NULL;

    free(res);
}
