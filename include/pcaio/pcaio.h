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
#ifndef INCLUDE_PCAIO_PCAIO_H_
#define INCLUDE_PCAIO_PCAIO_H_


struct pcaio;
struct pcaio_task;
typedef int (*pcaio_entrypoint_t) (int argc, void *argv[]);


/* this structure must be filled by user */
struct pcaio_config {
    unsigned short workers_min;
    unsigned short workers_max;
};


struct pcaio_config *
pcaio_config_default();


int
pcaio_init(struct pcaio_config *config);


int
pcaio_deinit();


int
pcaio_task_schedule(struct pcaio_task *t);


struct pcaio_task *
pcaio_task_newschedule(pcaio_entrypoint_t func, int argc, ...);


struct pcaio_task *
pcaio_task_new(pcaio_entrypoint_t func, int argc, ...);


int
pcaio_task_free(struct pcaio_task *t);


void
pcaio_currenttask_relax();


int
pcaio();


#endif  // INCLUDE_PCAIO_PCAIO_H_
