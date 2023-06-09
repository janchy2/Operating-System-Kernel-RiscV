#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/interrupt_const.h"
#include "../h/K_Console.h"
#include "../lib/console.h"

void Riscv::popSppSpie()
{
    //prelazimo u sistemski rezim ako je main ili kernel nit za konzolu
    if(TCB::running->myId == 0 || TCB::running->myId == 1) ms_sstatus(SSTATUS_SPP);
    else mc_sstatus(SSTATUS_SPP);

    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap1() {
    uint64 scause = r_scause();
    uint64 code;
    __asm__ volatile("ld %[variable], 80(s0)" : [variable] "=r"(code)); //citanje koda sistemskog poziva iz a0

    if (scause == ECALL_USR || scause == ECALL_SYS) {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        uint64 returnValue = 0;

        if (code == MEM_ALLOC) { ///////////////////////////////////

            uint64 numOfBlocks;
            __asm__ volatile("ld %[variable], 88(s0)" : [variable] "=r"(numOfBlocks)); //citanje argumenta iz a1
            void *allocated = MemoryAllocator::MemAllocObj().k_mem_alloc(numOfBlocks);
            returnValue = (uint64) allocated;

            __asm__ volatile("sd %[value], 80(s0)" : : [value] "r"(returnValue));
            //upis povratne vrednosti u a0

        }

        else if (code == MEM_FREE) { ///////////////////////////////

            uint64 address;
            __asm__ volatile("ld %[variable], 88(s0)" : [variable] "=r"(address)); //citanje argumenta
            returnValue = (uint64) MemoryAllocator::MemAllocObj().k_mem_free((void *) address);

            __asm__ volatile("sd %[value], 80(s0)" : : [value] "r"(returnValue));
            //upis povratne vrednosti u a0

        }

        else if (code == THREAD_CREATE) { ////////////////////////////

            uint64 argument;
            __asm__ volatile("ld %[variable], 88(s0)" : [variable] "=r"(argument)); //citanje argumenta
            TCB **handle = (TCB **) argument;
            __asm__ volatile("ld %[variable], 96(s0)" : [variable] "=r"(argument)); //citanje argumenta
            void (*start_routine)(void *) = (void (*)(void *)) argument;
            __asm__ volatile("ld %[variable], 104(s0)" : [variable] "=r"(argument)); //citanje argumenta
            void *arg = (void *) argument;
            __asm__ volatile("ld %[variable], 112(s0)" : [variable] "=r"(argument)); //citanje argumenta
            void *stack_space = (void *) argument;

            returnValue = (uint64) TCB::createTCB(handle, start_routine, arg, stack_space); //pravi novu nit

            __asm__ volatile("sd %[value], 80(s0)" : : [value] "r"(returnValue));
            //upis povratne vrednosti u a0

        }

        else if (code == THREAD_EXIT) { //////////////////////////////
            returnValue = (uint64) TCB::exit();
            if (!returnValue) { //ako exit nije uspeo
                __asm__ volatile("sd %[value], 80(s0)" : : [value] "r"(returnValue));
                //upis povratne vrednosti u a0
            }

        }

        else if (code == THREAD_DISPATCH) { ///////////////////////////

            TCB::dispatch();
        }


        else if (code == THREAD_JOIN) { //////////////////////////////

            uint64 argument;
            __asm__ volatile("ld %[variable], 88(s0)" : [variable] "=r"(argument)); //citanje argumenta
            TCB *thread = (TCB *) argument;

            TCB::join(thread);

        }

        else if (code == SEM_OPEN) { /////////////////////////////////

            uint64 argument;
            __asm__ volatile("ld %[variable], 88(s0)" : [variable] "=r"(argument)); //citanje argumenta
            K_Semaphore **handle = (K_Semaphore **) argument;
            __asm__ volatile("ld %[variable], 96(s0)" : [variable] "=r"(argument)); //citanje argumenta
            unsigned value = (unsigned) argument;

            returnValue = (uint64) K_Semaphore::createSemaphore(handle, value);

            __asm__ volatile("sd %[value], 80(s0)" : : [value] "r"(returnValue));
            //upis povratne vrednosti u a0

        }

        else if (code == SEM_CLOSE) { ///////////////////////////////

            uint64 argument;
            __asm__ volatile("ld %[variable], 88(s0)" : [variable] "=r"(argument)); //citanje argumenta
            K_Semaphore *handle = (K_Semaphore *) argument;
            returnValue = (uint64) K_Semaphore::closeSemaphore(handle);

            __asm__ volatile("sd %[value], 80(s0)" : : [value] "r"(returnValue));
            //upis povratne vrednosti u a0

        }

        else if (code == SEM_WAIT) { ////////////////////////////////

            uint64 argument;
            __asm__ volatile("ld %[variable], 88(s0)" : [variable] "=r"(argument)); //citanje argumenta
            K_Semaphore *handle = (K_Semaphore *) argument;

            returnValue = (uint64) handle->wait();

            __asm__ volatile("sd %[value], 80(s0)" : : [value] "r"(returnValue));
            //upis povratne vrednosti u a0

        }

        else if (code == SEM_SIGNAL) { ///////////////////////////////

        uint64 argument;
        __asm__ volatile("ld %[variable], 88(s0)" : [variable] "=r"(argument)); //citanje argumenta
        K_Semaphore *handle = (K_Semaphore *) argument;

        returnValue = (uint64) handle->signal();

        __asm__ volatile("sd %[value], 80(s0)" : : [value] "r"(returnValue));
        //upis povratne vrednosti u a0

    }

    else if (code == TIME_SLEEP) { /////////////////////////////////

        uint64 argument;
        __asm__ volatile("ld %[variable], 88(s0)" : [variable] "=r"(argument)); //citanje argumenta
        time_t time = (time_t) argument;

        TCB::sleep(time);

        TCB::dispatch();

    }

    else if(code == GETC) { ///////////////////////////////////////

        K_Console::ConsoleObj().inputItemAvailable->wait();
        char data = K_Console::ConsoleObj().inputBuffer->take();

        __asm__ volatile("sd %[value], 80(s0)" : : [value] "r" (data));
        //upis povratne vrednosti u a0
    }

    else if(code == PUTC) { //////////////////////////////////////

        char data;
        __asm__ volatile("ld %[variable], 88(s0)" : [variable] "=r" (data)); //citanje argumenta

        if(!K_Console::ConsoleObj().outputBuffer->append(data))  K_Console::ConsoleObj().outputItemAvailable->signal();
    }

        w_sstatus(sstatus);
        w_sepc(sepc);
        mc_sip(SIP_SSIP); //softverski prekid obradjen
    }

}

void Riscv::handleSupervisorTrap2() {
    // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)

    if(TCB::firstInSleepQueue) TCB::firstInSleepQueue->differenceSleepTime -= 1; //smanjuje preostalo vreme prve niti
    TCB::wakeUp(); //budi niti kojima je isteklo vreme za spavanje

    TCB::timeSliceCounter++;
    if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
    {
        uint64 volatile sepc = r_sepc();
        uint64 volatile sstatus = r_sstatus();
        TCB::timeSliceCounter = 0;
        TCB::dispatch();
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    mc_sip(SIP_SSIP); //softverski prekid obradjen
}

void Riscv::handleSupervisorTrap3() {
    // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
    int irq = plic_claim();

    if(irq == CONSOLE_IRQ) {

        //citanje podataka
        while((*(char*)CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT) {
            char data = *(char *) CONSOLE_RX_DATA;
            if(!K_Console::ConsoleObj().inputBuffer->append(data)) K_Console::ConsoleObj().inputItemAvailable->signal();
        }
    }

    plic_complete(irq);
}