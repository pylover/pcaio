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
    .workers = 0,
    .cancel = true,
    .tasks = {NULL, NULL, 0},
    .modulescount = 0,
};


int
master_init(unsigned short workers) {
    int i;
    struct pcaio_module *m;

    if (state.workers) {
        ERROR("already initialized");
        return -1;
    }

    tasklist_init(&state.tasks);
    taskqueue_init(&state.taskq);
    if (threadpool_init(&state.pool, workers, worker, &state.taskq)) {
        tasklist_deinit(&state.tasks);
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
            ERROR("pcaio_mod%s_init", m->name);
            goto failed;
        }
    }

    state.workers = workers;
    state.cancel = false;
    return 0;

failed:
    threadpool_deinit(&state.pool);
    tasklist_deinit(&state.tasks);
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
    tasklist_deinit(&state.tasks);
    taskqueue_deinit(&state.taskq);

    /* freeup installed modules */
    for (i = 0; i < state.modulescount; i++) {
        m = state.modules[i];
        if (m->dtor && m->dtor(m)) {
            ERROR("pcaio_mod%s_dtor", m->name);
        }
    }

    state.workers = 0;
    state.cancel = true;
    return 0;
}


int
master() {
    int i;
    int ret = 0;
    int tickstatus;
    int any = 1;
    struct pcaio_task *t;
    struct threadpool *tp;
    struct pcaio_module *m;
    unsigned int toutus;

    /* scale the threadpool to minimum capacity */
    tp = &state.pool;
    if (threadpool_scale(tp, state.workers)) {
        ERROR("threadpool_scale");
        return -1;
    }

    /* indicate this is the master thread */
    threadlocalucontext_set(NULL);

    /* main loop */
    while ((!state.cancel) && (state.tasks.count)) {
        /* modules tick */
        if (any) {
            toutus = CONFIG_PCAIO_MODULETIMEOUT_SHORT_US;
        }
        else {
            toutus = CONFIG_PCAIO_MODULETIMEOUT_LONG_US;
            usleep(CONFIG_PCAIO_MODULETIMEOUT_LONG_US);
        }
        any = 0;
        for (i = 0; i < state.modulescount; i++) {
            m = state.modules[i];
            if (((m->flags & MOD_PANIC) == 0) && m->tick) {
                tickstatus = m->tick(toutus);
                if (tickstatus == PMSIDLE) {
                    continue;
                }

                if (tickstatus == PMSAGAIN) {
                    any |= 1;
                    continue;
                }

                /* panic */
                m->flags |= MOD_PANIC;
                ERROR("mod%s panic", m->name);
            }
        }
    }

    INFO("shutting down all workers...");
    if (threadpool_scale(tp, 0)) {
        ERROR("threadpool_scale");
        ret = -1;
    }

    INFO("freeup remaining task(s)...");
    while (taskqueue_pop(&state.taskq, &t, 0) == 0) {
        task_free(t);
    }

    INFO("all workers have been shut down.");
    return ret;
}
