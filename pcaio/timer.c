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
#include <unistd.h>
#include <errno.h>

/* system */
#include <sys/timerfd.h>

/* local public */
#include "pcaio/modio.h"
#include "pcaio/timer.h"


struct pcaio_timer {
    int fd;
    struct pcaio_iomodule *iomod;
};


static int
_settimeout(int fd, time_t ms) {
    if ((ms == 0) || (fd < 0)) {
        errno = EINVAL;
        return -1;
    }

    struct timespec sec = {ms / 1000, (ms % 1000) * 1000000};
    struct timespec zero = {0, 0};
    struct itimerspec spec = {zero, sec};
    if (timerfd_settime(fd, 0, &spec, NULL) == -1) {
        return -1;
    }

    return 0;
}


struct pcaio_timer *
pcaio_timer_new(struct pcaio_iomodule *iomod) {
    int fd;
    struct pcaio_timer *t;

    fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
    if (fd == -1) {
        return NULL;
    }

    t = malloc(sizeof(struct pcaio_timer));
    if (t == NULL) {
        return NULL;
    }

    t->fd = fd;
    t->iomod = iomod;
    return t;
}


int
pcaio_timer_free(struct pcaio_timer *t) {
    if (t == NULL) {
        return -1;
    }

    close(t->fd);
    free(t);

    return 0;
}


int
sleepA_ms(struct pcaio_timer *t, time_t ms) {
    if (_settimeout(t->fd, ms)) {
        return -1;
    }

    if (t->iomod) {
        return t->iomod->await(t->fd, IOIN);
    }

    return pcaio_modio_await(t->fd, IOIN);
}
