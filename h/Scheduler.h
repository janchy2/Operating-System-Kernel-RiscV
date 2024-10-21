//
// Created by os on 4/16/23.
//

#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "../h/tcb.hpp"

class TCB;

class Scheduler {
public:
    static TCB* first, *last;
    static Scheduler& SchedulerObj();
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    TCB* get();
    void put(TCB* thread);

private:
    Scheduler() = default;
};


#endif //SCHEDULER_H
