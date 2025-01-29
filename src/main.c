#include "tinythreads.h"
#include "lab12.h"
#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

void primes(const int pos) {
    unsigned long i = 1;
    while (true) {
        if (isPrime(i))
            printAt(i, pos);
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

void button(const int pos) {
    LCDDR13 ^= 1;
    bool state = false;
    uint16_t presses = 0;

    while(true) {
        if (PINB & SET(PINB7)) {
            state = true;
        } else if (state) {
            LCDDR13 ^= 1;
            LCDDR18 ^= 1;
            printAt(++presses, pos);
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
    spawn(button, 4);
    primes(0);
}
