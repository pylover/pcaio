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
/* thirdparty */
#include <cutest.h>

/* local public */
#include "pcaio/pcaio.h"


static int
_subtaskA(int argc, void *argv[]) {
    unsigned int seed = (unsigned long) argv[0];

    pcaio_relaxA(0);
    return seed * seed;
}


static int
_taskA(int argc, void *argv[]) {
    int i;
    struct pcaio_task *tasks[3];
    int statuses[3];
    int sum = 0;

    for (i = 0; i < 3; i++) {
        tasks[i] = pcaio_task_new(_subtaskA, statuses + i, NULL, 1, i + 1);
    }
    pcaio_gatherA(tasks, 3);
    for (i = 0; i < 3; i++) {
        sum += statuses[i];
    }

    eqint(14, sum);
    return 0;
}


static void
test_gather() {
    struct pcaio_task *task;

    task = pcaio_task_new(_taskA, NULL, NULL, 0);
    eqint(0, pcaio(1, &task, 1));
}


int
main() {
    test_gather();
    return EXIT_SUCCESS;
}
