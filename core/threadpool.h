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
#ifndef CORE_THREADPOOL_H_
#define CORE_THREADPOOL_H_


/* local private */
#include "thread.h"

/* local public */
#include "pcaio/pcaio.h"


struct threadpool {
    thread_start_t starter;
    void *starter_arg;
    unsigned short min;
    unsigned short max;
    unsigned short count;
    thread_t *threads;
};


int
threadpool_init(struct threadpool *p, struct pcaio_config *c,
        thread_start_t starter, void *arg);


int
threadpool_deinit(struct threadpool *p);


int
threadpool_startall(struct threadpool *p);


int
threadpool_waitall(struct threadpool *p);


#endif  // CORE_THREADPOOL_H_
