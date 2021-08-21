#ifndef _voy_server_h
#define _voy_server_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#include "voy_request.h"
#include "voy_response.h"
#include "voy_file.h"
#include "voy_str.h"
#include "voy_conf.h"

#define VOY_DEFAULT_PORT 9090
#define VOY_RECV_BUFF_LEN 1000000

bool voy_server_start(voy_conf_t *conf);
bool voy_server_shutdown();
#endif
