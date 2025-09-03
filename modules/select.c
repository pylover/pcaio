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
#include <stdlib.h>
#include <stdatomic.h>
#include <math.h>

/* system */
#include <sys/resource.h>

/* thirdparty */
#include <clog.h>

/* local public */
#include "pcaio/io.h"
#include "pcaio/select.h"
#undef T
#define T pcaio_ioevent
#include "pcaio/ringT.h"
#include "pcaio/ringT.c"


struct pcaio_modselect {
    struct pcaio_module;

    unsigned short maxfileno;
    struct pcaio_ioeventring events;
    struct pcaio_ioevent *swap[];
};


/** from select(2)
 * POSIX allows an implementation to define an upper limit, advertised via
 * the constant FD_SETSIZE, on the range of file descriptors that can be
 * specified in a file  descriptor  set. The Linux kernel imposes no fixed
 * limit, but the glibc implementation makes fd_set a fixed-size type, with
 * FD_SETSIZE defined as 1024, and the FD_*() macros operating according to
 * that limit.  To monitor file descriptors greater than 1023, use poll(2)
 * or epoll(7) instead.
 */
#define MAXFILENO 1024


static struct pcaio_modselect *_mod = NULL;


static int
_dtor() {
    if (_mod == NULL) {
        return -1;
    }

    pcaio_ioeventring_deinit(&_mod->events);
    free(_mod);
    _mod = NULL;
    return 0;
}


static int
_tick(unsigned int timeout_us) {
    int i;
    int ready;
    int nfds;
    struct timeval tv;
    struct pcaio_ioevent *e;
    unsigned short swapcount;
    fd_set rfds;
    fd_set wfds;
    fd_set efds;

    if (RINGT_ISEMPTY(&_mod->events)) {
        return 0;
    }

    tv.tv_usec = timeout_us % 1000000;
    tv.tv_sec = timeout_us / 1000000;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    swapcount = 0;
    while (swapcount < _mod->maxfileno) {
        if (pcaio_ioeventring_pop(&_mod->events, &e)) {
            break;
        }

        if (!e->events) {
            /* what the hell, event is empty! */
            return -1;
        }

        if (e->events & IOREAD) {
            FD_SET(e->fd, &rfds);
        }

        if (e->events & IOWRITE) {
            FD_SET(e->fd, &wfds);
        }

        // TODO: register always
        if (e->events & IOERROR) {
            FD_SET(e->fd, &efds);
        }

        _mod->swap[swapcount++] = e;
    }

    if (swapcount == 0) {
        return 0;
    }

    nfds = select(_mod->maxfileno, &rfds, &wfds, &efds, &tv);
    if (nfds == -1) {
        ERROR("select() -> nfds: %d", nfds);
        return -1;
    }

    // if (nfds == 0) {
    //     return 0;
    // }

    for (i = 0; i < swapcount; i++) {
        e = _mod->swap[i];
        ready = 0;
        if (FD_ISSET(e->fd, &rfds)) {
            ready |= IOREAD;
        }

        if (FD_ISSET(e->fd, &wfds)) {
            ready |= IOWRITE;
        }

        if (FD_ISSET(e->fd, &efds)) {
            ready |= IOERROR;
        }

        if (!ready) {
            if (pcaio_ioeventring_push(&_mod->events, e)) {
                /* what the hell, the event queue is full.
                 * panic has been occured */
                ERROR("pcaio_ioeventring_push");
                return -1;
            }
            continue;
        }

        e->events = ready;
        pcaio_schedule(e->task);
    }

    return 0;
}


int
pcaio_modselect_monitor(struct pcaio_ioevent *e) {
    return pcaio_ioeventring_push(&_mod->events, e);
}


int
pcaio_modselect_wait(int fd, int events) {
    struct pcaio_ioevent e;
    struct pcaio_task *t;

    if ((fd < 0) || (fd > _mod->maxfileno) || (events == 0)) {
        return -1;
    }

    t = pcaio_self();
    if (t == NULL) {
        return -1;
    }

    e.events = events;
    e.task = t;
    e.fd = fd;

    if (pcaio_modselect_monitor(&e)) {
        return -1;
    }

    if (pcaio_relax(TASK_NOSCHEDULE)) {
        return -1;
    }

    return 0;
}


int
pcaio_modselect_use(unsigned short maxfileno) {
    size_t sz;
    unsigned char eventring_storagebits;
    struct rlimit limits;
    struct pcaio_modselect *m;

    if (maxfileno > MAXFILENO) {
        return -1;
    }

    if (getrlimit(RLIMIT_NOFILE, &limits)) {
        return -1;
    }

    if (maxfileno > limits.rlim_max) {
        return -1;
    }

    sz = sizeof(struct pcaio_modselect)
        + sizeof(struct pcaio_ioevent*) * maxfileno;
    m = malloc(sz);
    if (m == NULL) {
        return -1;
    }

    eventring_storagebits = (unsigned char)ceil(log2(maxfileno));
    if (pcaio_ioeventring_init(&m->events, eventring_storagebits)) {
        free(m);
        return -1;
    }

    m->name = "select";
    m->dtor = _dtor;
    m->tick = _tick;
    m->maxfileno = maxfileno;

    if (pcaio_module_install((struct pcaio_module *)m)) {
        pcaio_ioeventring_deinit(&m->events);
        free(m);
        return -1;
    }

    _mod = m;
    return 0;
}
