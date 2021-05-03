//
//  main.cpp
//  MinusMinusV3
//
//  Created by Tony Biehl on 1/11/21.
//  Copyright © 2021 Tony Biehl. All rights reserved.
//

#include <iostream>
#include "Program.h"
#include "support.hpp"
#include <stdlib.h>

int main() {
    Program prog;
    SymbolTable local; // define a local symbol table for execute to use per function/prodecure
    srand(time(0)); // set seed for rand
    bool success = false;
    do {
        success = load(prog);
    } while (!success);
    prog.push(0); // push a dummy return for main proc
    execute(prog, local, 0, 0);
    return 0;
}
