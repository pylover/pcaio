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


/* standard */
#include <errno.h>
#include <stdatomic.h>

/* posix */
#include <pthread.h>

/* local private */
#include "gather.h"
#include "master.h"


void
gather_taskterminated(struct pcaio_task *t) {
    struct gather *g = t->gather;

    pthread_mutex_lock(&g->mutex);
    atomic_fetch_sub(&g->count, 1);
    if (g->count) {
        pthread_mutex_unlock(&g->mutex);
        return;
    }

    /* all tasks are terminated */
    pthread_mutex_unlock(&g->mutex);
    pthread_mutex_destroy(&g->mutex);
    pcaio_schedule(g->task);
}


int
pcaio_gatherA(struct pcaio_task *tasks[], int count) {
    int i;
    int err;

    struct gather g = {
        .count = count,
        .task = pcaio_self(),
    };

    err = pthread_mutex_init(&g.mutex, NULL);
    if (err) {
        errno = err;
        return -1;
    }

    for (i = 0; i < count; i++) {
        tasks[i]->gather = &g;
        tasks[i]->terminated = gather_taskterminated;
        if (taskqueue_push(&state.taskq, tasks[i])) {
            goto failed;
        }
    }

    if (pcaio_relaxA(TASK_NOSCHEDULE)) {
        goto failed;
    }

    return 0;

failed:
    pthread_mutex_destroy(&g.mutex);
    return -1;
}
