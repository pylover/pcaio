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
#include <stdatomic.h>
#include <string.h>
#include <signal.h>

/* system */
#include <sys/epoll.h>

/* thirdparty */
#include <clog.h>

/* local public */
#include "pcaio/modio.h"
#include "pcaio/modepoll.h"


struct modepoll {
    struct pcaio_iomodule;

    int fd;
    unsigned short maxevents;
    struct pcaio_ioeventlist waitingevents;
    struct epoll_event events[];
};
static struct modepoll *_mod = NULL;


static int
_init() {
    int fd;

    fd = epoll_create1(0);
    if (fd == -1) {
        return -1;
    }

    memset(&_mod->waitingevents, 0, sizeof(_mod->waitingevents));
    pcaio_ioeventlist_init(&_mod->waitingevents);
    _mod->fd = fd;
    return 0;
}


static int
_dtor() {
    if (_mod == NULL) {
        return -1;
    }

    close(_mod->fd);
    free(_mod);
    _mod = NULL;
    return 0;
}


static enum pcaio_module_tickstatus
_tick(unsigned int timeout_us) {
    int i;
    int ret = PMSAGAIN;
    int nfds;
    struct pcaio_ioevent *e;
    // sigset_t sigmask;

    if (_mod->waitingevents.count == 0) {
        return PMSIDLE;
    }

    /* signals to wait */
    // sigemptyset(&sigmask);
    // sigfillset(&sigmask);
    // sigaddset(&sigmask, SIGINT);

    errno = 0;
    nfds = epoll_wait(_mod->fd, _mod->events, _mod->waitingevents.count,
            timeout_us / 1000);
    if (nfds == 0) {
        goto done;
    }

    if (nfds == -1) {
        ERROR("epoll_wait() -> nfds: %d", nfds);
        ret = PMSPANIC;
        while (pcaio_ioeventlist_pop(&_mod->waitingevents, &e) == 0) {
            /* aka EPOLLERR */
            e->events |= IOERR;
            pcaio_schedule(e->task);
        }
        goto done;
    }

    for (i = 0; i < nfds; i++) {
        e = (struct pcaio_ioevent*)_mod->events[i].data.ptr;
        e->events = _mod->events[i].events;

        if (pcaio_ioeventlist_delete(&_mod->waitingevents, e)) {
            return -1;
        }
        pcaio_schedule(e->task);
    }

done:
    return ret;
}


int
pcaio_modepoll_monitorA(int fd, int events) {
    struct pcaio_ioevent e;
    struct epoll_event ee;
    struct pcaio_task *t;

    if ((fd < 0) || (events == 0)) {
        return -1;
    }

    t = pcaio_self();
    if (t == NULL) {
        return -1;
    }

    memset(&e, 0, sizeof(e));
    e.events = events;
    e.task = t;
    e.fd = fd;

    ee.events = events | EPOLLONESHOT;
    ee.data.ptr = &e;
    if (epoll_ctl(_mod->fd, EPOLL_CTL_MOD, fd, &ee)) {
        if (epoll_ctl(_mod->fd, EPOLL_CTL_ADD, fd, &ee)) {
            return -1;
        }
        errno = 0;
    }

    if (pcaio_ioeventlist_append(&_mod->waitingevents, &e)) {
        epoll_ctl(_mod->fd, EPOLL_CTL_DEL, fd, &ee);
        return -1;
    }

    if (pcaio_relaxA(TASK_NOSCHEDULE)) {
        epoll_ctl(_mod->fd, EPOLL_CTL_DEL, fd, &ee);
        pcaio_ioeventlist_delete(&_mod->waitingevents, &e);
        return -1;
    }

    if (e.events & IOERR) {
        return -1;
    }

    return 0;
}


int
pcaio_modepoll_use(unsigned short maxevents, struct pcaio_iomodule **out) {
    size_t sz;
    struct modepoll *m;

    if (!maxevents) {
        return -1;
    }

    sz = sizeof(struct modepoll)
        + sizeof(struct epoll_event) * maxevents;
    m = malloc(sz);
    if (m == NULL) {
        return -1;
    }

    m->name = "epoll";
    m->init = _init;
    m->dtor = _dtor;
    m->tick = _tick;
    m->await = pcaio_modepoll_monitorA;
    m->maxevents = maxevents;

    if (pcaio_module_install((struct pcaio_module *)m)) {
        free(m);
        return -1;
    }

    _mod = m;
    if (out) {
        *out = (struct pcaio_iomodule*)m;
    }
    return 0;
}
