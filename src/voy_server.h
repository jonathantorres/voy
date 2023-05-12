#ifndef _voy_server_h
#define _voy_server_h

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "voy_conf.h"
#include "voy_file.h"
#include "voy_request.h"
#include "voy_response.h"
#include "voy_str.h"

#define VOY_DEFAULT_PORT  9090
#define VOY_RECV_BUFF_LEN 1000000

bool voy_server_start(voy_conf_t *conf);
bool voy_server_shutdown();
#endif
