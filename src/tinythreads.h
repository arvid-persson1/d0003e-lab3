#ifndef _TINYTHREADS_H
#define _TINYTHREADS_H

#include <stdbool.h>

#define SET(x) (1 << x)

struct ThreadBlock;
typedef struct ThreadBlock *Thread;
void spawn(void (* const function)(int), const int arg);
void yield(void);

typedef struct Mutex {
    bool locked;
    Thread waitQ;
} Mutex;

#define MUTEX_INIT {0, 0}
void lock(Mutex * const m);
void unlock(Mutex * const m);

#endif

