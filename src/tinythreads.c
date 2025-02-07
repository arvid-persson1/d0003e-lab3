#include "tinythreads.h"
#include "lab12.h"
#include <setjmp.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define NULL            0
#define DISABLE()       cli()
#define ENABLE()        sei()
#define STACKSIZE       80
#define NTHREADS        4
#define SETSTACK(buf, a) *((unsigned int*)(buf) + 8) = (unsigned int)(a) + STACKSIZE - 4; \
                         *((unsigned int*)(buf) + 9) = (unsigned int)(a) + STACKSIZE - 4

struct ThreadBlock {
    void (*function)(int);
    int arg;
    Thread next;
    jmp_buf context;
    char stack[STACKSIZE];
};

struct ThreadBlock threads[NTHREADS],
                   initp;

Thread freeQ   = threads,
       readyQ  = NULL,
       current = &initp;

bool initialized = false;

static void initialize(void) {
    for (int i = 0; i < NTHREADS - 1; i++)
        threads[i].next = &threads[i + 1];

    threads[NTHREADS - 1].next = NULL;

    initialized = true;
}

static void enqueue(const Thread p, Thread * const queue) {
    if (*queue == NULL) {
        p->next = NULL;
        *queue = p;
    } else {
        Thread q = *queue;
        *queue = p;
        p->next = q;
    }
}

static Thread dequeue(Thread * const queue) {
    Thread p = *queue;

    if (*queue)
        *queue = (*queue)->next;
    else
        // Empty queue-- panic.
        while (true);

    return p;
}

static void dispatch(const Thread next) {
    if (setjmp(current->context) == 0) {
        current = next;
        longjmp(next->context, 1);
    }
}

void spawn(void (* const function)(const int), const int arg) {
    Thread newp;

    DISABLE();

    if (!initialized)
        initialize();

    newp = dequeue(&freeQ);
    newp->function = function;
    newp->arg = arg;
    newp->next = NULL;

    if (setjmp(newp->context) == 1) {
        ENABLE();
        current->function(current->arg);
        DISABLE();

        // Thread next = dequeue(&readyQ);
        // enqueue(current, &freeQ);
        // dispatch(next);

        enqueue(current, &freeQ);
        dispatch(dequeue(&readyQ));
    }

    SETSTACK(&newp->context, &newp->stack);

    enqueue(newp, &readyQ);
    yield();

    ENABLE();
}

void yield(void) {
    Thread t = dequeue(&freeQ);
    enqueue(current, &readyQ);
    dispatch(t);
}

void lock(Mutex * const m) {
    DISABLE();
    
    if (m->locked) {
        enqueue(current, &m->waitQ);
        dispatch(dequeue(&readyQ));
    } else {
        m->locked = true;
    }

    ENABLE();
}

void unlock(Mutex * const m) {
    DISABLE();

    if (m->waitQ) {
        enqueue(current, &readyQ);
        dispatch(dequeue(&m->waitQ));
    } else {
        m->locked = false;
    }

    ENABLE();
}
