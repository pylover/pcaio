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


static int _hits = 0;
static int _terminates = 0;


static void
_done(struct pcaio_task *) {
    _terminates++;
}


static int
_workerA(const char *word, bool more) {
    struct pcaio_task *t;

    _hits++;
    if (more) {
        t = pcaio_fschedule(_workerA, NULL, 2, "bar", false);
        pcaio_task_callback(t, _done);
    }

    return strlen(word);
}


void
test_api() {
    struct pcaio_task *tasks[2];
    int foostatus;
    int thudstatus;

    tasks[0] = pcaio_task_new(_workerA, &foostatus, 1, "foo");
    tasks[1] = pcaio_task_new(_workerA, &thudstatus, 1, "thud");
    pcaio_task_callback(tasks[0], _done);
    pcaio_task_callback(tasks[1], _done);

    _hits = 0;
    eqint(0, pcaio(1, tasks, 2));
    eqint(4, _hits);

    eqint(3, foostatus);
    eqint(4, thudstatus);
    eqint(4, _terminates);
}


int
main() {
    test_api();
}
