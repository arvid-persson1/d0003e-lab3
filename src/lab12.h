#ifndef _LAB12_H
#define _LAB12_H

#include <stdbool.h>

void initClk(void);
void initLcd(void);
void initTimer(void);
void initButton(void);

void setTimerInterrupt(void);
void setButtonInterrupt(void);

void clearChar(const int);
void writeChar(const char, const int);
void printAt(const long, const int);

bool isPrime(const unsigned long);

#endif
