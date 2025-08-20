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
#ifndef CORE_MASTER_H_
#define CORE_MASTER_H_


/* standard */
#include <stdatomic.h>

/* local private */
#include "task.h"
#include "threadpool.h"

/* local public */
#include "pcaio/pcaio.h"


struct master {
    atomic_bool cancel;
    struct pcaio_config *config;
    atomic_uint tasks;
    struct taskqueue taskq;
    struct threadpool pool;
};


int
master_init(struct pcaio_config *config);


int
master_deinit();


void
master_cancel();


struct pcaio_task *
master_assign();


void
master_report(struct pcaio_task *t);


int
master();


#endif  // CORE_MASTER_H_
