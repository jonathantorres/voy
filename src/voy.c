#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "voy_server.h"

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
void *voy_conf_load(char *conf_file_path);

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

    void *conf = voy_conf_load(conf_file_path);
    if (!conf) {
        // handle error here and exit
    }

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

// TODO: move this function to it's own file and header
void *voy_conf_load(char *conf_file_path)
{
    if (conf_file_path) {

    }
    return NULL;
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
