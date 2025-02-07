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

Mutex blinkMutex = { true, 0 };

void blink(__attribute__((unused)) int _x) {
    while (true) {
        lock(&blinkMutex);
        LCDDR3 ^= 1;
    }
}

Mutex buttonMutex = { true, 0 };

void button(const int pos) {
    LCDDR13 ^= 1;
    uint16_t presses = 0;

    while(true) {
        lock(&buttonMutex);
        if (!(PINB & SET(PINB7))) {
            LCDDR13 ^= 1;
            LCDDR18 ^= 1;
            printAt(++presses, pos);
        }
    }
}

// Timer.
ISR(TIMER1_COMPA_vect) {
    unlock(&blinkMutex);
}

// Joystick input.
ISR(PCINT1_vect) {
    unlock(&buttonMutex);
}

int main() {
    initClk();
    initLcd();
    initTimer();
    initButton();
    setTimerInterrupt();
    setButtonInterrupt();

    spawn(blink, 0);
    spawn(button, 4);
    primes(0);
}
