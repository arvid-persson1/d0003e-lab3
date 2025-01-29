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

void blink(const int16_t freq) {
    int16_t last = TCNT1, acc = 0;

    while (true) {
        int16_t time = TCNT1,
                diff = time - last;
        last = time;
        acc += diff;

        if (acc >= freq) {
            LCDDR3 ^= 1;
            acc -= freq;
        }
    }
}

void button(__attribute__((unused)) int _x) {
    LCDDR13 ^= 1;
    bool state = false;

    while(true) {
        if (PINB & SET(PINB7)) {
            state = true;
        } else if (state) {
            LCDDR13 ^= 1;
            LCDDR18 ^= 1;
            state = false;
        }
    }
}

int main() {
    spawn(blink, 0);
    spawn(button, 4);
    primes(0);
}
