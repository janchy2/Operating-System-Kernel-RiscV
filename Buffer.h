//
// Created by os on 5/17/23.
//

#ifndef BUFFER_H
#define BUFFER_H
#include "MemoryAllocator.h"

const int BufferSize = 256;

class Buffer {

public:
    static Buffer* createBuffer();
    int append(char c);
    char take();
private:
    int numOfElems;
    char buffer[BufferSize];
    int head, tail;
};


#endif //BUFFER_H
