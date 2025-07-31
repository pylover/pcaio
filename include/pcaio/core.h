#ifndef PCAIO_H_
#define PCAIO_H_


typedef int (*pcaio_taskentry_t) (int argc, char *argv[]);


struct pcaio {
    int deleteme;
};


struct pcaio_task {
    int deleteme;
};


struct pcaio *
pcaio_new();


int
pcaio_free(struct pcaio *p);


struct pcaio_task *
pcaio_spawn(struct pcaio *p, pcaio_taskentry_t entry, int argc, ...);


int
pcaio_await(struct pcaio_task *task);


#endif  // PCAIO_H_
