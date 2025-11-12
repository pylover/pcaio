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
static int _logs[6];
static int _terminates = 0;


static void
_done(struct pcaio_task *) {
    _terminates++;
}


static int
_workerA(const char *word) {
    int l = strlen(word);
    int i = l;

    _logs[_hits++] = i;
    pcaio_relaxA(0);

    i *= 3;
    _logs[_hits++] = i;
    pcaio_relaxA(0);

    i += 7;
    _logs[_hits++] = i;
    return l;
}


void
test_api() {
    struct pcaio_task *tasks[2];
    int foostatus;
    int thudstatus;

    tasks[0] = pcaio_task_new(_workerA, &foostatus, _done,
            1, "foo");
    tasks[1] = pcaio_task_new(_workerA, &thudstatus, _done,
            1, "thud");

    _hits = 0;
    memset(_logs, 0, sizeof(int) * 6);
    eqint(0, pcaio(1, tasks, 2));
    eqint(6, _hits);
    eqint(3, _logs[0]);
    eqint(4, _logs[1]);
    eqint(9, _logs[2]);
    eqint(12, _logs[3]);
    eqint(16, _logs[4]);
    eqint(19, _logs[5]);

    eqint(3, foostatus);
    eqint(4, thudstatus);
    eqint(2, _terminates);
}


int
main() {
    test_api();
}
