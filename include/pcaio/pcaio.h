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
#ifndef INCLUDE_PCAIO_PCAIO_H_
#define INCLUDE_PCAIO_PCAIO_H_


struct pcaio;
struct pcaio_task;
typedef int (*pcaio_entrypoint_t) (int argc, void *argv[]);


/* this structure must be filled by user */
struct pcaio_config {
    size_t taskqueue_size;
    unsigned char workers;
};


struct pcaio *
pcaio_new(struct pcaio_config *config);


int
pcaio_free();


int
pcaio();


struct pcaio_task *
pcaio_schedule(const char *id, pcaio_entrypoint_t func, int argc, ...);


struct pcaio_task *
pcaio_task_new(const char *id, pcaio_entrypoint_t func, int argc, ...);


int
pcaio_task_schedule(struct pcaio_task *t);


void
pcaio_task_relax();


#endif  // INCLUDE_PCAIO_PCAIO_H_
