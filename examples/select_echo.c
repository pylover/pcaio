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
#include <pcaio/modio.h>
#include <pcaio/modselect.h>


#define CHUNKSIZE 80
#define WORKERS 1


static int
_echo() {
    int ret = 0;
    char buff[CHUNKSIZE];

    printf("write something then press enter.\n");
    for (;;) {
        ret = readA(STDIN_FILENO, buff, CHUNKSIZE);
        if (ret <= 0) {
            break;
        }

        printf("%.*s", ret, buff);
    }

    return ret;
}


int
main() {
    int ret;
    struct pcaio_iomodule *modselect;
    struct pcaio_task *t;

    /* make standard input nonblock */
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);

    /* create a task */
    t = pcaio_task_new(_echo, NULL, 0);

    /* create and register the select module */
    pcaio_modselect_use(4, &modselect);
    pcaio_modio_use(modselect);

    /* main loop */
    ret = pcaio(WORKERS, &t, 1);

    return ret;
}
