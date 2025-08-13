#ifndef PCAIO_H_
#define PCAIO_H_


struct pcaio;
struct pcaio_task;
typedef int (*pcaio_entrypoint_t) (int argc, void *argv[]);


/* this structure must be filled by user */
struct pcaio_config {
    size_t taskqueue_size;
    size_t task_stacksize;
    unsigned char workers;
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


int
pcaio_loop(struct pcaio *p);


void
pcaio_imok();


#endif  // PCAIO_H_
