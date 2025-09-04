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


#define CHUNK 64


static int
_producer(int argc, int argv[]) {
    int i;
    int j;
    int bytes;
    int rfd;
    int fd = argv[0];
    int ret = 0;
    int res;
    unsigned char tmp[CHUNK];

    rfd = open("/dev/urandom", O_NONBLOCK, O_RDONLY);
    if (rfd == -1) {
        return -1;
    }

    for (i = 0; i < 16; i++) {
retryr:
        bytes = read(rfd, tmp, CHUNK);
        if ((bytes == -1) && IO_MUSTWAIT(errno)) {
            errno = 0;
            pcaio_modselect_wait(rfd, IOREAD);
            goto retryr;
        }

        if (bytes == -1) {
            ERROR("read(2)");
            ret = -1;
            break;
        }

        /* encode */
        for (j = 0; j < bytes; j++) {
            if (tmp[j] > 127) {
                tmp[j] %= 26;
                tmp[j] += 97;
            }
            else {
                tmp[j] %= 26;
                tmp[j] += 65;
            }
        }

retryw:
        res = write(fd, tmp, bytes);
        if ((res == -1) && (IO_MUSTWAIT(errno))) {
            pcaio_modselect_wait(fd, IOWRITE);
            goto retryw;
        }

        if (res == -1) {
            ERROR("write(2)");
            ret = -1;
            break;
        }

        pcaio_relax(0);
    }

    close(fd);
    close(rfd);
    return ret;
}


static int
_consumer(int argc, int argv[]) {
    int fd = argv[0];
    int ret = 0;
    int bytes;
    char buff[CHUNK];

    for (;;) {
        bytes = read(fd, buff, CHUNK);
        if ((bytes == -1) && IO_MUSTWAIT(errno)) {
            errno = 0;
            pcaio_modselect_wait(fd, IOREAD);
            continue;
        }

        if (bytes == -1) {
            ERROR("read(2)");
            ret = -1;
            break;
        }

        if (bytes == 0) {
            /* end of file */
            break;
        }

        printf("%.*s\n", bytes, buff);
        pcaio_relax(0);
    }

    close(fd);
    return ret;
}


int
main() {
    int ret;
    int q[2];
    struct pcaio_task *t[2];
    struct pcaio_config c = {
        .workers_min = 2,
        .workers_max = 2,
    };

    /* create a pipe */
    if (pipe2(q, O_NONBLOCK | O_DIRECT)) {
        ret = -1;
        goto done;
    }

    /* create a task */
    t[0] = pcaio_task_new((pcaio_taskmain_t)_consumer, 1, q[0]);
    t[1] = pcaio_task_new((pcaio_taskmain_t)_producer, 1, q[1]);

    /* register the select module */
    pcaio_modselect_use(4);

    /* main loop */
    ret = pcaio(&c, t, 2);

done:
    return ret;
}
