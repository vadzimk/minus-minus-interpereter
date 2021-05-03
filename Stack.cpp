//
//  Stack.cpp
//  MinusMinus
//
//  Created by Tony Biehl on 8/15/16.
//  Copyright © 2016 Tony Biehl. All rights reserved.
//

#include "Stack.h"

#ifndef Stack_cpp
#define Stack_cpp
#include <iostream>

using namespace std;

//** Stack
template<class T>
Stack<T>::Stack()
{
    size = 0;
}

//** push
// push value onto stack, return true if successful, else false
template<class T>
bool Stack<T>::push(T value)
{
    if (size > STACK_SIZE)
        return false;
    values[size] = value;
    size++;
    return true;
}

//** pop
// pop value from stack, return true if successful, else false
template<class T>
bool Stack<T>::pop()
{
    if (size > 0) {
        size--;
        return true;
    }
    return false;
}

//** pop(num)
// pop number of values from stack, return true if successful, else false
template<class T>
bool Stack<T>::pop(int num) {
    if (size - num >= 0){
        size -= num;
        return true;
    }
    return false;
}

//** peek(index, success)
// peek value from stack using the index. Return success false if out of range and values[0],
//       else true and correct value
template<class T>
T Stack<T>::peek(int index, bool& success){
    T temp = values[0];
    success = true;
    if (index < 0 || index >= size){
        success = false;
        return temp;
    }
    return values[index];
}

//** peek
// peek value from top of stack. if stack empty then values[0] (can be invalid)
template<class T>
T Stack<T>::peek() {
    T temp = values[0];
    if (size > 0)
        return values[size-1];
    return temp; // empty stack
}

//** poke(index, value)
// poke value to stack using the index. Return false if out of range, else true
template<class T>
bool Stack<T>::poke(int index, T value) {
    if (index < 0 || index >= size)
        return false;
    values[index] = value;
    return true;
}

//** getStackSize
// return the number of items on stack
template<class T>
int Stack<T>::getStackSize() {
    return size;
}

//** clear
// clear the stack;
template<class T>
void Stack<T>::clear() {
    size = 0;
}


#endif

