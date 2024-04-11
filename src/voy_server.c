#include "voy_server.h"

static void *voy_new_request_buffer()
{
    char *recv_buff = malloc(VOY_RECV_BUFF_LEN);
    if (!recv_buff) {
        // TODO: log this
        return NULL;
    }
    memset(recv_buff, 0, VOY_RECV_BUFF_LEN);

    return recv_buff;
}

static int voy_bind_and_listen(int port)
{
    int server_fd;
    struct sockaddr_in server_addr;

    // TODO: Use getaddrinfo() to get information
    // on the ip addresses and whatnot

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        // TODO: log this
        perror("server: socket()");
        return -1;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // re-use the address when stopping and restarting the server
    int reuse_addr = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(int)) < 0) {
        // TODO: log this
        perror("server: setsockopt()");
        close(server_fd);
        return -1;
    }

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0) {
        // TODO: log this
        perror("server: bind()");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        // TODO: log this
        perror("server: listen()");
        close(server_fd);
        return -1;
    }

    return server_fd;
}

static void voy_set_default_response_headers(voy_response_t *res)
{
    voy_response_set_header(res, "Server", voy_str_dup("voy v0.0.1"));
    voy_response_set_header(res, "Connection", voy_str_dup("close"));
}

static void voy_serve_404(voy_response_t *res)
{
    char *not_found_msg       = "Not found\n";
    char not_found_len_str[3] = {0, 0, 0};
    snprintf(not_found_len_str, 3, "%d", (int)strlen(not_found_msg));
    voy_response_set_status_code(res, 404);
    voy_response_set_body(res, voy_str_dup(not_found_msg));
    voy_response_set_body_len(res, strlen(not_found_msg));
    voy_response_set_header(res, "Content-Length", voy_str_dup(not_found_len_str));
    voy_response_set_header(res, "Content-Type", voy_str_dup("text/html"));
}

static void voy_handle_conn(int conn_fd)
{
    voy_request_t *req = voy_request_new();
    bool status        = true;

    if (!req) {
        // TODO: log this
        close(conn_fd);
        return;
    }

    while (true) {
        if (req->done_reading) {
            break;
        }

        char *req_buff = voy_new_request_buffer();
        if (!req_buff) {
            // TODO: log this
            status = false;
            close(conn_fd);
            break;
        }

        int bytes_r = recv(conn_fd, req_buff, VOY_RECV_BUFF_LEN, 0);
        if (bytes_r < 0) {
            // TODO: log this
            status = false;
            close(conn_fd);
            perror("Error: recv()");
            free(req_buff);
            break;
        }
        req_buff[bytes_r - 1] = '\0';

        if (!req->line_is_read) {
            status = voy_request_parse_start_line(req, req_buff);
            if (!status) {
                // TODO: log this
                free(req_buff);
                close(conn_fd);
                break;
            }
        }

        if (!req->headers_are_read) {
            status = voy_request_parse_headers(req, req_buff);
            if (!status) {
                // TODO: log this
                free(req_buff);
                close(conn_fd);
                break;
            }
        }

        if (!req->body_is_read) {
            status = voy_request_parse_body(req, req_buff);
            if (!status) {
                // TODO: log this
                free(req_buff);
                close(conn_fd);
                break;
            }
        }
    }

    voy_response_t *res = voy_response_new();
    if (!res) {
        // TODO: log this
        voy_request_free(req);
        close(conn_fd);
        return;
    }

    bool file_found = false;
    file_found      = voy_file_serve(req, res);

    if (!file_found) {
        voy_serve_404(res);
    }
    voy_set_default_response_headers(res);

    char *http_status_line = voy_response_get_start_line(res);
    char *response_headers = voy_response_get_headers(res);
    char *response_body    = voy_response_get_body(res);

    if (send(conn_fd, http_status_line, strlen(http_status_line), 0) < 0) {
        perror("server: send() http status line");
        // TODO: log this
        voy_request_free(req);
        voy_response_free(res);
        close(conn_fd);
        return;
    }
    if (send(conn_fd, response_headers, strlen(response_headers), 0) < 0) {
        perror("server: send() http response headers");
        // TODO: log this
        voy_request_free(req);
        voy_response_free(res);
        close(conn_fd);
        return;
    }
    if (send(conn_fd, response_body, res->body_len, 0) < 0) {
        perror("server: send() http response body");
        // TODO: log this
        voy_request_free(req);
        voy_response_free(res);
        close(conn_fd);
        return;
    }
    close(conn_fd);
    voy_request_free(req);
    voy_response_free(res);
}

voy_array_t *get_listen_ports(voy_conf_t *conf)
{
    if (!conf) {
        // TODO: log this error
        return NULL;
    }
    voy_array_t *ports = voy_array_new(10, sizeof(int *));

    if (!ports) {
        return NULL;
    }

    // ports from the default server
    if (conf->default_server && conf->default_server->ports) {
        VOY_ARRAY_FOREACH (conf->default_server->ports, i) {
            int *p = voy_array_get(conf->default_server->ports, i);
            if (p) {
                voy_array_push(ports, p);
            }
        }
    }

    // ports from virtual hosts
    if (conf->vhosts) {
        VOY_ARRAY_FOREACH (conf->vhosts, i) {
            voy_server_conf_t *vhost = voy_array_get(conf->vhosts, i);
            if (vhost && vhost->ports) {
                VOY_ARRAY_FOREACH (vhost->ports, i) {
                    int *p       = voy_array_get(vhost->ports, i);
                    bool p_found = false;

                    // make sure that there are no duplicates
                    for (unsigned int j = 0; j < ports->len; j++) {
                        int *sp = voy_array_get(ports, j);
                        if (*sp == *p) {
                            p_found = true;
                            break;
                        }
                    }
                    if (p && !p_found) {
                        voy_array_push(ports, p);
                    }
                }
            }
        }
    }

    return ports;
}

bool serve_on_port(int port)
{
    int server_fd, conn_fd;
    if ((server_fd = voy_bind_and_listen(port)) < 0) {
        // TODO: log this
        return false;
    }
    printf("Server running on port %d\n", port);

    // TODO: initialize thread pool

    while (true) {
        if ((conn_fd = accept(server_fd, (struct sockaddr *)NULL, NULL)) < 0) {
            // TODO: log this
            perror("server: accept()");
            continue;
        }

        // TODO: handle this request in it's own thread
        // for now, we'll just fork
        pid_t pid = fork();
        if (pid > 0) {
            // parent process
            close(conn_fd);
        } else if (pid == 0) {
            // this is the child
            close(server_fd);
            voy_handle_conn(conn_fd);
            exit(0);
        } else {
            // something went wrong
            // the server won't be able to process the request
            perror("server: fork()");
            continue;
        }
    }
    close(server_fd);
    return true;
}

bool voy_server_start(voy_conf_t *conf)
{
    // get an array of ports to listen to from the conf file
    voy_array_t *ports = get_listen_ports(conf);

    if (!ports) {
        // TODO: log this error
        return false;
    }

    // TODO: launch a thread for each port to listen to
    // in this thread create a listener for that port

    // TODO: in each listener, when a request comes in,
    // figure out which virtual server to pick
    // and serve the request accordingly

    // TODO: wait for those threads to finish
    // probably never as long as the server
    // is running ok with no issues

    // For now, listen to the first port found that does not require
    // superuser priviledges, later on implement this
    // using the threads and whatnot
    int port = VOY_DEFAULT_PORT;
    VOY_ARRAY_FOREACH (ports, i) {
        int *p = voy_array_get(ports, i);
        if (p && *p > 1024) {
            port = *p;
            break;
        }
    }

    if (!serve_on_port(port)) {
        // TODO: log this error
        return false;
    }

    return true;
}

bool voy_server_shutdown()
{
    puts("shutting down the server");
    exit(EXIT_SUCCESS);
}
