#include "voy_file.h"

typedef struct {
    char *extension;
    char *file_name;
    char *static_contents;
} voy_file_t;

typedef struct {
    char *extension;
    char *type_str;
    int file_type;
} voy_content_type_t;

#define VOY_FILE_EXT_LEN 10
#define VOY_FILE_TYPE_BINARY 0
#define VOY_FILE_TYPE_TEXT 1
#define VOY_CONTENT_TYPES_LEN 28

static voy_content_type_t content_types[VOY_CONTENT_TYPES_LEN] = {
    { "html", "text/html", VOY_FILE_TYPE_TEXT },
    { "htm", "text/html", VOY_FILE_TYPE_TEXT },
    { "css", "text/css", VOY_FILE_TYPE_TEXT },
    { "md", "text/markdown", VOY_FILE_TYPE_TEXT },
    { "txt", "text/plain", VOY_FILE_TYPE_TEXT },
    { "xml", "text/xml", VOY_FILE_TYPE_TEXT },
    { "js", "application/javascript", VOY_FILE_TYPE_TEXT },
    { "json", "application/json", VOY_FILE_TYPE_TEXT },
    { "pdf", "application/pdf", VOY_FILE_TYPE_BINARY },
    { "zip", "application/zip", VOY_FILE_TYPE_BINARY },
    { "bmp", "image/bmp", VOY_FILE_TYPE_BINARY },
    { "gif", "image/gif", VOY_FILE_TYPE_BINARY },
    { "jpg", "image/jpeg", VOY_FILE_TYPE_BINARY },
    { "jpeg", "image/jpeg", VOY_FILE_TYPE_BINARY },
    { "ico", "image/x-icon", VOY_FILE_TYPE_BINARY },
    { "png", "image/png", VOY_FILE_TYPE_BINARY },
    { "tiff", "image/tiff", VOY_FILE_TYPE_BINARY },
    { "svg", "image/svg", VOY_FILE_TYPE_TEXT },
    { "mp4", "audio/mp4", VOY_FILE_TYPE_BINARY },
    { "mp4", "video/mp4", VOY_FILE_TYPE_BINARY },
    { "mpeg", "audio/mpeg", VOY_FILE_TYPE_BINARY },
    { "mpeg", "video/mpeg", VOY_FILE_TYPE_BINARY },
    { "ogg", "audio/ogg", VOY_FILE_TYPE_BINARY },
    { "ogg", "video/ogg", VOY_FILE_TYPE_BINARY },
    { "quicktime", "video/quicktime", VOY_FILE_TYPE_BINARY },
    { "ttf", "font/ttf", VOY_FILE_TYPE_BINARY },
    { "woff", "font/woff", VOY_FILE_TYPE_BINARY },
    { "woff2", "font/woff2", VOY_FILE_TYPE_BINARY },
};

size_t _voy_get_binary_file_contents_len(int fd)
{
    size_t len = (size_t)lseek(fd, (off_t)0, SEEK_END);
    lseek(fd, (off_t)0, SEEK_SET);
    return len;
}

size_t _voy_get_text_file_contents_len(FILE *fp)
{
    size_t len = 0;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        len++;
    }
    return len;
}

char *_voy_set_binary_file_contents(int fd, size_t contents_len)
{
    char *contents = NULL;
    long bytes_r;
    contents = malloc(contents_len);
    if (!contents) {
        return NULL;
    }
    memset(contents, 0, contents_len);
    bytes_r = read(fd, contents, contents_len);

    if (bytes_r < 0) {
        return NULL;
    }
    return contents;
}

char *_voy_set_text_file_contents(FILE *fp, size_t contents_len)
{
    char *contents = NULL;
    char *contents_p = NULL;

    contents = malloc(contents_len + 1);
    if (!contents) {
        return NULL;
    }
    memset(contents, 0, contents_len + 1);
    contents_p = contents;
    rewind(fp);
    int c;
    while ((c = fgetc(fp)) != EOF) {
        *contents_p = c;
        contents_p++;
    }
    *contents_p = '\0';
    return contents;
}

char *_voy_get_content_len_str(size_t contents_len)
{
    int content_len_str_len = 12;
    char *content_len_str = malloc(content_len_str_len);
    if (!content_len_str) {
        return NULL;
    }
    memset(content_len_str, 0, content_len_str_len);
    sprintf(content_len_str, "%ld", contents_len);
    return content_len_str;
}

char *_voy_get_file_path(voy_request_t *req)
{
    char *uri = NULL;
    char *uri_p = NULL;
    char *req_uri_p = NULL;
    int uri_len = strlen(req->uri);
    uri = malloc(uri_len);
    if (!uri) {
        return NULL;
    }
    memset(uri, 0, uri_len);
    uri_p = uri;
    req_uri_p = req->uri;

    bool slash_found = false;
    while (*req_uri_p != '\0') {
        if (*req_uri_p == '/' && slash_found == false) {
            slash_found = true;
            req_uri_p++;
            continue;
        }
        *uri_p++ = *req_uri_p++;
    }

    // requested /, maybe try an index file?
    if (strcmp(uri, "") == 0) {
        free(uri);
        return NULL;
    }

    int file_to_serve_len = strlen(uri);
    char *file_to_serve = malloc(file_to_serve_len);
    if (!file_to_serve) {
        free(uri);
        return NULL;
    }
    memset(file_to_serve, 0, file_to_serve_len);
    file_to_serve[0] = '\0';
    strcat(file_to_serve, uri);
    free(uri);
    return file_to_serve;
}

char *_voy_get_file_ext(char *uri)
{
    size_t uri_len = strlen(uri);
    size_t dot_loc = 0;
    char *uri_p = uri;
    char *file_ext = malloc(VOY_FILE_EXT_LEN);

    if (!file_ext) {
        return NULL;
    }
    memset(file_ext, 0, VOY_FILE_EXT_LEN);
    while (*uri_p != '\0') {
        if (*(uri_p - 1) == '.') {
            break;
        }
        dot_loc++;
        uri_p++;
    }
    // we couldn't find the file extension, use txt by default
    if (dot_loc == 0) {
        free(file_ext);
        return strdup("txt");
    }
    memcpy(file_ext, uri_p, uri_len - dot_loc);
    file_ext[(uri_len - dot_loc) + 1] = '\0';

    return file_ext;
}

voy_content_type_t *_voy_get_content_type_struct(char *file_ext)
{
    voy_content_type_t *found_content_type = NULL;

    for (int i = 0; i < VOY_CONTENT_TYPES_LEN; i++) {
        if (strcmp(file_ext, content_types[i].extension) == 0) {
            found_content_type = &content_types[i];
        }
    }
    return found_content_type;
}

void _voy_serve_file(voy_request_t *req, voy_response_t *res, char *contents, size_t contents_len)
{
    char *file_ext = _voy_get_file_ext(req->uri);
    voy_content_type_t *cur_content_type = _voy_get_content_type_struct(file_ext);
    char *content_len_str = _voy_get_content_len_str(contents_len);

    voy_response_set_status_code(res, 200);
    voy_response_set_header(res, "Content-Length", strdup(content_len_str));
    voy_response_set_header(res, "Content-Type", strdup(cur_content_type->type_str));
    voy_response_set_body(res, contents);
    voy_response_set_body_len(res, contents_len);

    free(file_ext);
    free(content_len_str);
}

bool voy_file_serve(voy_request_t *req, voy_response_t *res)
{
    char *file_to_serve = _voy_get_file_path(req);
    if (!file_to_serve) {
        return false;
    }

    char *file_ext = _voy_get_file_ext(req->uri);
    voy_content_type_t *cur_content_type = _voy_get_content_type_struct(file_ext);
    char *contents = NULL;
    size_t contents_len;

    if (cur_content_type != NULL && cur_content_type->file_type == VOY_FILE_TYPE_BINARY) {
        int fd = open(file_to_serve, O_RDONLY);
        if (fd == -1) {
            perror("file opening failed");
            return false;
        }
        contents_len = _voy_get_binary_file_contents_len(fd);
        if (contents_len == 0) {
            return false;
        }
        contents = _voy_set_binary_file_contents(fd, contents_len);
        if (!contents) {
            return false;
        }
    } else if (cur_content_type != NULL && cur_content_type->file_type == VOY_FILE_TYPE_TEXT) {
        // it's a text file
        FILE *fp = fopen(file_to_serve, "r");
        if (!fp) {
            perror("file opening failed");
            printf("%s\n", file_to_serve);
            return false;
        }
        contents_len = _voy_get_text_file_contents_len(fp);
        if (contents_len == 0) {
            fclose(fp);
            return false;
        }
        contents = _voy_set_text_file_contents(fp, contents_len);
        if (!contents) {
            fclose(fp);
            return false;
        }
        fclose(fp);
    } else {
        return false;
    }

    _voy_serve_file(req, res, contents, contents_len);

    free(file_ext);
    free(file_to_serve);
    return true;
}
