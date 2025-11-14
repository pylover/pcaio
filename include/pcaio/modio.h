// Copyright 2025 Vahid Mardani
/*
 * This file is part of pcaio.
 *  pcaio is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  pcaio is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with pcaio. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */
#ifndef INCLUDE_PCAIO_MODIO_H_
#define INCLUDE_PCAIO_MODIO_H_


/* statndard */
#include <errno.h>

/* system */
#include <sys/socket.h>

/* local public */
#include "pcaio/pcaio.h"


struct pcaio_ioevent;
typedef struct pcaio_ioevent {
    int fd;
    int events;
    struct pcaio_task *task;

    /* ioevent list */
    struct pcaio_ioevent *pcaio_ioeventlist_next;
    struct pcaio_ioevent *pcaio_ioeventlist_prev;
} pcaio_ioevent_t;
#undef T
#define T pcaio_ioevent
#include "pcaio/ringT.h"
#include "pcaio/listT.h"


struct pcaio_iomodule {
    struct pcaio_module;
    int (*await)(int fd, int events);
};


#define IOIN 0x1
#define IOOUT 0x4
#define IOERR 0x8
#define RETRY(e) \
    (((e) == EAGAIN) || ((e) == EWOULDBLOCK) || ((e) == EINPROGRESS))


int
pcaio_modio_use(struct pcaio_iomodule *defmod);


int
pcaio_modio_await(int fd, int events);


ssize_t
readA(int fd, void *buf, size_t count);


ssize_t
writeA(int fd, void *buf, size_t count);


int
acceptA(int sockfd, struct sockaddr *restrict addr,
        socklen_t *restrict addrlen);


int
connectA(int sockfd, const struct sockaddr *addr, socklen_t addrlen);


#ifdef _GNU_SOURCE
int
accept4A(int sockfd, struct sockaddr *restrict addr,
        socklen_t *restrict addrlen, int flags);
#endif  // _GNU_SOURCE


#ifdef _DEFAULT_SOURCE
ssize_t
writevA(int fd, const struct iovec *iov, int iovcnt);
#endif  // _DEFAULT_SOURCE


#if _POSIX_C_SOURCE >= 200112L
/* posix */
#include <sys/types.h>
#include <netdb.h>
int
getaddrinfoA(const char *restrict node, const char *restrict service,
        const struct addrinfo *restrict hints,
        struct addrinfo **restrict result);
#endif  // _POSIX_C_SOURCE >= 200112L


#endif  // INCLUDE_PCAIO_MODIO_H_
