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


#define TASKS_MAX 200
#define WORKERS 2


static struct pcaio_config
_config = {
    .workers_min = WORKERS,
    .workers_max = WORKERS,
};


static int
_task(int argc, void *argv[]) {
    pcaio_currenttask_relax();
    pcaio_currenttask_relax();
    return 0;
}


int
main() {
    int i;

    pcaio_init(&_config);
    for (i = 0; i < TASKS_MAX; i++) {
        pcaio_task_newschedule(_task, 0);
    }
    pcaio();
    pcaio_deinit();
    INFO("%ld tasks has been completed successfully", TASKS_MAX);
    return EXIT_SUCCESS;
}
