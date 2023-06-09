//
// Created by os on 4/7/23.
//
#include "../h/MemoryAllocator.h"

//NAPOMENA:
//DEO KODA PREUZET IZ RESENJA SA 2. KOLOKVIJUMA, SEPTEMBAR 2015. I KOLOKVIJUMA 2021 !!!


FreeMem *MemoryAllocator::freeMemHead = nullptr;
bool MemoryAllocator::isMade = false;

MemoryAllocator& MemoryAllocator::MemAllocObj() {
    if(!isMade) { //jos nije stvoren objekat
        size_t memory = (size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR + 1; //inicijalni blok - ceo memorijski prostor za alokaciju
        freeMemHead = (FreeMem *)HEAP_START_ADDR;
        freeMemHead->next = nullptr;
        freeMemHead->prev = nullptr;
        freeMemHead->numOfBlocks = (memory + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    }
    static MemoryAllocator instance;
    return instance;
}

void *MemoryAllocator::k_mem_alloc(size_t numOfBlocks) {
    FreeMem* curr = freeMemHead;
    for(; curr != nullptr; curr = curr->next) { //pronalazak slobodnog prostora
        if(curr->numOfBlocks >= numOfBlocks) break;
    }
    if(!curr) return nullptr; //nema mesta
    if(curr->numOfBlocks > numOfBlocks) { //ako ostaje slobodan fragment
        size_t remaining = curr->numOfBlocks - numOfBlocks;
        curr->numOfBlocks = numOfBlocks;
        size_t offset = numOfBlocks * MEM_BLOCK_SIZE;
        FreeMem *newFreeMem = (FreeMem *) ((char *) curr + offset);
        if (curr->prev) curr->prev->next = newFreeMem;
        else freeMemHead = newFreeMem;
        newFreeMem->next = curr->next;
        newFreeMem->prev = curr->prev;
        newFreeMem->numOfBlocks = remaining;
    } else { //ne ostaje slobodan fragment
        if(curr->prev) curr->prev->next = curr->next;
        else freeMemHead = curr->next;
    }
    curr->next = nullptr; //zaglavlje ostaje
    curr->prev = nullptr;
    return (char *) curr + sizeOfHeader;
}

int MemoryAllocator::k_mem_free(void *address) {
    FreeMem *newFreeMem = (FreeMem *)((char *) address - sizeOfHeader);
    if(!newFreeMem || newFreeMem < HEAP_START_ADDR || newFreeMem > HEAP_END_ADDR) return -1; //greska
    FreeMem* curr = nullptr;
    if(freeMemHead && newFreeMem > freeMemHead) { //ako ne treba da se doda kao prvi
        for(curr = freeMemHead; curr->next && newFreeMem > curr->next; curr = curr->next);
        //zaustavimo se kada je adresa novog fragmenta izmedju curr i curr->next
    }
    newFreeMem->prev = curr; //prevezivanje
    if(curr) newFreeMem->next = curr->next;
    else newFreeMem->next = freeMemHead;
    if(newFreeMem->next) newFreeMem->next->prev = newFreeMem;
    if(curr) curr->next = newFreeMem;
    else freeMemHead = newFreeMem;

    tryToMerge(newFreeMem); //pokusava se spajanje sa prethodnim i/ili sledecim
    tryToMerge(curr);

    return 0;
}

void MemoryAllocator::tryToMerge(FreeMem *p) {
    if(!p) return;
    size_t offset = p->numOfBlocks * MEM_BLOCK_SIZE;
    if(p->next && ((char*)p + offset) == (char*)(p->next)) {
        p->numOfBlocks += p->next->numOfBlocks;
        p->next = p->next->next;
        if(p->next) p->next->prev = p;
    }
}


