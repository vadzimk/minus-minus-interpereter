//
//  support.hpp
//  MinusMinusV3
//
//  Created by Tony Biehl on 2/13/21.
//  Copyright © 2021 Tony Biehl. All rights reserved.
//

#ifndef support_hpp
#define support_hpp

#include <iostream>
#include "Program.h"
#include "Stack.h"


/**
 * Get a filename, open file and pass to Program object to read from file into the object's internal program
 */
bool load(Program &p);

/** calculate
 Input: postFix stack created by parseEquation, local SymbolTable, and success boolean
 output: calculated value
 calculate value from the postFix stack by treating it like prefix (easy to do with a stack).
 Use the passed local SymbolTable to look up variables. Function is recursive
 changed: postFix
 */
int calculate(Program &p, Stack<string> &postFix, SymbolTable &local, bool &success);

/** compare
 input: line (with the boolean compare) and local SymbolTable
 output: extract two values and an operation string from line, use the SymbolTable to look up locals,
 and return true or false
 changed: line
 */
bool compareBool(Program &p, SymbolTable &local);

/** convertOperand
 input: s (has operand), local SymbolTable, and boolean success
 output: convert operand into a value
 Note: operand is either a literal, a variable and/or equation, or could be a function call.
 function calls start with a FUNCTION_ARG character (put on by parseEquation, not user).
 changed: s, table and success
 */
int convertOperand(Program &p, string &s, SymbolTable &table, bool &success);

/** execute
 Will take the Program object p with the addition of:
   1. A local SymbolTable
   2. Where to start in the MinusMinus code (lineStart)
   3. The number of parmeters (numParms)
      Note: the values of the arguments are on the
            p object's stack and the local SymbolTable
            will link up the parameter name with the
            stack offset
 Sets up needed variables and even has a whiles stack
    for use with keeping track of the top of current while
 Will do a loop with a switch statement for executing
    MinusMinus instrucions until either the boolean run
    is false or an error occurs
 */
void execute(Program &p, SymbolTable &local, int line, int numParms);

/** parseEquation
 input: exp, local SymbolTable, and success
 output: parse the equation in exp and create a postFix stack with operands and operators in postFix order.
 note: it will use a temporary operatorStack for operators to get the order correct. Also save lineNumber to avoid program problems
 And it will treat function calls as operands but append a FUNCTION_ARG on the front for easy recognition
 by the calculate function. Routine is recursive.
 changed: p, local, success and restore lineNumber
 */
int parseEquation(Program &p, string exp, SymbolTable &local, bool &success);

#endif /* support_hpp */
