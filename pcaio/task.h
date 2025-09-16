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
#ifndef PCAIO_TASK_H_
#define PCAIO_TASK_H_


/* standard */
#include <ucontext.h>

/* local private */
typedef struct pcaio_task task_t;
#undef T
#define T task
#include "threadlocalT.h"

/* local public */
typedef struct pcaio_task workertask_t;
#undef T
#define T workertask
#include "pcaio/queueT.h"
#include "pcaio/pcaio.h"


struct pcaio_task {
    /* used by anyone to flag the task */
    int flags;

    /* used by taskqueue */
    struct pcaio_task *workertaskqueue_next;

    /* used by worker */
    struct ucontext_t context;

    /* provided by the user */
    int exitstatus;
    pcaio_taskmain_t func;
    int argc;
    void *argv[];
};


struct pcaio_task *
task_new(pcaio_taskmain_t func, int argc, va_list args);


void
task_free(struct pcaio_task *t);


#endif  // PCAIO_TASK_H_
