# Operating-Systems-1-Project
A small kernel of a multithreaded operating system with time sharing, written for RISC V architecture using assembly and C/C++. Includes a memory allocator, threads, semaphores and preemption on timer and keyboard interrupts.
The user application and the kernel use the same address space, as together they make a static program in the operating memory.
These are the implemeneted system calls:
- void* mem_alloc (size_t size)
- int mem_free (void*)
- int thread_create (thread_t* handle, void(*start-routine)(void*), void* arg)
- int thread_exit ()
- void thread_dispatch ()
- void thread_join ( thread_t handle)
- int sem_open (sem_t* handle, unsigned init) 
- int sem_close (sem_t handle)
- int sem_wait (sem_t id)
- int sem_signal (sem_t id)
- int time_sleep (time_t)
- char getc ()
- void putc (char)

The user application should be written in function void userMain().
