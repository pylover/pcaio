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
#include <stdio.h>  // NOLINT
#include <errno.h>

/* posix */
#include <pthread.h>


static pthread_key_t
TLSTATICNAME(_storage);


int
TLNAME(_init) (TLNAME(_dtor) dtor) {
    int err;

    err = pthread_key_create(&TLSTATICNAME(_storage),
            (void (*)(void *))dtor);
    if (err) {
        errno = err;
        return -1;
    }

    return 0;
}


int
TLNAME(_set) (TLTYPE() *val) {
    int err;

    err = pthread_setspecific(TLSTATICNAME(_storage), val);
    if (err) {
        errno = err;
        return -1;
    }

    return 0;
}


TLTYPE() *
TLNAME(_get) () {
    void *obj;

    obj = pthread_getspecific(TLSTATICNAME(_storage));
    if (obj == NULL) {
        return NULL;
    }

    return (TLTYPE()*)obj;
}


void
TLNAME(_delete) () {
    pthread_key_delete(TLSTATICNAME(_storage));
}
