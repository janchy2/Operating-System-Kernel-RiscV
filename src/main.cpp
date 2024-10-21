#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/K_Console.h"

extern void outputHandler();
extern void userMain();

int main()
{
    //pravi TCB za main
    TCB::shouldBeStarted = false;
    TCB::createTCB(&TCB::running, (void(*)(void*))&main, nullptr, nullptr);
    TCB::running->setPhase(TCB::RUNNING);

    Riscv::w_stvec((uint64) (&Riscv::ivt)|1);

    //pravi kernel nit za konzolu
    TCB::shouldBeStarted = true;
    thread_create(&K_Console::outputThread, (void(*)(void*))&outputHandler, nullptr);

    //pravi korisnicku nit
    TCB* userThread;
    thread_create(&userThread, (void(*)(void*))&userMain, nullptr);


    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);


    while(!userThread->isFinished()) {
        thread_dispatch();
    }

    K_Console::deallocate();
    userThread->deleteStack(); //oslobadja korisnicku nit
    userThread->deleteSemaphore();
    mem_free((void*)userThread);
    K_Console::outputThread->deleteStack(); //oslobadja kernel nit za konzolu
    K_Console::outputThread->deleteSemaphore();
    mem_free((void*)K_Console::outputThread);
    TCB::running->deleteSemaphore(); //oslobadja TCB za main
    mem_free((void*)TCB::running);

    return 0;
}
