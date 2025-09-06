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


/* standard */
/* thirdparty */
#include <clog.h>

/* local public */
#include "pcaio/modio.h"


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
await_read(int fd, void *buf, size_t count) {
    ssize_t bytes;

    FEED(0);

retry:
    bytes = read(fd, buf, count);
    if (bytes == -1) {
        if (!RETRY(errno)) {
            return -1;
        }

        if (pcaio_modio_await(fd, IOREAD)) {
            return -1;
        }

        goto retry;
    }

    errno = 0;
    return bytes;
}
