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
#ifndef INCLUDE_PCAIO_THREADLOCALT_H_
#define INCLUDE_PCAIO_THREADLOCALT_H_


/* generic stuff (must included once) */
#define TL_NAME_PASTER2(x, y) x ## y
#define TL_NAME_EVALUATOR2(x, y)  TL_NAME_PASTER2(x, y)
#define TL_NAME_PASTER3(x, y, z) x ## y ## z
#define TL_NAME_EVALUATOR3(x, y, z)  TL_NAME_PASTER3(x, y, z)


#define TL_NAME(n) TL_NAME_EVALUATOR3(threadlocal, TL, n)
#define TL_TYPE() TL_NAME_EVALUATOR2(TL, _t)


#endif  // INCLUDE_PCAIO_THREADLOCALT_H_


void
TL_NAME(_set) (TL_TYPE() *val);


TL_TYPE() *
TL_NAME(_get) ();
