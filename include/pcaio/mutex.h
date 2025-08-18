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
#ifndef INCLUDE_PCAIO_MUTEX_H_
#define INCLUDE_PCAIO_MUTEX_H_


/* standard */
#include <threads.h>


typedef mtx_t mutex_t;


int
mutex_init(mutex_t *m);


void
mutex_deinit(mutex_t *m);


void
mutex_acquire(mutex_t *m);


void
mutex_release(mutex_t *m);


#endif  // INCLUDE_PCAIO_MUTEX_H_
