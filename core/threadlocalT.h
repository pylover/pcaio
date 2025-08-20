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
#ifndef CORE_THREADLOCALT_H_
#define CORE_THREADLOCALT_H_


/* standard */
#include <threads.h>


/* generic stuff (must included once) */
#define TLNAME_PASTER2(x, y) x ## y
#define TLNAME_EVALUATOR2(x, y)  TLNAME_PASTER2(x, y)
#define TLNAME_PASTER3(x, y, z) x ## y ## z
#define TLNAME_EVALUATOR3(x, y, z)  TLNAME_PASTER3(x, y, z)


#define TLNAME(n) TLNAME_EVALUATOR3(threadlocal, T, n)
#define TLSTATICNAME(n) TLNAME_EVALUATOR3(_, T, n)
#define TLTYPE() TLNAME_EVALUATOR2(T, _t)


#endif  // CORE_THREADLOCALT_H_


typedef void (*TLNAME(_dtor))(TLTYPE() *val);


int
TLNAME(_init) (TLNAME(_dtor) dtor);


int
TLNAME(_set) (TLTYPE() *val);


TLTYPE() *
TLNAME(_get) ();


void
TLNAME(_deinit) ();
