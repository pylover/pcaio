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
#include <stdio.h>
#include <stdlib.h>

/* thirdparty */
#include <clog.h>

/* pcaio */
#include <pcaio/pcaio.h>
#include <pcaio/modio.h>
#include <pcaio/modepoll.h>
#include <pcaio/timer.h>


#define TASKS_MAX 3
#define WORKERS 1


static int
_taskA(int argc, void *argv[]) {
    int i;
    long id = (long)argv[0];
    struct pcaio_timer *timer;

    /* NULL means to use the default io module configured via
     * pcaio_modio_use */
    timer = pcaio_timer_new(NULL);
    INFO("task %d started", id);
    pcaio_relaxA(0);
    for (i = 0; i < 5; i++) {
        /* async sleep for 500 miliseconds */
        INFO("task %d #%d sleep", id, i);
        if (sleepA_ms(timer, 50 + i * 100)) {
            ERROR("pcaio_asleep_ms");
            break;
        }
    }

    pcaio_timer_free(timer);
    INFO("task %d done", id);
    return 0;
}


int
main() {
    int i;
    struct pcaio_iomodule *modepoll;
    struct pcaio_task *tasks[TASKS_MAX];

    /* register the epoll module */
    pcaio_modepoll_use(8, &modepoll);
    pcaio_modio_use(modepoll);

    for (i = 0; i < TASKS_MAX; i++) {
        tasks[i] = pcaio_task_new(_taskA, 1, i);
    }
    pcaio(WORKERS, tasks, TASKS_MAX);
    INFO("%ld tasks has been completed successfully", TASKS_MAX);
    return EXIT_SUCCESS;
}
