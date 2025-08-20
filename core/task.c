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
#include <errno.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "config.h"
#include "task.h"
#undef T
#define T task
#include "threadlocalT.c"
#include "queueT.c"

/* local public */
#include "pcaio/pcaio.h"


static void
_taskmain(struct pcaio_task *t) {
    int status;

    status = t->func(t->argc, t->argv);
    if (status) {
        ERROR("task %p exited with status: %d", t, status);
    }
    t->status = TS_TERMINATING;
}


struct pcaio_task *
task_new(pcaio_entrypoint_t func, int argc, va_list args) {
    struct pcaio_task *t;
    size_t allocsize;

    /* validate arguments */
    if (func == NULL) {
        errno = EINVAL;
        return NULL;
    }

    /* allocate mem for task */
    allocsize = sizeof(struct pcaio_task) + argc * sizeof(void *);
    t = malloc(allocsize);
    if (t == NULL) {
        return NULL;
    }

    /* zero it */
    memset(t, 0, allocsize);

    /* store entry point */
    t->func = func;

    /* copy the args */
    t->argc = argc;
    while (argc) {
        t->argv[t->argc - argc] = va_arg(args, void *);
        argc--;
    }

    /* hollaaaa */
    t->status = TS_NAIVE;
    t->stacksize = CONFIG_PCAIO_STACKSIZE_DEFAULT;
    return t;
}


/** allocate stack and create a dedicated ucontext for the task.
 */
int
task_createcontext(struct pcaio_task *t, ucontext_t *successor) {
    void *stack;

    /* copy he current context */
    if (getcontext(&t->context) != 0) {
        return -1;
    }

    /* allocate mem for stack */
    stack = malloc(t->stacksize);
    if (stack == NULL) {
        free(t);
        return -1;
    }
    memset(stack, 0, t->stacksize);
    t->context.uc_stack.ss_sp = stack;
    t->context.uc_stack.ss_size = t->stacksize;
    t->context.uc_link = successor;

    /* create the actual ucontext */
    /* On architectures where int and pointer types are the same size
     * (e.g., x86-32, where both types are 32 bits), you may be able to get
     * away with passing pointers as arguments to makecontext() following
     * argc. However, doing this is not guaranteed to be portable, is
     * undefined according to the standards, and won't work on architectures
     * where pointers are larger than ints. Nevertheless, starting with
     * glibc 2.8, glibc makes some changes to makecontext(), to permit this on
     * some 64-bit architectures (e.g., x86-64).
     * */
    // TODO: glibc >= 2.8 test macro
    makecontext(&t->context, (void (*)(void))_taskmain, 1, t);

    t->status = TS_ARTFUL;
    return 0;
}


void
task_free(struct pcaio_task *t) {
    if (t == NULL) {
        return;
    }

    if (t->context.uc_stack.ss_sp) {
        free(t->context.uc_stack.ss_sp);
    }
    free(t);
}
