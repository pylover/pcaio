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
/* thirdparty */
#include <cutest.h>
#include <clog.h>

/* local public */
#include "pcaio/pcaio.h"


int hits = 0;
int logs[6];


static int
_worker(int argc, char *argv[]) {
    int i = strlen(argv[0]);

    logs[hits++] = i;
    FEED(0);

    i *= 3;
    logs[hits++] = i;
    FEED(0);

    i += 7;
    logs[hits++] = i;
    return 0;
}


void
test_api() {
    struct pcaio_task *tasks[2];

    tasks[0] = pcaio_task_new((pcaio_taskmain_t)_worker, 1, "foo");
    tasks[1] = pcaio_task_new((pcaio_taskmain_t)_worker, 1, "thud");

    hits = 0;
    memset(logs, 0, sizeof(int) * 6);
    eqint(0, pcaio(NULL, tasks, 2));
    eqint(6, hits);
    eqint(3, logs[0]);
    eqint(4, logs[1]);
    eqint(9, logs[2]);
    eqint(12, logs[3]);
    eqint(16, logs[4]);
    eqint(19, logs[5]);
}


int
main() {
    test_api();
}
