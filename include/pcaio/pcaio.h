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


#define FEED(f) pcaio_feed(f)


enum taskflags {
    TASK_TERMINATED = 0x1,
    TASK_NOSCHEDULE = 0x2,
};


enum moduleflags {
    MOD_PANIC = 0x1,
};


struct pcaio;
struct pcaio_task;
struct pcaio_module;
typedef int (*pcaio_taskmain_t) (int argc, void *argv[]);


struct pcaio_module {
    const char *name;
    int flags;
    int (*init)();
    int (*dtor)();
    int (*tick)(unsigned int timeout_us);
};


/* this structure must be provided by the user */
struct pcaio_config {
    unsigned short workers_min;
    unsigned short workers_max;
};


int
pcaio_fschedule(pcaio_taskmain_t func, int argc, ...);


struct pcaio_task *
pcaio_task_new(pcaio_taskmain_t func, int argc, ...);


int
pcaio_task_free(struct pcaio_task *t);


int
pcaio_schedule(struct pcaio_task *t);


struct pcaio_task *
pcaio_self();


int
pcaio_feed(int flags);


int
pcaio_module_install(struct pcaio_module *m);


int
pcaio(struct pcaio_config *c, struct pcaio_task *tasks[],
        unsigned short count);


#endif  // INCLUDE_PCAIO_PCAIO_H_
