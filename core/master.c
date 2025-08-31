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
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "master.h"
#include "task.h"
#include "context.h"
#include "worker.h"
#include "threadpool.h"

/* local public */
#include "pcaio/pcaio.h"


struct master state = {
    .config = NULL,
    .cancel = true,
    .tasks = 0,
    .modulescount = 0,
};


int
master_init(struct pcaio_config *config) {
    int i;
    struct pcaio_module *m;

    if (state.config) {
        ERROR("already initialized");
        return -1;
    }

    taskqueue_init(&state.taskq);
    if (threadpool_init(&state.pool, config->workers_max, worker,
                &state.taskq)) {
        taskqueue_deinit(&state.taskq);
        return -1;
    }

    /* threadlocal storages */
    if (threadlocaltask_init(task_free) || threadlocalucontext_init(NULL)) {
        goto failed;
    }

    /* initialize installed modules */
    for (i = 0; i < state.modulescount; i++) {
        m = state.modules[i];
        if (m->init && m->init(m)) {
            ERROR("pcaio_module_%s_init", m->name);
            goto failed;
        }
    }

    state.config = config;
    state.cancel = false;
    return 0;

failed:
    threadpool_deinit(&state.pool);
    taskqueue_deinit(&state.taskq);
    return -1;
}


int
master_deinit() {
    int i;
    struct pcaio_module *m;

    threadlocaltask_delete();
    threadlocalucontext_delete();
    threadpool_deinit(&state.pool);
    taskqueue_deinit(&state.taskq);

    /* freeup installed modules */
    for (i = 0; i < state.modulescount; i++) {
        m = state.modules[i];
        if (m->dtor && m->dtor(m)) {
            ERROR("pcaio_module_%s_dtor", m->name);
        }
    }

    state.config = NULL;
    state.cancel = true;
    return 0;
}


int
master() {
    int i;
    struct threadpool *tp;
    struct pcaio_module *m;

    /* scale the threadpool to minimum capacity */
    tp = &state.pool;
    if (threadpool_scale(tp, state.config->workers_min)) {
        ERROR("threadpool_scale");
        return -1;
    }

    while ((!state.cancel) && (state.tasks)) {
        /* modules tick */
        for (i = 0; i < state.modulescount; i++) {
            m = state.modules[i];
            // TODO: config timeout us
            if (m->tick && m->tick(1000)) {
                ERROR("pcaio_module_%s_tick", m->name);
            }
        }
        sleep(.3);
    }

    INFO("shutting down all workers...");
    if (threadpool_scale(tp, 0)) {
        ERROR("threadpool_scale");
        return -1;
    }

    INFO("all workers have been shut down.");
    return 0;
}
