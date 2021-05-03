//
//  Symbols.cpp
//  MinusMinus
//
//  Created by Tony Biehl on 8/15/16.
//  Copyright © 2016 Tony Biehl. All rights reserved.
//

#include "SymbolTable.h"

using namespace std;

Symbol::Symbol(string s, int off, SymbolType t) {
    symbol = s;
    offset = off;
    type = t;
}

Symbol::Symbol() {
    symbol = "";
    offset = -1;
    type = NONE;
}

bool Symbol::operator==(Symbol s) {
    return symbol == s.symbol;
}

string Symbol::getSymbol() {
    return symbol;
}

int Symbol::getOffset() {
    return offset;
}

SymbolType Symbol::getType() {
    return type;
}

ostream &operator<<(ostream &out, const Symbol &t) {
    out << t.symbol << " " << t.offset << " ";
    switch (t.type) {
        case FUNC:
            out << "function";
            break;
        case PROC:
            out << "procedure";
            break;
        case VARIABLE:
            out << "variable";
            break;
        case NONE:
            out << "none";
            break;
    }
    return out;
}


SymbolTable::SymbolTable() {
    for (int i = 0; i < SYMBOLS_SIZE; i++)
        items[i] = Symbol("", -1, NONE);
    size = 0;
}

//** add
// takes Symbol item to add to symbol table, returns true if successful, else false
bool SymbolTable::add(Symbol item) {
    bool notFound = true;
    int index = 0;
    while (index < size && notFound) {
        if (items[index] == item)
            notFound = false;
        index++;
    }
    if (notFound) {
        items[size] = item;
        size++;
    }
    return notFound;
}

//** get
// takes Symbol item as a call by reference, uses the symbol to search for the item,
// if found, updates offset and numeric fields and returns true, else returns false
bool SymbolTable::get(Symbol &item) {
    int foundIndex = -1;
    int index = 0;
    bool found = false;
    while (index < size && foundIndex < 0) {
        if (items[index] == item)
            foundIndex = index;
        index++;
    }
    if (foundIndex >= 0) {
        item = items[foundIndex];
        found = true;
    }
    return found;
}

//** getSize
// return the size of the SymbolTable
int SymbolTable::getSize() {
    return size;
}

//** printTable
// print the entire SymbolTable
void SymbolTable::printTable() {
    for (int x = 0; x < size; x++)
        cout << items[x] << endl;
}
