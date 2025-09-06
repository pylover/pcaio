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

/* local public */
#include "pcaio/pcaio.h"


struct pcaio_iomodule {
    struct pcaio_module;
    int (*await)(int fd, int events);
};


#define IOREAD 0x1
#define IOWRITE 0x2
#define IOERROR 0x4
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


#endif  // INCLUDE_PCAIO_MODIO_H_
