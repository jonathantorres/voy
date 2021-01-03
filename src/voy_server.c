#include "voy_server.h"

void *voy_new_request_buffer(int conn_fd)
{
    int bytes_r = 0;
    char *recv_buff = NULL;

    recv_buff = malloc(VOY_RECV_BUFF_LEN);
    if (recv_buff == NULL) {
        fprintf(stderr, "no memory!\n");
        return NULL;
    }
    memset(recv_buff, 0, VOY_RECV_BUFF_LEN);

    bytes_r = recv(conn_fd, recv_buff, VOY_RECV_BUFF_LEN, 0);
    if (bytes_r < 0) {
        perror("Error: recv()");
        free(recv_buff);
        return NULL;
    }
    recv_buff[bytes_r-1] = '\0';

    return recv_buff;
}

int voy_bind_and_listen(int port)
{
    int server_fd;
    struct sockaddr_in server_addr;

    // TODO: Use getaddrinfo() to get information
    // on the ip addresses and whatnot

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("server: socket()");
        return -1;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // re-use the address when stopping and restarting the server
    int reuse_addr = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(int)) < 0) {
        perror("server: setsockopt()");
        close(server_fd);
        return -1;
    }

    if (bind(server_fd, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in)) < 0) {
        perror("server: bind()");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("server: listen()");
        close(server_fd);
        return -1;
    }

    return server_fd;
}

void voy_set_default_response_headers(voy_response_t *res)
{
    voy_response_set_header(res, "Server", voy_str_dup("voy v0.0.1"));
    voy_response_set_header(res, "Connection", voy_str_dup("close"));
}

void voy_serve_404(voy_response_t *res)
{
    char *not_found_msg = "Not found\n";
    char not_found_len_str[3] = {0, 0, 0};
    snprintf(not_found_len_str, 3, "%d", (int)strlen(not_found_msg));
    voy_response_set_status_code(res, 404);
    voy_response_set_body(res, voy_str_dup(not_found_msg));
    voy_response_set_body_len(res, strlen(not_found_msg));
    voy_response_set_header(res, "Content-Length", voy_str_dup(not_found_len_str));
    voy_response_set_header(res, "Content-Type", voy_str_dup("text/html"));
}

bool voy_server_start(void *conf)
{
    if (conf) {} // TODO
    int server_fd, conn_fd;
    int port = VOY_DEFAULT_PORT;

    voy_request_t *req = NULL;
    voy_response_t *res = NULL;
    if ((server_fd = voy_bind_and_listen(port)) < 0) {
        return false;
    }
    printf("Server running on port %d\n", port);

    while (true) {
        char *req_buff = NULL;
        if ((conn_fd = accept(server_fd, (struct sockaddr*) NULL, NULL)) < 0) {
            perror("server: accept()");
            continue;
        }

        if ((req_buff = voy_new_request_buffer(conn_fd)) == NULL) {
            close(conn_fd);
            continue;
        }

        // create the request object with the request buffer
        if ((req = voy_request_new(req_buff)) == NULL) {
            close(conn_fd);
            free(req_buff);
            continue;
        }
        free(req_buff);

        // initialize the response object
        if ((res = voy_response_new()) == NULL) {
            close(conn_fd);
            continue;
        }

        bool file_found = false;
        file_found = voy_file_serve(req, res);

        if (!file_found) {
            voy_serve_404(res);
        }
        voy_set_default_response_headers(res);

        char *http_status_line = voy_response_get_start_line(res);
        char *response_headers = voy_response_get_headers(res);
        char *response_body = voy_response_get_body(res);

        if (send(conn_fd, http_status_line, strlen(http_status_line), 0) < 0) {
            perror("server: send() http status line");
            close(conn_fd);
            continue;
        }
        if (send(conn_fd, response_headers, strlen(response_headers), 0) < 0) {
            perror("server: send() http response headers");
            close(conn_fd);
            continue;
        }
        if (send(conn_fd, response_body, res->body_len, 0) < 0) {
            perror("server: send() http response body");
            close(conn_fd);
            continue;
        }
        close(conn_fd);
        voy_request_free(req);
        voy_response_free(res);
    }
    close(server_fd);
    return true;
}
