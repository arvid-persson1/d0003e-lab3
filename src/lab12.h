#ifndef _LAB12_H
#define _LAB12_H

#include <stdbool.h>

void initClk(void);
void initLcd(void);
void initTimer(void);
void initButton(void);

void clearChar(const int pos);
void writeChar(const char ch, const int pos);
void printAt(const long num, const int pos);

bool isPrime(const unsigned long n);

#endif
