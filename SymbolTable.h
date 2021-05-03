//
//  SymbolTable.hpp
//  MinusMinus
//
//  Created by Tony Biehl on 8/15/16.
//  Copyright © 2016 Tony Biehl. All rights reserved.
//

#ifndef SymbolTable_h
#define SymbolTable_h

#include <iostream>
#include <string>
#include <ostream>

using std::string;
using std::ostream;

const int SYMBOLS_SIZE = 200;

enum SymbolType {
    FUNC, PROC, VARIABLE, NONE
};

// single entry into the SymbolTable
class Symbol {
private:
    string symbol;
    int offset;
    SymbolType type;
public:
    Symbol(string s, int off, SymbolType t);

    Symbol();

    bool operator==(Symbol s);

    string getSymbol();

    int getOffset();

    SymbolType getType();

    friend ostream &operator<<(ostream &, const Symbol &);
};

// Essentially a Bag structure with no order
class SymbolTable {
private:
    Symbol items[SYMBOLS_SIZE];
    int size;
public:
    SymbolTable();

    // takes Symbol item to add to symbol table, returns true if successful, else false
    bool add(Symbol item);

    // takes Symbol item as a call by reference, uses the symbol to search for the item,
    // if found, updates offset and numeric fields and returns true, else returns false
    bool get(Symbol &item);

    // return the size of the SymbolTable
    int getSize();

    // print the entire SymbolTable
    void printTable();
};

#endif /* SymbolTable_h */
