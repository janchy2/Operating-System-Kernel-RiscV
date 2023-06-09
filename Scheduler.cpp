//
// Created by os on 4/16/23.
//

#include "../h/Scheduler.h"

TCB* Scheduler::first = nullptr;
TCB* Scheduler::last = nullptr;

Scheduler &Scheduler::SchedulerObj() {
    static Scheduler instance;
    return instance;
}

TCB *Scheduler::get() {
    TCB::timeSliceCounter = 0; //desava se promena konteksta, znaci resetuje se tajmer
    while(!first->isReady()) { //ako je neka nit u medjuvremenu blokirana
        TCB* prev = first;
        first = first->getNextInScheduler();
        prev->setNextInScheduler(nullptr);
    }
    TCB* oldFirst = first;
    first = first->getNextInScheduler();
    oldFirst->setNextInScheduler(nullptr);
    if(!first) last = nullptr;
    return oldFirst;
}

void Scheduler::put(TCB *thread) {
    if(!last) first = thread;
    else last->setNextInScheduler(thread);
    last = thread;
    thread->setNextInScheduler(nullptr);
    thread->setPhase(TCB::READY);
}


