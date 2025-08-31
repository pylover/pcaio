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
#ifndef INCLUDE_PCAIO_SELECT_H_
#define INCLUDE_PCAIO_SELECT_H_


/* local public */
#include "pcaio/io.h"


int
pcaio_modselect_use(unsigned short maxfileno);


int
pcaio_modselect_wait(int fd, int events);


int
pcaio_modselect_monitor(struct pcaio_ioevent *e);


#endif  // INCLUDE_PCAIO_SELECT_H_
