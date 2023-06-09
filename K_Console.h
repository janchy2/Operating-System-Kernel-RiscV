//
// Created by os on 5/26/23.
//

#ifndef K_CONSOLE_H
#define K_CONSOLE_H
#include "K_Semaphore.h"
#include "tcb.hpp"
#include "Buffer.h"


void outputHandler();

class K_Console {

public:
    static TCB* outputThread;

    K_Semaphore* outputItemAvailable;
    K_Semaphore* inputItemAvailable;

    Buffer* outputBuffer;
    Buffer* inputBuffer;

    static K_Console ConsoleObj();
    static bool isMade;
    static void deallocate();

};


#endif //K_CONSOLE_H
