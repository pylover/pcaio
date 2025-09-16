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
#include <stdint.h>  // NOLINT
#include <errno.h>

/* posix */
#include <pthread.h>


void
LNAME(list_init) (struct LNAME(list) *q) {
    pthread_mutex_init(&q->mutex, NULL);
}


void
LNAME(list_deinit) (struct LNAME(list) *q) {
    pthread_mutex_destroy(&q->mutex);
}


int
LNAME(list_append) (struct LNAME(list) *q, LTYPE() *v) {
    if (v->LNAME(list_prev) || v->LNAME(list_next)) {
        return -1;
    }

    pthread_mutex_lock(&q->mutex);
    if (q->tail == NULL) {
        /* empty list */
        q->head = v;
        q->tail = v;
    }
    else {
        v->LNAME(list_prev) = q->tail;
        q->tail->LNAME(list_next) = v;
        q->tail = v;
    }

    q->count++;
    pthread_mutex_unlock(&q->mutex);
    return 0;
}


int
LNAME(list_delete) (struct LNAME(list) *q, LTYPE() *v) {
    if (v == NULL) {
        return -1;
    }

    pthread_mutex_lock(&q->mutex);

    if ((v->LNAME(list_prev) == NULL) && (v->LNAME(list_next) == NULL)) {
        if ((q->head != v) || (q->tail != v)) {
            goto failed;
        }

        q->head = NULL;
        q->tail = NULL;
    }
    else if (v->LNAME(list_next) == NULL) {
        if (q->tail != v) {
            goto failed;
        }

        q->tail = v->LNAME(list_prev);
        q->tail->LNAME(list_next) = NULL;
        v->LNAME(list_prev) = NULL;
    }
    else if (v->LNAME(list_prev) == NULL) {
        if (q->head != v) {
            goto failed;
        }

        q->head = v->LNAME(list_next);
        q->head->LNAME(list_prev) = NULL;
        v->LNAME(list_next) = NULL;
    }
    else {
        v->LNAME(list_next)->LNAME(list_prev) = v->LNAME(list_prev);
        v->LNAME(list_prev)->LNAME(list_next) = v->LNAME(list_next);
        v->LNAME(list_prev) = NULL;
        v->LNAME(list_next) = NULL;
    }

    q->count--;
    pthread_mutex_unlock(&q->mutex);
    return 0;

failed:
    pthread_mutex_unlock(&q->mutex);
    return -1;
}


// int
// LNAME(_pushall) (struct LNAME() *q, T *v[], size_t count) {
//     int i;
//
//     /* some guards */
//     if (count == 0) {
//         return -1;
//     }
//
//     for (i = 0; i < count; i++) {
//         if (v[i]->LNAME(_next) || (v[i] == q->tail)) {
//             return -1;
//         }
//     }
//
//     /* take the ownership */
//     pthread_mutex_lock(&q->mutex);
//
//     if (q->tail == NULL) {
//         /* list is empty */
//         q->tail = v[0];
//         q->head = v[0];
//     }
//
//     /* chain the rest */
//     for (i = 1; i < count; i++) {
//         q->tail->LNAME(_next) = v[i];
//         q->tail = v[i];
//     }
//
//     q->tail->LNAME(_next) = NULL;
//
//     pthread_cond_broadcast(&q->condition);
//     pthread_mutex_unlock(&q->mutex);
//     return 0;
// }
//
//
// int
// LNAME(_pop) (struct LNAME() *q, T **out, int flags) {
//     int ret = 0;
//     T *o;
//
//     pthread_mutex_lock(&q->mutex);
//     if (flags & QWAIT) {
//         while (q->head == NULL) {
//             if (pthread_cond_wait(&q->condition, &q->mutex)) {
//                 ret = -1;
//                 goto done;
//             }
//         }
//     }
//     else if (q->head == NULL) {
//         ret = -1;
//         goto done;
//     }
//
//     o = q->head;
//     if (o->LNAME(_next) == NULL) {
//         q->head = NULL;
//         q->tail = NULL;
//     }
//     else {
//         q->head = o->LNAME(_next);
//         o->LNAME(_next) = NULL;
//     }
//
//     *out = o;
//
// done:
//     pthread_mutex_unlock(&q->mutex);
//     return ret;
// }
