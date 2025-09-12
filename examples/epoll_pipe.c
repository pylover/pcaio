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
#include <pcaio/modepoll.h>


#define CHUNKSIZE 64
#define WORKERS 2


static int
_producer(int argc, int argv[]) {
    int i;
    int j;
    int rfd;
    int fd = argv[0];
    int ret = 0;
    unsigned char buff[CHUNKSIZE];

    rfd = open("/dev/urandom", O_NONBLOCK, O_RDONLY);
    if (rfd == -1) {
        return -1;
    }

    for (i = 0; i < 16; i++) {
        ret = await_read(rfd, buff, CHUNKSIZE);
        if (ret <= 0) {
            break;
        }

        /* encode */
        for (j = 0; j < ret; j++) {
            buff[j] = (buff[j] % 26) + (buff[j] & 0x80? 97: 65);
        }

        ret = await_write(fd, buff, ret);
        if (ret <= 0) {
            break;
        }
    }

    close(fd);
    close(rfd);
    return ret > 0? 0: ret;
}


static int
_consumer(int argc, int argv[]) {
    int fd = argv[0];
    int ret;
    char buff[CHUNKSIZE];

    for (;;) {
        ret = await_read(fd, buff, CHUNKSIZE);
        if (ret <= 0) {
            break;
        }

        printf("%.*s\n", ret, buff);
    }

    close(fd);
    return ret;
}


int
main() {
    int ret;
    int q[2];
    struct pcaio_iomodule *modepoll;
    struct pcaio_task *t[2];

    /* create a pipe */
    if (pipe2(q, O_NONBLOCK | O_DIRECT)) {
        ret = -1;
        goto done;
    }

    /* create a task */
    t[0] = pcaio_task_new((pcaio_taskmain_t)_consumer, 1, q[0]);
    t[1] = pcaio_task_new((pcaio_taskmain_t)_producer, 1, q[1]);

    /* register the epoll module */
    pcaio_modepoll_use(4, &modepoll);
    pcaio_modio_use(modepoll);

    /* main loop */
    ret = pcaio(WORKERS, t, 2);

done:
    return ret;
}
