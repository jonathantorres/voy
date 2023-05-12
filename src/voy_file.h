#ifndef _voy_file_h
#define _voy_file_h

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "voy_request.h"
#include "voy_response.h"
#include "voy_str.h"

bool voy_file_serve(voy_request_t *req, voy_response_t *res);

#endif
