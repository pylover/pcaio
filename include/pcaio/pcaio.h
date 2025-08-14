#ifndef PCAIO_PCAIO_H_
#define PCAIO_PCAIO_H_


struct pcaio;
struct pcaio_task;
typedef int (*pcaio_entrypoint_t) (int argc, void *argv[]);


/* this structure must be filled by user */
struct pcaio_config {
    size_t taskqueue_size;
    unsigned char workers;
};


struct pcaio *
pcaio_new(const struct pcaio_config *config);


int
pcaio_free();


int
pcaio();


struct pcaio_task *
pcaio_schedule(const char *id, pcaio_entrypoint_t func, int argc, ...);


struct pcaio_task *
pcaio_task_new(const char *id, pcaio_entrypoint_t func, int argc, ...);


int
pcaio_task_schedule(struct pcaio_task *t);


void
pcaio_task_relax();


#endif  // PCAIO_PCAIO_H_
