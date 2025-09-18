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
#ifndef INCLUDE_PCAIO_TIMER_H_
#define INCLUDE_PCAIO_TIMER_H_


/* system */
#include <sys/time.h>

/* local public */
#include "pcaio/pcaio.h"


struct pcaio_timer;


struct pcaio_timer *
pcaio_timer_new(struct pcaio_iomodule *iomod);


int
pcaio_timer_free(struct pcaio_timer *t);


int
sleepA_ms(struct pcaio_timer *t, time_t ms);


#endif  // INCLUDE_PCAIO_TIMER_H_
