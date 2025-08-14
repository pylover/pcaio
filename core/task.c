/* standard */
#include <stdlib.h>
#include <errno.h>

/* system */
/* thirdparty */
#include <clog.h>

/* local private */
#include "task.h"
#undef queue_t
#define queue_t task
#include "pcaio/queueT.c"

/* local public */
#include "pcaio/core.h"

/* local private */
#include "config.h"


static void
_taskmain(struct pcaio_task *t) {
    int status;

    status = t->func(t->argc, t->argv);
    if (status) {
        ERROR("task %s exited with status: %d", t->id, status);
    }
}


struct pcaio_task *
task_new(const char *id, pcaio_entrypoint_t func, int argc, va_list args) {
    struct pcaio_task *t;

    /* validate arguments */
    if ((id == NULL) || (func == NULL)) {
        errno = EINVAL;
        return NULL;
    }

    /* allocate mem for task */
    t = malloc(sizeof(struct pcaio_task) + argc * sizeof(void *));
    if (t == NULL) {
        return NULL;
    }

    /* store the id */
    t->id = id;

    /* store entry point */
    t->func = func;

    /* copy the args */
    t->argc = argc;
    while (argc) {
        t->argv[t->argc - argc] = va_arg(args, void *);
        argc--;
    }

    /* hollaaaa */
    t->status = TS_IDLE;
    return t;
}


int
task_createcontext(struct pcaio_task *t, size_t size) {
    void *stack;
    // struct pointerbag *bag = (struct pointerbag *)t;

    if ((size == 0) || (size > CONFIG_PCAIO_STACKSIZE_MAX)) {
        errno = EINVAL;
        return -1;
    }

    /* copy he current context */
    if (getcontext(&t->context) != 0) {
        return -1;
    }

    /* allocate mem for stack */
    stack = malloc(size);
    if (stack == NULL) {
        free(t);
        return -1;
    }
    t->context.uc_stack.ss_sp = stack;
    t->context.uc_stack.ss_size = size;

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
    makecontext(&t->context, (void (*)(void))_taskmain, 1, t);

    return 0;
}


int
task_dispose(struct pcaio_task *t) {
    if (t == NULL) {
        return -1;
    }

    if (t->context.uc_stack.ss_sp) {
        free(t->context.uc_stack.ss_sp);
    }
    free(t);
    return 0;
}
