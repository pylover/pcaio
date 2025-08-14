#ifndef CORE_CORE_H_
#define CORE_CORE_H_


struct pcaio {
    const struct pcaio_config *config;
    struct taskqueue *tasks;
};


#endif  // CORE_CORE_H_
