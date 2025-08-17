// Copyright 2023 Vahid Mardani
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
#include <stdlib.h>
#include <errno.h>
#include <threads.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "thread.h"


int
thread_new(thread_t *tid, thread_start_t f, void *arg) {
    int status;

    status = thrd_create(tid, f, arg);
    if (status != thrd_success) {
        if (status == thrd_nomem) {
            errno = ENOMEM;
        }
        return -1;
    }

    return 0;
}
