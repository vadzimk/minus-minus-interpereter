//
//  Program.cpp
//  MinusMinusV3
//
//  Created by Tony Biehl on 2/13/21.
//  Copyright © 2021 Tony Biehl. All rights reserved.
//

#include "Program.h"
#include "support.hpp"

Program::Program() {
    size = lineNumber = errorCount = 0;

}

bool Program::addParmOffset(string parmName, int offset,
                            SymbolTable &localVars) // add the parameter to the local vars and attach the offset to the stack to it
{
    int tos = stack.getStackSize() - 1; // get actual top of stack
    Symbol parm = Symbol(parmName, tos - offset, VARIABLE); // set parm to actual index in stack
    return localVars.add(parm); // add the parm
}

/** checkFirstChar
 input: s and ch
 output: check to make sure string s is not empty then check if first character of s is equal to ch
 (return true if so, else false). This prevents crashes
 */
bool checkFirstChar(string s, char ch) {
    return (s.length() > 0 && s[0] == ch);
} // end checkFirstChar



/** chop
 input: str and x
 output: chop string by keeping string str from x to the end of the line
 changed: str
 */
void chop(string &str, int x) {
    if (x < str.length())
        str = str.substr(x, str.length());
    else
        str = "";
} // end chop

/**
 * @return one of Commands enums
 * */
Commands Program::convertLine(string &c) {
    command = UNKNOWN;
    string cmdText = nextToken(c, END_CMD);
    trim(SPACES, cmdText);
    trim(SPACES, c);
    for (int i = 0; i < cmdText.length(); i++)
        cmdText[i] = toupper(cmdText[i]);
    if (cmdText == "")
        command = BLANK;
    else if (cmdText.length() > 0 && cmdText[0] == ';')
        command = COMMENT;
    else if (cmdText == "DECLARE")
        command = DECLARE;
    else if (cmdText == "ENDIF")
        command = ENDIF;
    else if (cmdText == "ENDWHILE")
        command = ENDWHILE;
    else if (cmdText == "FUNCTION")
        command = FUNCTION;
    else if (cmdText == "IF")
        command = IF;
    else if (cmdText == "INPUT")
        command = INPUT;
    else if (cmdText == "PRINT")
        command = PRINT;
    else if (cmdText == "PRINTLN")
        command = PRINTLN;
    else if (cmdText == "PROCEDURE")
        command = PROCEDURE;
    else if (cmdText == "RETURN")
        command = RETURN;
    else if (cmdText == "WHILE")
        command = WHILE;
    else if (cmdText.length() > 0 && isValidID(cmdText)) {
        if (c.length() > 1 && c.substr(0, 2) == ":=")
            command = ASSIGN;
        else if (c[0] == '(')
            command = CALL;
    }
    return command;
}


/** countArguments
 input: table SymbolTable, parameters passed via s (in parenthesis),
 and boolean zero for just putting on zeros onto stack and not calculated values
 output: take every argument in s and either put a zero onto the stack or push the parsed value
 changed: s and stack
 */
int Program::countArguments(SymbolTable &localVars, string &s) {
    int count = 0;
    string temp;
    bool success;
    trim(PARENTHESIS, s);
    while (s != "") // count the parameters
    {
        temp = nextToken(s, END_COMMA); // get parameter
        stack.push(parseEquation(*this, temp, localVars, success)); // save on stack
        count++; //
        if (s != "" && nextToken(s, END_COMMA) != ",")
            errorMsg("Missing comma");
    }
    return count;
} // end countArguments


/** errorMsg
 input: msg for the error message
 output: print lineNum and error and count it via errorCount.
 Return false for convenience
 changed: errorCount
 */
bool Program::errorMsg(string msg) {
    // assume lineNumber pointing one past line with error
    cout << "Error at line " << lineNumber << ": " << msg << endl;
    errorCount++;
    return false;
} // end errorMsg

int Program::getErrorCount() {
    return errorCount;
}

int Program::getLineNumber() {
    return lineNumber;
}

bool Program::getMethod(Symbol &s) {
    return methods.get(s);
}

/** isOperator
 input: string s as a math operator
 output: check if math operator (true if operator, else false)
 */
bool Program::isOperator(string s) {
    if (s == "+" || s == "-" || s == "*" || s == "/" || s == "%")
        return true;
    return false;
} // end isOperator

/** isValidID
 input: string id
 output: will check id and return true if valid id else false (and generate any needed errors)
 changed: number of errors
 */
bool Program::isValidID(string id) {
    bool valid = true;
    if (id.length() == 0) {
        errorMsg("blank ID");
        valid = false;
    } else if (!isalpha(id[0])) {
        errorMsg("ID " + id + " must start with a letter");
        valid = false;
    }
    for (int x = 1; valid && x < id.length(); x++) {
        if (!isalnum(id[x])) {
            errorMsg("Invalid character in identifier of " + id);
            valid = false;
        }
    }
    return valid;
} // end isValidID


/**
 * @param inFile mm source code file
 * @param list boolean directs to list the source code
 * @post SymbolTable 'methods' created
 * */
bool Program::loadProg(ifstream &inFile,
                       bool list) // load the program using inFile given. Return true if successful else false
{
    bool read = false;
    Commands cmd;
    Symbol s;
    int temp;

    if (inFile.fail())
        cout << "Could not open file" << endl;
    else {
        read = true;
        while (!inFile.eof() && size < SIZE) {
            getline(inFile, line);
            temp = line.length();
            if (temp > 0 && line[temp-1] < ' ')
                line[temp-1] = ' '; // remove returns
            progLine[size] = line;
            size++;
            if (list)
                cout << setw(3) << size << " " << line << endl;
        }
        lineNumber = 0;
        for (int i = 0; i < size; i++) {
            line = progLine[lineNumber];
            lineNumber++;
            cmd = convertLine(line);
            string method = nextToken(line, END_PAREN);
            if (cmd == FUNCTION) {
                s =  Symbol(method,lineNumber-1, FUNC);
                methods.add(s);
            } else if (cmd == PROCEDURE) {
                s =  Symbol(method,lineNumber-1, PROC);
                methods.add(s);
            }
        }
    }
    return read;
}

void Program::operator=(int lineNum) // goto lineNum
{
    lineNumber = lineNum;
}

Commands Program::operator++() // next line and return the command
{
    Commands cmd = UNKNOWN;
    if (lineNumber >= size) {
        command = cmd = ENDPROGRAM;
    } else if (errorCount == 0) {
        line = progLine[lineNumber++];
        cmd = convertLine(line);
    }
    return cmd;
}

string Program::operator-=(Extract e) // get next token
{
    switch (e) {
        case BOOLEAN:
            return nextToken(line, END_COMPARE);
        case CMD:
            return nextToken(line, END_CMD);
        case COMMA:
            return nextToken(line, END_COMMA);
        case COMPARE:
            return nextToken(line, END_COMPARE);
        case EQUATION:
        case LINE:
            return nextToken(line, END_LINE);
        case PARMS:
            trim(PARENTHESIS, line);
            return "(";
        case OPEN_PARM:
            return nextToken(line, END_PAREN);
        case STRING:
        case TOKEN:
            return nextToken(line, END_SPACE);
    }
    return " ";
}

bool Program::operator==(string method) // see if valid function or procedure
{
    Symbol temp = Symbol(method, 0, NONE);
    if (methods.get(temp))
        return true;
    return false;
}

Commands &operator*=(Commands &c, Program &p) // friend operator to get the command
{
    c = p.command;
    return c;
}

string &operator*=(string &s, Program &p) // friend operator to get the last token
{
    s = p.lastToken;
    //cout << "lastToken " << s << endl;
    return s;
}

int &operator*=(int &offset, Program &p) // friend operator to get the item on stack via offset
{
    bool success;
    offset = p.stack.peek(offset, success);
    if (!success)
        p.errorMsg("Stack access out of range");
    return offset;
}

/** nextFactor
 input: s with an equation to get the next part (factor) of
 output: get next factor of an equation: operator, literal, variable or function call and return that.
 With function call, FUNCTION_ARG (~) is appended to the front (for parseEquation) for easier recognition
 changed: s
 */
string Program::nextFactor(string &s) {
    string temp = "";
    int x = 0;
    bool endFactor = false;
    bool variable = false;
    trimIt(s);
    // check for operator
    if (s.length() > 0
        && (s[x] == '(' || s[x] == '*' || s[x] == '/' || s[x] == '+' || s[x] == '-' || s[x] == '%' || s[x] == ')')) {
        temp += s[x];
        endFactor = true;
        x++;
    }
    if (!endFactor && s.length() > x) { // check for variable or literal
        if (isalpha(s[x]))
            variable = true;
        while (!endFactor && s.length() > x) {
            if (!variable && isdigit(s[x])) {
                temp += s[x];
                x++;
            } else if (variable && isalnum(s[x])) {
                temp += s[x];
                x++;
            } else
                endFactor = true;
        }
    }
    s = s.substr(x);
    trimIt(s);
    if (variable && checkFirstChar(s, '(')) { // a function! Get the entire function call
        x = 0; // reset x
        int countPar = 0; // count open parentheis
        do {
            temp += s[x];
            if (s[x] == ')')
                countPar--;
            else if (s[x] == '(')
                countPar++;
            x++;
        } while (s.length() > x && countPar > 0);
        if (countPar > 0)
            errorMsg("Missing end ) to function call");
        s = s.substr(x);
        temp = FUNCTION_ARG + temp;
    }
    return temp;
} // end nextFactor

/** nextToken
 input: s, endCH, variable char, defaults to ' '
 if END_CMD ('^'), get chars until either a open parenthsis or space
 if END_COMMA (','), get chars until a comma is reached
 Note: if only a comma then the comma is returned
 Also note: if a comma is in parenthesis (like a function) routine keeps going
 if END_LINE ('~'), gets the rest of the line and empties line
 if END_COMPARE ('='), gets chars until a compare operator hit
 if END_PAREN ('('), gets chars until a open parenthesis operator hit
 if a double quote then get entire string
 output: returns extracted string from s and sets to currentToken and chops s
 changed: s and currentToken (access that by token function)
 */
string Program::nextToken(string &s, char endCH) {
    string temp = "";
    int x = 0;
    int numParens = 0;
    bool endToken = false;
    bool endCall = false;
    trim(SPACES, s);
    //cout << "nextToken start |" << s << "|\n";
    if (endCH == END_LINE) { // get the rest of the line
        temp = s;
        s = "";
    } else if (endCH == END_CMD) {
        endCH = END_SPACE;
        endCall = true; // do end parenthesis AND space
    }
    if (s.length() > 0) {
        if (checkFirstChar(s, '"') && endCH == ' ')
            endCH = '"';
        while (!endToken && x < s.length()) {
            char ch = s[x];
            if (ch == '\t') {
                ch = ' ';
                temp += "   ";
            }
            if (endCH == END_COMPARE && (ch == '<' || ch == '=' || ch == '>')) {
                endToken = true;
            } else if ((endCH == END_PAREN || endCall) && ch == '(') {
                endToken = true;
            } else if (ch == '"') {
                temp += ch; // have a string, get it
                x++;
                while (x < s.length() && s[x] != '"') {
                    temp += s[x];
                    x++;
                }
                temp += s[x]; // final quote
            } else if (endCH == ch) {
                if (endCH == END_COMMA) {
                    if (x == 0) {
                        temp += ch; // comma is the token
                        x++; // remove it
                        endToken = true;
                    } else if (numParens > 0)
                        temp += ch; // comma part of a function call
                    else
                        endToken = true; // command ends the token
                } else
                    endToken = true; // must be END_SPACE
            } else {
                // allow function calls to be one unit (skip their commas)
                if (ch == '(')
                    numParens++;
                else if (ch == ')')
                    numParens--;
                temp += ch;
                if (endCH == ch)
                    endToken = true;
            }
            if (!endToken)
                x++;
        } // end while
        if (numParens > 0)
            errorMsg("Parenthesis mismatch");
        s = s.substr(x);
    } // end if
    //cout << "nextToken end with s |" << s << "| and temp |" << temp << "|\n";
    lastToken = temp; // save a copy of the token
    return temp;
} // end nextToken


int Program::peek() // get a copy of top of stack
{
    return stack.peek();
}

int Program::peek(string v,
                  SymbolTable &localVars) // get a copy of the value of local variable (localVars points to correct place on stack)
{
    bool success = false;
    int result = 0;
    Symbol sym = Symbol(v, 0, NONE);
    if (!localVars.get(sym))
        errorMsg(v + " variable not found");
    else if (sym.getType() != VARIABLE)
        errorMsg(v + " is not a variable");
    else
        result = stack.peek(sym.getOffset(), success);
    if (!success)
        errorMsg("Could not access correct variable offset in stack");
    return result;
}

int Program::peek(int i) // know which offset on stack you want
{
    bool success = false;
    int val = stack.peek(i, success);
    if (!success)
        errorMsg("Could not access correct index in stack");
    return val;
}

void Program::pop(int x) // pop last x items from stack
{
    if (!stack.pop(x))
        errorMsg("Tried to pop too many values from stack");
}

/** precedence
 return precedence of three operators (*, /, % have precedence of 1, all others precedence of 0)
 */
int Program::precedence(string s) {
    int pred = 0;
    if (s == "*" || s == "/" || s == "%")
        pred = 1;
    return pred;
} // end precedence

void Program::push(int line) // push current line onto stack
{
    stack.push(line);
}

void
Program::push(string v, SymbolTable &localVars) // push local variable space onto stack and add variable to localVars
{
    int next = stack.getStackSize(); // get next available space on stack
    stack.push(0);
    Symbol var = Symbol(v, next, VARIABLE);
    localVars.add(var);
}

void Program::poke(string v, int val,
                   SymbolTable &localVars) // set variable to value and use localVars to find place on stack
{
    Symbol s = Symbol(v, 0, VARIABLE);
    bool success = localVars.get(s);
    if (!success) {
        errorMsg("Invalid variable of " + v);
        return;
    }
    // got the symbol for var, now get offset to stack
    int offset = s.getOffset();
    stack.poke(offset, val);
}


void Program::setLineNumber(int lineNum) // set the program line
{
    if (lineNum < 0 || lineNum >= size)
        errorMsg("New line number out of range");
    else
        lineNumber = lineNum;
}

/** trim
 Trim the s string variable of beginning spaces and ending spaces or,
 remove the beginning double quote and ending double quote (error if no quotes) or,
 remove the ending parenthesis (error if no ending)
 input: Trim t and s
 Trim is a enum of SPACES, QUOTE, and PARENTHESIS
 output: s is trimmed according to t
 changed: s
 */
void Program::trim(Trim t, string &s) {
    int i;
    if (t == SPACES) {
        trimIt(s);
    } else if (t == QUOTE) {
        if (checkFirstChar(s, '"'))
            chop(s, 1);
        else
            errorMsg("No beginning quote");
        i = s.length() - 1;
        if (i >= 0 && s[i] == '"')
            s = s.substr(0, i);
        else
            errorMsg("No ending quote");
    } else if (t == PARENTHESIS) {
        if (checkFirstChar(s, '('))
            chop(s, 1);
        else
            errorMsg("No beginning parenthesis");
        i = s.length() - 1;
        if (i >= 0 && s[i] == ')')
            s = s.substr(0, i);
        else
            errorMsg("No ending parenthesis");
    } else
        errorMsg("Bad trim value");

} // end trim

/** trimIt
 input: s
 output: remove all leading and trailing spaces from s
 changed: s
 */
void Program::trimIt(string &s) {
    int i;
    while (checkFirstChar(s, ' ')) {
        chop(s, 1);
    }
    i = s.length() - 1;
    while (s.length() > 0 && s[i] == ' ') {
        s = s.substr(0, i);
        i--;
    }
} // end trimIt
