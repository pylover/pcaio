#ifndef PCAIO_H_
#define PCAIO_H_


#include <ucontext.h>


typedef int (*pcaio_entrypoint_t) (int argc, void *argv[]);


struct pcaio_task {
    const char *id;
    struct ucontext_t uctx;
    pcaio_entrypoint_t func;
    int argc;
    void *argv[];
};


/* this structure must be filled by user */
struct pcaio_config {
    size_t taskqueue_size;
    size_t taskstack_size;
};


/* this structure is not thread-safe */
struct pcaio {
    const struct pcaio_config *config;
    struct ucontext_t uctx;
    struct pcaio_task *queue;
};


struct pcaio *
pcaio_new(const struct pcaio_config *config);


int
pcaio_free(struct pcaio *p);


int
pcaio_schedule(struct pcaio *p, struct pcaio_task *t);


struct pcaio_task *
pcaio_spawn(struct pcaio *p, const char *id, pcaio_entrypoint_t func,
        int argc, ...);


int
pcaio_await(struct pcaio_task *task);


#endif  // PCAIO_H_
