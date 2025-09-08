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
#include "pcaio/pcaio.h"
#include "pcaio/modio.h"
#include "pcaio/modselect.h"


struct modselect {
    struct pcaio_iomodule;

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


static struct modselect *_mod = NULL;


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
    int ret = 0;
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

        if (e->events & IOIN) {
            FD_SET(e->fd, &rfds);
        }

        if (e->events & IOOUT) {
            FD_SET(e->fd, &wfds);
        }

        FD_SET(e->fd, &efds);
        _mod->swap[swapcount++] = e;
    }

    if (swapcount == 0) {
        return 0;
    }

    nfds = select(_mod->maxfileno, &rfds, &wfds, &efds, &tv);
    if (nfds == -1) {
        /* io error, user interrupts or something else */
        ERROR("select() -> nfds: %d", nfds);

        /* clearing read & write filesets but errors */
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        ret = -1;
    }

    for (i = 0; i < swapcount; i++) {
        e = _mod->swap[i];
        ready = 0;
        if (FD_ISSET(e->fd, &rfds)) {
            ready |= IOIN;
        }

        if (FD_ISSET(e->fd, &wfds)) {
            ready |= IOOUT;
        }

        if ((nfds == -1) || FD_ISSET(e->fd, &efds)) {
            ready |= IOERR;
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

    return ret;
}


int
pcaio_modselect_await(int fd, int events) {
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

    if (pcaio_ioeventring_push(&_mod->events, &e)) {
        return -1;
    }

    if (pcaio_feed(TASK_NOSCHEDULE)) {
        return -1;
    }

    if (e.events & IOERR) {
        return -1;
    }

    return 0;
}


int
pcaio_modselect_use(unsigned short maxfileno, struct pcaio_iomodule **out) {
    size_t sz;
    unsigned char eventring_storagebits;
    struct rlimit limits;
    struct modselect *m;

    if (maxfileno > MAXFILENO) {
        return -1;
    }

    if (getrlimit(RLIMIT_NOFILE, &limits)) {
        return -1;
    }

    if (maxfileno > limits.rlim_max) {
        return -1;
    }

    sz = sizeof(struct modselect)
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
    m->init = NULL;
    m->dtor = _dtor;
    m->tick = _tick;
    m->await = pcaio_modselect_await;
    m->maxfileno = maxfileno;

    if (pcaio_module_install((struct pcaio_module *)m)) {
        pcaio_ioeventring_deinit(&m->events);
        free(m);
        return -1;
    }

    _mod = m;
    if (out) {
        *out = (struct pcaio_iomodule*)m;
    }
    return 0;
}
