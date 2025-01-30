#include "tinythreads.h"
#include "lab12.h"
#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void primes(const int pos) {
    unsigned long i = 1;
    while (true) {
        if (isPrime(i))
            printAt(i, pos);
        i++;
    }
}

void blink(__attribute__((unused)) int _x) {
    LCDDR3 ^= 1;
}

static bool state = false;
static uint16_t presses = 0;

void button(const int pos) {
    if (PINB & SET(PINB7)) {
        state = true;
    } else if (state) {
        LCDDR13 ^= 1;
        LCDDR18 ^= 1;
        printAt(++presses, pos);
        state = false;
    }
}

// Timer.
ISR(TIMER1_COMPA_vect) {
    spawn(blink, 0);
}

// Joystick input.
ISR(PCINT1_vect) {
    spawn(button, 4);
}

int main() {
    initClk();
    initLcd();
    initTimer();
    initButton();
    setTimerInterrupt();
    setButtonInterrupt();

    LCDDR13 ^= 1;

    primes(0);
}
