/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                return 0;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
  TokenScanner scanner;
  scanner.ignoreWhitespace();
  scanner.scanNumbers();
  scanner.setInput(line);

  std::string op_str=scanner.nextToken();
  bool is_temp=true;//the line is a temporary statement
  if(scanner.getTokenType(op_str)==NUMBER) {
    is_temp=false;
  }
  if(is_temp) {
    if(op_str=="LET") {
      if(!scanner.hasMoreTokens()) {
        error("SYNTAX ERROR");
        return;
      }
      std::string name=scanner.nextToken();
      scanner.saveToken(name);
      Expression *exp = parseExp(scanner);
      state.setValue(name,exp->eval(state));
      delete exp;
    } else if(op_str=="PRINT") {
      Expression *exp = parseExp(scanner);
      std::cout<<exp->eval(state)<<'\n';
      delete exp;
    } else if(op_str=="INPUT") {
      std::string name=scanner.nextToken();
      int value;
      std::string input_str;
      TokenScanner ts;
      Expression *exp;
      while(true) {
        std::cout<<" ? ";
        getline(std::cin, input_str);
        ts.setInput(input_str);
        exp = parseExp(ts);
        if (exp->getType() == CONSTANT) {
          break;
        }
        error("INVALID NUMBER");
      }
      value=exp->eval(state);
      state.setValue(name,value);
      delete exp;
    } else if(op_str=="RUN") {
      program.run(state,program);
    } else if(op_str=="LIST") {
      program.list();
    } else if(op_str=="CLEAR") {
      program.clear();
      state.Clear();
    } else if(op_str=="QUIT") {
      program.clear();
      state.Clear();
      exit(0);
    } else if(op_str=="HELP") {
      std::cout<<"Learn it yourself, do not count on anyone!\n";
    } else {
      error("SYNTAX ERROR");
    }
  }
  else {
    int line_number=stringToInteger(op_str);
    if (!scanner.hasMoreTokens()) {
      program.removeSourceLine(line_number);
      return;
    } else {
      program.addSourceLine(line_number, line);
    }
    op_str=scanner.nextToken();
    if(op_str=="REM") {
      program.setParsedStatement(line_number,new RemStmt());
    } else if(op_str=="LET") {
      std::string name=scanner.nextToken();
      std::string as=scanner.nextToken();
      if(as!="=") {
        error("SYNTAX ERROR");return;
      }
      program.setParsedStatement(line_number,new AssignStmt(name, parseExp(scanner)));
    } else if(op_str=="PRINT") {
      program.setParsedStatement(line_number,new PrintStmt(parseExp(scanner)));
    } else if(op_str=="INPUT") {
      std::string name=scanner.nextToken();
      program.setParsedStatement(line_number,new InputStmt(name));
    } else if (op_str=="END") {
      program.setParsedStatement(line_number,new EndStmt());
    } else if(op_str=="GOTO") {
      int target= stringToInteger(scanner.nextToken());
      program.setParsedStatement(line_number,new GotoStmt(target));
    } else if (op_str=="IF") {
      TokenScanner l_scanner,r_scanner;
      l_scanner.ignoreWhitespace();
      l_scanner.scanNumbers();
      r_scanner.ignoreWhitespace();
      r_scanner.scanNumbers();
      std::string str=scanner.nextToken();
      while(str!="<"&&str!="="&&str!=">") {
        l_scanner.saveToken(str);
        str=scanner.nextToken();
      }
      std::string op=str;
      Expression *lhs= parseExp(l_scanner);
      while(str!="THEN") {
        r_scanner.saveToken(str);
        str=scanner.nextToken();
      }
      Expression *rhs=parseExp(r_scanner);
      str=scanner.nextToken();
      int target= stringToInteger(str);
      program.setParsedStatement(line_number,new IfGotoStmt(lhs,rhs,op,target));
    } else {
      program.removeSourceLine(line_number);
      error("SYNTAX ERROR");
    }

  }

}

