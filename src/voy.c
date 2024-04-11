#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "voy_conf.h"
#include "voy_log.h"
#include "voy_server.h"

#define VOY_VERSION       "0.1.0"
#define DEFAULT_PREFIX    "/usr/local/voy"
#define DEFAULT_CONF_FILE DEFAULT_PREFIX "/conf/voy.conf"
#define DEFAULT_LOG_FILE  DEFAULT_PREFIX "/log/voy.log"

void debug_conf(voy_conf_t *conf);
static void voy_print_usage();
static void voy_print_version();
static void handle_signals();

static char *log_file_path  = NULL;
static char *conf_file_path = NULL;

int main(int argc, char **argv)
{
    int opt;

    while ((opt = getopt(argc, argv, "hvc:l:")) != -1) {
        switch (opt) {
            case 'v':
                voy_print_version();
                break;
            case 'h':
                voy_print_usage();
                break;
            case 'c':
                /* fprintf(stderr, "%s", "error: path to the log file must be specified\n"); */
                conf_file_path = optarg;
                break;
            case 'l':
                /* fprintf(stderr, "%s", "error: path to the log file must be specified\n"); */
                log_file_path = optarg;
                break;
            default:
                voy_print_usage();
                break;
        }
    }

    // TODO: initialize logging mechanism
    voy_log_init();

    // TODO: figure out which path to use for the configuration file
    // either from the -conf option, or configured from the build
    // the -conf option would override any location set in the build
    voy_conf_t *conf = voy_conf_load(conf_file_path);
    if (!conf) {
        // TODO: log the error
        exit(EXIT_FAILURE);
    }

    // just temporary
    // debug_conf(conf);

    // TODO: Handle signals that will tell the server to
    // reload the configuration files (HUP)
    // or to gracefully shutdown (TERM, INT, QUIT)
    handle_signals();

    bool status = voy_server_start(conf);
    if (!status) {
        // handle error here and exit
        voy_conf_free(conf);
        exit(EXIT_FAILURE);
    }

    voy_conf_free(conf);

    return 0;
}

void voy_wait_for_children()
{
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) {
        // do nothing for now
        // maybe we should log this
    }
    return;
}

static void sig_handler(int signum)
{
    switch (signum) {
        case SIGTERM:
        case SIGINT:
        case SIGQUIT:
            voy_server_shutdown();
            break;
        case SIGHUP:
            voy_conf_reload();
            break;
        case SIGCHLD:
            voy_wait_for_children();
            break;
    }
}

static void handle_signals()
{
    struct sigaction act;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;

    // server shutdown signals
    if (sigaction(SIGTERM, &act, NULL) == -1) {
        // TODO: log the error
    }
    if (sigaction(SIGINT, &act, NULL) == -1) {
        // TODO: log the error
    }
    if (sigaction(SIGQUIT, &act, NULL) == -1) {
        // TODO: log the error
    }

    // server restart signal
    if (sigaction(SIGHUP, &act, NULL) == -1) {
        // TODO: log the error
    }

    // handling of terminated children
    if (sigaction(SIGCHLD, &act, NULL) == -1) {
        // TODO: log the error
    }
}

void debug_conf(voy_conf_t *conf)
{
    if (conf && conf->default_server) {
        if (conf->default_server->ports) {
            VOY_ARRAY_FOREACH (conf->default_server->ports, i) {
                int *n = voy_array_get(conf->default_server->ports, i);
                printf("found port: %d\n", *n);
            }
        }
        if (conf->default_server->index_pages) {
            VOY_ARRAY_FOREACH (conf->default_server->index_pages, i) {
                voy_str_t *i_page = voy_array_get(conf->default_server->index_pages, i);
                printf("found index page: %s\n", i_page->string);
            }
        }
        if (conf->default_server->error_pages) {
            VOY_ARRAY_FOREACH (conf->default_server->error_pages, i) {
                voy_error_page_t *err_page = voy_array_get(conf->default_server->error_pages, i);
                printf("found error page (code): %d\n", *(err_page->code));
                printf("found error page (path): %s\n", err_page->page->string);
            }
        }
    }

    if (conf && conf->vhosts) {
        VOY_ARRAY_FOREACH (conf->vhosts, i) {
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
                    VOY_ARRAY_FOREACH (cur_vhost->error_pages, i) {
                        voy_error_page_t *err_page = voy_array_get(cur_vhost->error_pages, i);
                        printf("vhost error page (code): %d\n", *(err_page->code));
                        printf("vhost error page (path): %s\n", err_page->page->string);
                    }
                }
            }
        }
    }
    exit(EXIT_SUCCESS);
}

static void voy_print_version()
{
    fprintf(stderr, "voy version v%s\n", VOY_VERSION);
    exit(EXIT_SUCCESS);
}

static void voy_print_usage()
{
    fprintf(stderr, "Usage: voy -[chlv] [-c conf] [-l path]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h\t\t: This help menu\n");
    fprintf(stderr, "  -v\t\t: Show server version and exit\n");
    fprintf(stderr, "  -c filename\t: Use the specified configuration file (default is %s)\n",
            DEFAULT_CONF_FILE);
    fprintf(stderr, "  -l path\t: Store logs in this location (default is %s)\n", DEFAULT_LOG_FILE);
    fprintf(stderr, "\n");
    exit(EXIT_SUCCESS);
}
