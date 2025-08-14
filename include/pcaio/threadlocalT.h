#ifndef CORE_THREADLOCALT_H_
#define CORE_THREADLOCALT_H_


/* generic stuff (must included once) */
#define TL_NAME_PASTER2(x, y) x ## y
#define TL_NAME_EVALUATOR2(x, y)  TL_NAME_PASTER2(x, y)
#define TL_NAME_PASTER3(x, y, z) x ## y ## z
#define TL_NAME_EVALUATOR3(x, y, z)  TL_NAME_PASTER3(x, y, z)


#define TL_NAME(n) TL_NAME_EVALUATOR3(threadlocal, TL, n)
#define TL_TYPE() TL_NAME_EVALUATOR2(TL, _t)


#endif  // CORE_THREADLOCALT_H_


void
TL_NAME(_set) (TL_TYPE() *val);


TL_TYPE() *
TL_NAME(_get) ();
