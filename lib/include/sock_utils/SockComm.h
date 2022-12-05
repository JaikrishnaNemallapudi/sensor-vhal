/*
** Copyright 2022 Intel Corporation
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** SPDX-License-Identifier: Apache-2.0
*/

#ifndef SOCK_COMM_H
#define SOCK_COMM_H

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef int SOCK_BOOL;
#define SOCK_TRUE 1
#define SOCK_FALSE 0

#define SOCK_MAX_PATH_LEN 128

#define UNUSED(expr)  \
    do {              \
        (void)(expr); \
    } while (0)

#ifndef LOG_TAG
#define LOG_TAG "sensors@2.0-service.intel"
#endif

#ifdef HOST_BUILD
#include <errno.h>
#include <memory.h>
#define SOCK_UTIL_DEFAULT_PORT 6666
#define SOCK_UTIL_DEFAULT_PATH "./ipc/sock-util-default"
#define sock_log(...)     \
    printf("[INFO] ");    \
    printf("[SOCK_LOG]"); \
    printf(__VA_ARGS__);  \
    printf("\n");
#else
#include <android/log.h>
#define sock_log(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__);
#define SOCK_UTIL_DEFAULT_PORT 6666
#define SOCK_UTIL_DEFAULT_PATH "/ipc/display-sock"
#endif

#define CLIENT_BUF_CAPACITY (2 * 1024 * 1024)
typedef enum _sock_conn_status { disconnect = -1, normal = 0, readable = 1 } sock_conn_status_t;

#define DEBUG_SOCK_SERVER 0
#define DEBUG_SOCK_CLIENT 0

#define USE_INET_SOCKET 1

typedef enum _sock_conn_type {
    SOCK_CONN_TYPE_ABS_SOCK  = 0,
    SOCK_CONN_TYPE_UNIX_SOCK = 1,
    SOCK_CONN_TYPE_INET_SOCK = 2,
} SOCK_conn_type_t;

#endif /* SOCK_UTIL_H */
