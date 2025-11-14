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


/* system */
#include <sys/uio.h>

/* posix */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/* thirdparty */
#include <clog.h>

/* local public */
#include "pcaio/modio.h"
#undef T
#define T pcaio_ioevent
#include "pcaio/ringT.c"
#include "pcaio/listT.c"


struct modio {
    struct pcaio_module;

    struct pcaio_iomodule *defaultmodule;
};


static struct modio *_mod = NULL;


static int
_dtor() {
    if (_mod == NULL) {
        return -1;
    }

    free(_mod);
    _mod = NULL;
    return 0;
}


int
pcaio_modio_await(int fd, int events) {
    if (_mod == NULL) {
        return -1;
    }

    return _mod->defaultmodule->await(fd, events);
}


int
pcaio_modio_use(struct pcaio_iomodule *defmod) {
    if (defmod == NULL) {
        return -1;
    }

    if (_mod) {
        /* already installed */
        return -1;
    }

    _mod = malloc(sizeof(struct modio));
    if (_mod == NULL) {
        return -1;
    }

    _mod->name = "io";
    _mod->dtor = _dtor;
    _mod->init = NULL;
    _mod->tick = NULL;

    _mod->defaultmodule = defmod;

    if (pcaio_module_install((struct pcaio_module *)_mod)) {
        free(_mod);
        _mod = NULL;
        return -1;
    }
    return 0;
}


ssize_t
readA(int fd, void *buf, size_t count) {
    ssize_t bytes;

    pcaio_relaxA(0);

retry:
    bytes = read(fd, buf, count);
    if (bytes == -1) {
        if (!RETRY(errno)) {
            return -1;
        }

        if (pcaio_modio_await(fd, IOIN)) {
            return -1;
        }

        errno = 0;
        goto retry;
    }

    return bytes;
}


ssize_t
writeA(int fd, void *buf, size_t count) {
    ssize_t bytes;

    pcaio_relaxA(0);

retry:
    bytes = write(fd, buf, count);
    if (bytes == -1) {
        if (!RETRY(errno)) {
            return -1;
        }

        if (pcaio_modio_await(fd, IOOUT)) {
            return -1;
        }

        errno = 0;
        goto retry;
    }

    return bytes;
}


int
acceptA(int sockfd, struct sockaddr *restrict addr,
        socklen_t *restrict addrlen) {
    int fd;

    pcaio_relaxA(0);

retry:
    fd = accept(sockfd, addr, addrlen);
    if (fd == -1) {
        if (!RETRY(errno)) {
            return -1;
        }

        if (pcaio_modio_await(sockfd, IOIN)) {
            return -1;
        }

        errno = 0;
        goto retry;
    }

    return fd;
}


#ifdef _GNU_SOURCE

int
accept4A(int sockfd, struct sockaddr *restrict addr,
        socklen_t *restrict addrlen, int flags) {
    int fd;

    pcaio_relaxA(0);

retry:
    fd = accept4(sockfd, addr, addrlen, flags);
    if (fd == -1) {
        if (!RETRY(errno)) {
            return -1;
        }

        if (pcaio_modio_await(sockfd, IOIN)) {
            return -1;
        }

        errno = 0;
        goto retry;
    }

    return fd;
}

#endif  // _GNU_SOURCE


#ifdef _DEFAULT_SOURCE

ssize_t
writevA(int fd, const struct iovec *iov, int iovcnt) {
    ssize_t bytes;

    pcaio_relaxA(0);

retry:
    bytes = writev(fd, iov, iovcnt);
    if (bytes == -1) {
        if (!RETRY(errno)) {
            return -1;
        }

        if (pcaio_modio_await(fd, IOOUT)) {
            return -1;
        }

        errno = 0;
        goto retry;
    }

    return bytes;
}


#endif  // _DEFAULT_SOURCE


#if _POSIX_C_SOURCE >= 200112L

int
getaddrinfoA(const char *restrict node, const char *restrict service,
        const struct addrinfo *restrict hints,
        struct addrinfo **restrict result) {
    int ret;

retry:
    ret = getaddrinfo(node, service, hints, result);
    if (RETRY(ret)) {
        pcaio_relaxA(0);
        errno = 0;
        goto retry;
    }

    return ret;
}


#endif  // _POSIX_C_SOURCE >= 200112L
