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
await_read(int fd, void *buf, size_t count);


ssize_t
await_write(int fd, void *buf, size_t count);


int
await_accept(int sockfd, struct sockaddr *restrict addr,
        socklen_t *restrict addrlen);


#ifdef _GNU_SOURCE
int
await_accept4(int sockfd, struct sockaddr *restrict addr,
        socklen_t *restrict addrlen, int flags);
#endif  // _GNU_SOURCE


#endif  // INCLUDE_PCAIO_MODIO_H_
