#include "tinythreads.h"
#include "lab12.h"
#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

void primes(void) {
    unsigned long i = 1;
    while (true) {
        if (isPrime(i))
            printAt(i, 0);
        i++;
    }
}

void blink(int targetTicks) {
    targetTicks /= 2;
    while (true) {
        while (timerRead() < targetTicks);
        timerReset();
        LCDDR3 ^= 1;
    }
}

void button(int presses) {
    LCDDR13 ^= 1;
    bool state = false;

    while(true) {
        if (PINB & SET(PINB7)) {
            state = true;
        } else if (state) {
            LCDDR13 ^= 1;
            LCDDR18 ^= 1;
            printAt(++presses, 4);
            state = false;
        }
    }
}

int main() {
    initClk();
    initLcd();
    initTimer();
    initButton();
    setButtonInterrupt();
    
    spawn(blink, TICKS_PER_SECOND);
    spawn(button, 0);
    primes();
}
