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


// int
// worker(int argc, char *argv[]) {
//     int i = strlen(argv[0]);
//
//     logs[hits++] = i;
//     pcaio_task_relax();
//
//     i *= 3;
//     logs[hits++] = i;
//     pcaio_task_relax();
//
//     i += 7;
//     logs[hits++] = i;
//     return i;
// }


// void
// test_api_advanced() {
//     struct pcaio_task *t;
//
//     eqint(0, pcaio_init(NULL));
//
//     t = pcaio_task_new("w #1", (pcaio_entrypoint_t)worker, 1, "foo");
//     isnotnull(t);
//     eqint(0, pcaio_task_schedule(t));
//
//     t = pcaio_task_new("w #2", (pcaio_entrypoint_t)worker, 1, "thud");
//     isnotnull(t);
//     eqint(0, pcaio_task_schedule(t));
//
//     hits = 0;
//     memset(logs, 0, sizeof(int) * 6);
//     eqint(0, pcaio());
//     eqint(0, pcaio_deinit());
//     eqint(6, hits);
//     eqint(3, logs[0]);
//     eqint(4, logs[1]);
//     eqint(9, logs[2]);
//     eqint(12, logs[3]);
//     eqint(16, logs[4]);
//     eqint(19, logs[5]);
// }


void
test_api_simple() {
    pcaio_init(NULL);

    // isnotnull(pcaio_schedule("w #1", (pcaio_entrypoint_t)worker, 1, "foo"));
    // isnotnull(pcaio_schedule("w #2", (pcaio_entrypoint_t)worker, 1, "thud"));

    hits = 0;
    memset(logs, 0, sizeof(int) * 6);
    eqint(0, pcaio());
    eqint(0, pcaio_deinit());
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
    test_api_simple();
    // test_api_advanced();
}
