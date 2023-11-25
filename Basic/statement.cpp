/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"


/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

AssignStmt::AssignStmt(std::string &_name, Expression *_exp) {
  name=_name;
  exp=_exp;
}

AssignStmt::~AssignStmt() {
  delete exp;
}

void AssignStmt::execute(EvalState &state, Program &program) {
  int value=exp->eval(state);
  state.setValue(name,value);
}

PrintStmt::PrintStmt(Expression *_exp) {
  exp=_exp;
}

PrintStmt::~PrintStmt() {
  delete exp;
}

void PrintStmt::execute(EvalState &state, Program &program) {
  int value=exp->eval(state);
  std::cout<<value<<'\n';
}

InputStmt::InputStmt(std::string &_name) {name=_name;}

void InputStmt::execute(EvalState &state, Program &program) {
  int value;
  std::string input_str;
  TokenScanner ts;
  Expression *exp;
  while(true) {
    std::cout<<" ? ";
    getline(std::cin, input_str);
    ts.setInput(input_str);
    exp= parseExp(ts);
    if (exp->getType() == CONSTANT) {
      break;
    }
    error("INVALID NUMBER");
  }
  value=exp->eval(state);
  state.setValue(name,value);
}

void EndStmt::execute(EvalState &state, Program &program) {
  program.cur_line=-1;
}

GotoStmt::GotoStmt(int &num) {
  TargetLineNumber=num;
}

void GotoStmt::execute(EvalState &state, Program &program) {
  Statement *tmp=program.getParsedStatement(TargetLineNumber);
  program.cur_line=TargetLineNumber;
  tmp->execute(state,program);
}

IfGotoStmt::IfGotoStmt(Expression *_l, Expression *_r, std::string &_op, int num):op(_op),TargetLineNumber(num) {
  lhs=_l,rhs=_r;
}

IfGotoStmt::~IfGotoStmt() {
  delete lhs;
  delete rhs;
}

void IfGotoStmt::execute(EvalState &state, Program &program) {
  int l_value=lhs->eval(state),r_value=rhs->eval(state);
  if((op=="<"&&l_value<r_value)||(op=="="&&l_value==r_value)||(op==">"&&l_value>r_value)) {
    Statement *tmp=program.getParsedStatement(TargetLineNumber);
    program.cur_line=TargetLineNumber;
    tmp->execute(state,program);
  }
}