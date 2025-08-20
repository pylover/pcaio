/* standard */
#include <ucontext.h>

/* local private */
#include "context.h"
#undef T
#define T ucontext
#include "threadlocalT.c"
