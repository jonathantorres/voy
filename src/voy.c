#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "voy_server.h"
#include "voy_conf.h"

#define VOY_VERSION "0.1.0"
#define DEFAULT_PREFIX "/usr/local/voy"
#define DEFAULT_CONF_FILE DEFAULT_PREFIX "/conf/voy.conf"
#define DEFAULT_LOG_FILE DEFAULT_PREFIX "/log/voy.log"

typedef enum {
    VOY_ARG_NONE,
    VOY_ARG_VERSION,
    VOY_ARG_OK,
    VOY_ARG_ERROR,
} voy_args_status_t;

void voy_print_usage();
void voy_print_version();
voy_args_status_t voy_parse_args(int argc, char **argv);

static char *log_file_path = NULL;
static char *conf_file_path = NULL;
static char *args_err_msg = NULL;
static char *args_err_unknown_arg = NULL;

int main(int argc, char **argv)
{
    voy_args_status_t args_status = voy_parse_args(argc, argv);
    switch (args_status) {
        case VOY_ARG_NONE:
            voy_print_usage();
            break;
        case VOY_ARG_VERSION:
            voy_print_version();
            break;
        case VOY_ARG_ERROR:
            if (args_err_msg == NULL) {
                args_err_msg = "error: unknown error";
            }
            fprintf(stderr, "%s", args_err_msg);
            if (args_err_unknown_arg) {
                fprintf(stderr, " %s", args_err_unknown_arg);
            }
            fprintf(stderr, "\n");
            voy_print_usage();
            break;
        case VOY_ARG_OK:
            // all good, nothing to do
            break;
    }

    // TODO: initialize logging mechanism

    voy_conf_t *conf = voy_conf_load(conf_file_path);
    if (!conf) {
        // handle error here and exit
    }
    if (conf && conf->default_server) {
        if (conf->default_server->ports) {
            VOY_ARRAY_FOREACH(conf->default_server->ports) {
                int *n = voy_array_get(conf->default_server->ports, i);
                printf("found port: %d\n", *n);
            }
        }
        if (conf->default_server->index_pages) {
            VOY_ARRAY_FOREACH(conf->default_server->index_pages) {
                voy_str_t *i_page = voy_array_get(conf->default_server->index_pages, i);
                printf("found index page: %s\n", i_page->string);
            }
        }
        if (conf->default_server->error_pages) {
            VOY_ARRAY_FOREACH(conf->default_server->error_pages) {
                voy_error_page_t *err_page = voy_array_get(conf->default_server->error_pages, i);
                printf("found error page (code): %d\n", *(err_page->code));
                printf("found error page (path): %s\n", err_page->page->string);
            }
        }
    }

    if (conf && conf->vhosts) {
        VOY_ARRAY_FOREACH(conf->vhosts) {
            voy_server_conf_t *cur_vhost = voy_array_get(conf->vhosts, i);

            if (cur_vhost) {
                if (cur_vhost->ports) {
                    for (unsigned int j = 0; j < cur_vhost->ports->len; j++) {
                        int *vhost_port = voy_array_get(cur_vhost->ports, j);
                        if (vhost_port) {
                            printf("vhost port: %d\n", *vhost_port);
                        }
                    }
                }
                if (cur_vhost->index_pages) {
                    for (unsigned int j = 0; j < cur_vhost->index_pages->len; j++) {
                        voy_str_t *vhost_i_page = voy_array_get(cur_vhost->index_pages, j);
                        if (vhost_i_page) {
                            printf("vhost index page: %s\n", vhost_i_page->string);
                        }
                    }
                }
                if (cur_vhost->error_pages) {
                    VOY_ARRAY_FOREACH(cur_vhost->error_pages) {
                        voy_error_page_t *err_page = voy_array_get(cur_vhost->error_pages, i);
                        printf("vhost error page (code): %d\n", *(err_page->code));
                        printf("vhost error page (path): %s\n", err_page->page->string);
                    }
                }
            }
        }
    }

    exit(0); // remove this!
    bool status = voy_server_start(conf);
    if (!status) {
        // handle error here and exit
        exit(1);
    }

    if (log_file_path) {
        printf("log file path = %s\n", log_file_path);
    }

    if (conf_file_path) {
        printf("conf file path = %s\n", conf_file_path);
    }
    return 0;
}

voy_args_status_t voy_parse_args(int argc, char **argv)
{
    if (argc <= 1) {
        return VOY_ARG_NONE;
    }

    for (int i = 1; i < argc; i++) {
        char *cur_arg = argv[i];
        if (strcmp(cur_arg, "-v") == 0 || strcmp(cur_arg, "--version") == 0) {
            return VOY_ARG_VERSION;
        } else if (strcmp(cur_arg, "-c") == 0 || strcmp(cur_arg, "--config") == 0) {
            i++;
            if (argv[i] == NULL) {
                args_err_msg = "error: configuration file must be specified";
                return VOY_ARG_ERROR;
            }
            conf_file_path = argv[i];
        } else if (strcmp(cur_arg, "-l") == 0 || strcmp(cur_arg, "--log") == 0) {
            i++;
            if (argv[i] == NULL) {
                args_err_msg = "error: path to the log file must be specified";
                return VOY_ARG_ERROR;
            }
            log_file_path = argv[i];
        } else {
            args_err_msg = "error: unknown option";
            args_err_unknown_arg = cur_arg;
            return VOY_ARG_ERROR;
        }
    }

    return VOY_ARG_OK;
}

void voy_print_version()
{
    fprintf(stderr, "voy version v%s\n", VOY_VERSION);
    exit(EXIT_SUCCESS);
}

void voy_print_usage()
{
    fprintf(stderr, "voy [-v] [--version]\n");
    fprintf(stderr, "    [-c <file> | --config <file>]\n");
    fprintf(stderr, "    [-l <path> | --log <path>]\n");
    fprintf(stderr, "\n-v --version\tPrint version and exit");
    fprintf(stderr, "\n-c <file>\tUse the specified configuration file (default is %s)", DEFAULT_CONF_FILE);
    fprintf(stderr, "\n-l <path>\tStore logs in this location (default is %s)", DEFAULT_LOG_FILE);
    fprintf(stderr, "\n");
    exit(EXIT_SUCCESS);
}
