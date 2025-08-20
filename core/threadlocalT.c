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
#include <threads.h>


static tss_t TLSTATICNAME(_storage);


int
TLNAME(_init) (TLNAME(_dtor) dtor) {
    int ret;

    ret = tss_create(&TLSTATICNAME(_storage), (tss_dtor_t)dtor);
    if (ret != thrd_success) {
        return -1;
    }

    return 0;
}


int
TLNAME(_set) (TLTYPE() *val) {
    int ret;

    ret = tss_set(TLSTATICNAME(_storage), val);
    if (ret != thrd_success) {
        return -1;
    }

    return 0;
}


TLTYPE() *
TLNAME(_get) () {
    void *ret;

    ret = tss_get(TLSTATICNAME(_storage));
    if (ret == NULL) {
        return NULL;
    }

    return (TLTYPE()*)ret;
}


void
TLNAME(_delete) () {
    tss_delete(TLSTATICNAME(_storage));
}
