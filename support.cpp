//
//  support.cpp
//  MinusMinusV3
//
//  Created by Tony Biehl on 2/13/21.
//  Copyright © 2021 Tony Biehl. All rights reserved.
//

#include "support.hpp"

using namespace std;


/** calculate
 Input: postFix stack created by parseEquation, local SymbolTable, and success boolean
 output: calculated value
 calculate value from the postFix stack by treating it like prefix (easy to do with a stack).
 Use the passed local SymbolTable to look up variables. Function is recursive
 changed: postFix
 */
int calculate(Program &p, Stack<string> &postFix, SymbolTable &local, bool &success) {
    int result = 0, result2 = 0;
    string s, operand1, operand2;
    success = true; // be optimistic
    if (postFix.getStackSize() == 0) { // error
        p.errorMsg("blank equation");
        return 0;
    }
    s = postFix.peek(); // get operator
    postFix.pop();
    if (!p.isOperator(s)) {
        if (postFix.getStackSize() > 1) // single operand?
            success = false; // no, bad equation
        else // yes, get single operand value
            result = convertOperand(p, s, local, success);
    } else { // regular equation
        if (postFix.getStackSize() == 0) { // no operand?
            p.errorMsg("Missing operand");
            return 0;
        }
        operand2 = postFix.peek(); // get operand 2
        if (p.isOperator(operand2)) //
            result2 = calculate(p, postFix, local, success);
        else {
            postFix.pop(); // remove operand
            result2 = convertOperand(p, operand2, local, success);
        }
        if (postFix.getStackSize() == 0) { // no operand?
            p.errorMsg("Missing second operand");
            return 0;
        }
        operand1 = postFix.peek(); // get operand 1
        if (p.isOperator(operand1)) //
            result = calculate(p, postFix, local, success);
        else {
            postFix.pop(); // remove operand
            result = convertOperand(p, operand1, local, success);
        }
        //cout << "operator " << s << " operand1 " << operand1 << " operand2 " << operand2 << endl;
        //cout << "Values " << result << " " << result2 << endl;
        switch (s[0]) // do operation
        {
            case '+' :
                result += result2;
                break;
            case '-' :
                result -= result2;
                break;
            case '*' :
                result *= result2;
                break;
            case '/' :
                result /= result2;
                break;
            case '%' :
                result %= result2;
                break;
        }
    }
    return result;
} // end calculate

/** compare
 input: line (with the boolean compare) and local SymbolTable
 output: extract two values and an operation string from line, use the SymbolTable to look up locals,
 and return true or false
 changed: line
 */
bool compareBool(Program &p, SymbolTable &local) {
    bool success;
    int val1, val2;
    string operand1, compareOp, operand2;
    p -= COMPARE;
    operand1 *= p; // get first part of compare
    val1 = parseEquation(p, operand1, local, success);
    if (!success)
        p.errorMsg("Invalid first operand of compare");
    p -= CMD;
    compareOp *= p;
    p -= EQUATION;
    operand2 *= p;
    val2 = parseEquation(p, operand2, local, success);
    if (!success)
        p.errorMsg("Invalid second operand of compare");
    success = false;
    if (compareOp == "=")
        success = (val1 == val2);
    else if (compareOp == "<")
        success = (val1 < val2);
    else if (compareOp == ">")
        success = (val1 > val2);
    else if (compareOp == "<=")
        success = (val1 <= val2);
    else if (compareOp == ">=")
        success = (val1 >= val2);
    else if (compareOp == "<>")
        success = (val1 != val2);
    else
        p.errorMsg("Invalid comparision operation of " + compareOp);
    return success;
} // end compare

/** convertOperand
 input: s (has operand), local SymbolTable, and boolean success
 output: convert operand into a value
 Note: operand is either a literal, a variable and/or equation, or could be a function call.
 function calls start with a FUNCTION_ARG character (put on by parseEquation, not user).
 changed: s, table and success
 */
int convertOperand(Program &p, string &s, SymbolTable &table, bool &success) {
    int result = 0;
    Symbol sym;
    bool unaryMinus = false;
    if (s.length() > 0) {
        if (s[0] == '+') // unary plus
            s = s.substr(1); // remove the unary plus
        else if (s[0] == '-') { // unary minus
            s = s.substr(1); // remove the minus
            unaryMinus = true;
        }
        p.trimIt(s);
        if (s.length() == 0) {
            p.errorMsg("leading + or - only");
            s = "0";
        }
        if (isdigit(s[0])) { // get literal
            result = atoi(s.c_str());
        } else if (isalnum(s[0])) { // get variable
            sym = Symbol(s, 0, NONE);
            if (!table.get(sym))
                p.errorMsg(s + " variable not found");
            else if (sym.getType() != VARIABLE)
                p.errorMsg(s + " is not a variable");
            else
                result = p.peek(sym.getOffset());
        } else if (s[0] == FUNCTION_ARG) { // get function
            s = s.substr(1); // remove the function indicator
            string temp;
            int x = 0;
            while (s.length() > x && s[x] != '(') { // get the function name
                temp += s[x];
                x++;
            }
            if (s.length() == x || s[x] != '(')
                p.errorMsg(temp + " not a function call");
            else {
                s = s.substr(x);
                p.trimIt(s);
                sym = Symbol(temp, 0, NONE);
                if (temp == "rand") {
                    result = rand();
                    p.trim(PARENTHESIS, s);
                    if (s != "")
                        p.errorMsg("built-in function rand has no parameters");
                } else if (!p.getMethod(sym)) // functions and procedures in "methods"
                    p.errorMsg(temp + " function not found");
                else { // call function
                    SymbolTable t; // create a symbol table for the function
                    p.push(0); // space for return value
                    p.push(p.getLineNumber()); // put return address on stack
                    int numParms = p.countArguments(table, s); // count and put args onto stack
                    execute(p, t, sym.getOffset(), numParms); // do the function with new symbol table

                    p.setLineNumber(p.peek()); // set the return address
                    p.pop(1); // remove return address
                    result = p.peek(); // get the return value
                    p.pop(1); // remove return value
                }
            }
        } else
            p.errorMsg(s + " unknown operand");
    }
    if (unaryMinus)
        result = -result;
    return result;
} // end convertOperand

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
void execute(Program &p, SymbolTable &local, int lineStart, int numParms) {
    Symbol lookup; // for looking up from SymbolTable
    Stack<int> whiles;
    p.setLineNumber(lineStart);
    //cout << "start at " << lineStart << endl;
    Commands codeMethod = ++p;
    Commands command;
    bool success, run = true, print = false;
    int temp = 1; // for first parm
    int numLocals = 0; // count how many locals on stack
    int val; // scratch variable
    int numIfs = 0; // to count nested ifs for endif
    int numWhiles = 0; // to count nested whiles for endwhile
    string token, compare, variable, method;
    while (codeMethod == COMMENT || codeMethod == BLANK)
        codeMethod = ++p; // skip leading comments and blank lines
    if (codeMethod == FUNCTION || codeMethod == PROCEDURE) {
        p -= OPEN_PARM;
        token *= p; // get function/procedure name
        p -= PARMS; // remove parenthesis for parms
        while (temp <= numParms) {
            p -= COMMA; // get a parm
            token *= p;
            if (p.isValidID(token)) {
                if (!p.addParmOffset(token, numParms - temp, local))
                    p.errorMsg(
                            "No room on stack");
                numLocals++;
            }
            p -= COMMA; // get past comma
            temp++;
        }
    }
    while (run && p.getErrorCount() == 0) {
        //cout << "at line " << p.getLineNumber()+1 << endl;
        // switch (++p) // now to start executing function/procedure code
        ++p; // point to next command
        Commands cmd;
        cmd *= p;
        /*
        string str;
        str *= p;
        cout << "cmd = " << str << " line " << p.getLineNumber()<< endl;
        cin >> str;
        */
        switch (cmd) {
            case ASSIGN:
                // TODO fill in the code
                break;
            case CALL:
                // TODO fill in the code
                break;
            case COMMENT:
            case BLANK:
                break;
            case DECLARE:
                // TODO fill in the code
                break;
            case ENDIF:
// TODO fill in the code
                break;
            case ENDPROGRAM: // ran out of code
                run = false;
                return; // exit
            case ENDWHILE:
// TODO fill in the code
                break;
            case FUNCTION:
            case PROCEDURE:
// TODO fill in the code
                return; // exit
            case IF:
// TODO fill in the code
                break;
            case INPUT:
                // TODO fill in the code
                break;
            case PRINT:
                print = true;
            case PRINTLN:
                // TODO fill in the code
                if (!print)
                    cout << endl;
                print = false;
                break;
            case RETURN:
// TODO fill in the code
                return;
            case UNKNOWN:
                token *= p;
                if (token != "")
                    p.errorMsg("Bad command");
                break;
            case WHILE:
// TODO fill in the code
                break;
        }
    }
}

/**
 Get a filename, open file and pass to Program object to read from file into the object's internal program
 */
bool load(Program &p) {
    bool read = false;
    string filename, t;
    ifstream inFile;
    cout << "Please enter program name: ";
    getline(cin, filename);
    inFile.open(filename.c_str());
    if (inFile.fail()) {
        cout << "Could not open " << filename << endl;
        inFile.close();
        return false;
    }
    cout << "List program (y for yes)? ";
    getline(cin, t);
    if (t.length() > 0 && tolower(t[0]) == 'y')
        read = p.loadProg(inFile, true);
    else
        read = p.loadProg(inFile, false);
    return read;
}

/** parseEquation
 input: exp, local SymbolTable, and success
 output: parse the equation in exp and create a postFix stack with operands and operators in postFix order.
 note: it will use a temporary operatorStack for operators to get the order correct. Also save lineNumber to avoid program problems
 And it will treat function calls as operands but append a FUNCTION_ARG on the front for easy recognition
 by the calculate function. Routine is recursive.
 changed: p, local, success and restore lineNumber
 */
int parseEquation(Program &p, string exp, SymbolTable &local, bool &success) {
    Stack<string> postFix;
    Stack<string> operatorStack;
    bool leadMinus = true;
    int oldLineNumber = p.getLineNumber();
    int temp;
    string s = p.nextFactor(exp), op, operand1;
    // TODO fill in the code
    temp = calculate(p, postFix, local, success);
    p.setLineNumber(oldLineNumber);
    return temp;
} // end parseEquation



