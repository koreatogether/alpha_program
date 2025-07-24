#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <Arduino.h>
#include "config.h"

class Communicator
{
public:
    void begin();
    void print(const char *message);
    void println(const char *message);
    void print(String message);
    void println(String message);
    void println();
    int readUserInput(unsigned long timeout);
    String readStringInput(unsigned long timeout);
    bool isSerialAvailable();
    String readStringUntil(char terminator);
    String readNonBlockingLine(); // New non-blocking function
    void clearInputBuffer();
};

#endif // COMMUNICATOR_H
