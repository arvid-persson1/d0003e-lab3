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

const uint16_t TICKS_PER_SECOND = 2;
static const uint16_t FREQ = 8000000 / 1024 / TICKS_PER_SECOND;

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

    // PCIE1: enable PCINT(15:8) interrupts.
    EIMSK  = SET(PCIE1);
    // PCIN15: enable PCINT15 interrupt.
    PCMSK1 = SET(PCINT15);
    
    // Part 2 step 2 init:
    // COM1A(1:0): set OC1A on compare match.
    TCCR1A = SET(COM1A1) | SET(COM1A0);
    // WGM1(3:2): CTC mode.
    // CS1(2:0): 1024 prescaling factor.
    TCCR1B = SET(WGM12)  | SET(CS12) | SET(CS10);
    // OCIE1A: output comparison A enabled.
    TIMSK1 = SET(OCIE1A);

    // Interrupts must be disabled when accessing 16-bit registers.
    DISABLE();
    // OCR1A(:): counter comparison A value.
    OCR1A  = FREQ;
    // Reset timer.
    TCNT1  = 0;
    ENABLE();

    initialized = true;
}

// `enqueue` and `dequeue` should probably be renamed.

static void enqueue(const Thread p, Thread * const queue) {
    p->next = *queue;
    *queue = p;
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

        // thread next = dequeue(&readyQ);
        // enqueue(current, &freeQ);
        // dispatch(next);

        enqueue(current, &freeQ);
        dispatch(dequeue(&readyQ));
    }

    SETSTACK(&newp->context, &newp->stack);

    enqueue(newp, &readyQ);

    ENABLE();
}

void yield(void) {
    DISABLE();

    enqueue(current, &readyQ);
    dispatch(dequeue(&readyQ));

    ENABLE();
}

// Joystick input.
ISR(PCINT1_vect) {
    if (!(PINB & SET(PORTB7)))
        yield();
}

// Timer.
ISR(TIMER1_COMPA_vect) {
    // TODO: 
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
