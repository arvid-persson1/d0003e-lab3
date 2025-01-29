#ifndef _TINYTHREADS_H
#define _TINYTHREADS_H

#include <stdbool.h>
#include <stdint.h>

#define SET(x) (1 << x)

struct ThreadBlock;
typedef struct ThreadBlock *Thread;
void spawn(void (* const function)(int), const int arg);
void yield(void);

extern const uint16_t TICKS_PER_SECOND;
uint_fast8_t timerRead(void);
void timerReset(void);

typedef struct Mutex {
    bool locked;
    Thread waitQ;
} Mutex;

#define MUTEX_INIT {0, 0}
void lock(Mutex * const m);
void unlock(Mutex * const m);

#endif

