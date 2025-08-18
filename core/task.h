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
#ifndef CORE_TASK_H_
#define CORE_TASK_H_


/* standard */
#include <ucontext.h>

/* local private */
typedef struct pcaio_task task_t;
#undef T
#define T task
#include "pcaio/queueT.h"

/* local public */
#include "pcaio/pcaio.h"


enum taskstatus {
    TS_NAIVE,
    TS_ARTFUL,
    TS_RELAXING,
    TS_TERMINATING,
};


struct pcaio_task {
    /* used by taskqueue */
    struct pcaio_task *next;

    size_t stacksize;
    struct ucontext_t context;
    enum taskstatus status;

    /* provided by the user */
    const char *id;
    pcaio_entrypoint_t func;
    int argc;
    void *argv[];
};



struct pcaio_task *
task_vnew(const char *id, pcaio_entrypoint_t func, int argc, va_list args);


int
task_createcontext(struct pcaio_task *t, ucontext_t *successor);


int
task_free(struct pcaio_task *t);


int
taskqueue_init(struct taskqueue *q);


#endif  // CORE_TASK_H_
