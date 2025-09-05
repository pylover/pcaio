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
#include <string.h>
#include <errno.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "config.h"
#include "context.h"
#include "master.h"
#include "task.h"
#undef T
#define T task
#include "threadlocalT.c"

/* local public */
#undef T
#define T task
#include "pcaio/queueT.c"
#include "pcaio/pcaio.h"


static void
_taskmain(unsigned int p1, unsigned int p2);


struct pcaio_task *
task_new(pcaio_taskmain_t func, int argc, va_list args) {
    struct pcaio_task *t;
    void *stack;
    size_t allocsize;
    unsigned long p;

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
    t->flags = 0;

    /* store entry point */
    t->func = func;

    /* copy the args */
    t->argc = argc;
    while (argc) {
        t->argv[t->argc - argc] = va_arg(args, void *);
        argc--;
    }

    /* copy he current context */
    if (getcontext(&t->context) != 0) {
        free(t);
        return NULL;
    }

    /* allocate mem for stack */
    stack = malloc(CONFIG_PCAIO_STACKSIZE_DEFAULT);
    if (stack == NULL) {
        free(t);
        return NULL;
    }
    memset(stack, 0, CONFIG_PCAIO_STACKSIZE_DEFAULT);
    t->context.uc_stack.ss_sp = stack;
    t->context.uc_stack.ss_size = CONFIG_PCAIO_STACKSIZE_DEFAULT;
    t->context.uc_link = NULL;

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

    /* so, split the task pointer into two unsigned integers */
    p = (long) t;
    makecontext(&t->context, (void (*)(void))_taskmain, 2, p >> 32,
            p & 0xffffffff);

    /* hollaaaa */
    atomic_fetch_add(&state.tasks, 1);
    return t;
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
    atomic_fetch_sub(&state.tasks, 1);
}


static void
_taskmain(unsigned int p1, unsigned int p2) {
    struct pcaio_task *t;
    int exitstatus;
    ucontext_t *landing;

    /* ressemble the task pointer */
    t = (struct pcaio_task *)(((long)p1) << 32 | p2);
    asm volatile("" ::: "memory");

    /* execute the task's main function (aka start the task!) */
    exitstatus = t->func(t->argc, t->argv);
    if (exitstatus) {
        ERROR("task %p exited with status: %d", t, exitstatus);
    }

    /* tell the worker the task is completed and there is nothing to schedule
     * anymore. */
    t->flags |= TASK_TERMINATED;

    /* land */
    landing = threadlocalucontext_get();
    if (landing) {
        setcontext(landing);
    }

    /* ooops! */
    ERROR("threadlocalucontext_get() == NULL");
}
