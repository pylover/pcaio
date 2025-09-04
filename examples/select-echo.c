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
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>

/* thirdparty */
#include <clog.h>

/* pcaio */
#include <pcaio/pcaio.h>
#include <pcaio/select.h>


#define CHUNKSIZE 80


static int
_echo(int argc, void *argv[]) {
    int ret = 0;
    int bytes;
    char c[CHUNKSIZE];

    printf("write something then press enter.\n");
    for (;;) {
        bytes = read(STDIN_FILENO, c, CHUNKSIZE);
        if (bytes > 0) {
            printf("%.*s", bytes, c);
            continue;
        }

        if (bytes == 0) {
            /* end of file */
            break;
        }

        if (!IO_MUSTWAIT(errno)) {
            ERROR("read(2)");
            ret = -1;
            break;
        }

        errno = 0;
        if (pcaio_modselect_wait(STDIN_FILENO, IOREAD)) {
            ERROR("pcaio_modselect_wait");
            ret = -1;
            break;
        }
    }

    return ret;
}


int
main() {
    int ret;
    struct pcaio_task *t;
    struct pcaio_config c = {
        .workers_min = 1,
        .workers_max = 1,
    };

    /* make standard input nonblock */
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);

    /* create a task */
    t = pcaio_task_new(_echo, 0);

    /* register the select module */
    pcaio_modselect_use(4);

    /* main loop */
    ret = pcaio(&c, &t, 1);

    return ret;
}
