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
#ifndef CORE_WORKER_H_
#define CORE_WORKER_H_


/* standard */
#include <ucontext.h>

/* local private */
#include "thread.h"


typedef struct worker {
    struct ucontext_t maincontext;
    struct pcaio_task *currenttask;
} worker_t;


#undef TL
#define TL worker
#include "pcaio/threadlocalT.h"


struct worker *
worker_new();


int
worker_free(struct worker *w);


int
worker(atomic_bool *cancel);


#endif  // CORE_WORKER_H_
