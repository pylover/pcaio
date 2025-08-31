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
#include <stdlib.h>
#include <unistd.h>

/* thirdparty */
#include <clog.h>

/* pcaio */
#include <pcaio/pcaio.h>
#include <pcaio/select.h>


#define CHUNKSIZE 16


static int
_urandom(int argc, void *argv[]) {
    int i;
    int ret = 0;
    int bytes;
    int fd = -1;
    char buff[CHUNKSIZE];
    unsigned short *val = (unsigned short *)buff;

    fd = open("/dev/urandom", O_RDONLY | O_NONBLOCK);

    for (i = 0; i < 10; i++) {
        if (pcaio_modselect_wait(fd, IOREAD)) {
            ERROR("pcaio_waitfd");
        }
        bytes = read(fd, buff, CHUNKSIZE);
        if (bytes == -1) {
            ERROR("read(2)");
            ret = -1;
            break;
        }

        if (bytes == 0) {
            break;
        }

        INFO("urandom: %u", *val);
    }

    close(fd);
    return ret;
}


int
main() {
    struct pcaio_task *t;
    struct pcaio_config c = {
        .workers_min = 1,
        .workers_max = 1,
    };
    pcaio_modselect_use(16);
    t = pcaio_task_new(_urandom, 0);
    return pcaio(&c, &t, 1);
}
