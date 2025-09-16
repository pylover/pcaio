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
#ifndef PCAIO_MASTER_H_
#define PCAIO_MASTER_H_


/* local private */
#include "config.h"
#include "task.h"
#include "threadpool.h"

/* local public */
#include "pcaio/pcaio.h"


struct master {
    /* provided by user */
    unsigned short workers;

    /* atomic vars used for process exit condition */
    atomic_bool cancel;
    atomic_uint tasks;

    /* the famous taskqueue/threadpool couple */
    struct workertaskqueue taskq;
    struct threadpool pool;

    /* vector of modules pointer*/
    unsigned char modulescount;
    struct pcaio_module *modules[CONFIG_PCAIO_MODULES_MAX];
};


extern struct master state;


int
master_init(unsigned short workers);


int
master_deinit();


int
master();


#endif  // PCAIO_MASTER_H_
