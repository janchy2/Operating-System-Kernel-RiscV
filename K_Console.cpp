//
// Created by os on 5/26/23.
//

#include "../h/K_Console.h"
#include "../h/syscall_c.hpp"

TCB* K_Console::outputThread = nullptr;
bool K_Console::isMade = false;

K_Console K_Console::ConsoleObj() {
    static K_Console instance;
    if(!isMade) {
        instance.inputBuffer = Buffer::createBuffer();
        instance.outputBuffer = Buffer::createBuffer();
        K_Semaphore::createSemaphore(&instance.inputItemAvailable, 0);
        K_Semaphore::createSemaphore(&instance.outputItemAvailable, 0);
        isMade = true;
    }
    return instance;
}


void outputHandler() {

    while(true) {
        while((*(char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT) {
            sem_wait(K_Console::ConsoleObj().outputItemAvailable); //da li ima znakova
            char data = K_Console::ConsoleObj().outputBuffer->take();
            *(char*)CONSOLE_TX_DATA = data;
        }
    }
}

void K_Console::deallocate() {
    mem_free((void*)ConsoleObj().inputBuffer);
    mem_free((void*)ConsoleObj().outputBuffer);
    mem_free((void*)ConsoleObj().outputItemAvailable);
    mem_free((void*)ConsoleObj().inputItemAvailable);
}