/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"



Program::Program() = default;

Program::~Program() = default;

void Program::clear() {
    cur_line=-1;
    line_numbers.clear();//vector
    line_strings.clear();//vector
    for(auto i:line_statements) {
      delete i; //Statement *
    }
    line_statements.clear();//vector<Statement *>
    lines.clear();//map int -> int
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
  auto it = lines.find(lineNumber);
  if (it == lines.end()) {
    lines.insert(std::pair<int, int>(lineNumber, line_numbers.size()));
    line_strings.push_back(line);
    line_numbers.push_back(lineNumber);
    line_statements.push_back(nullptr);
  } else {
    int index = lines[lineNumber];
    line_strings[index] = line;
    line_numbers[index] = lineNumber;
    delete line_statements[index];
    line_statements[index] = nullptr;
  }
}

void Program::removeSourceLine(int lineNumber) {
  auto it = lines.find(lineNumber);
  if (it == lines.end()) return;
  else {
    lines.erase(lineNumber);
  }
}

std::string Program::getSourceLine(int lineNumber) {
  auto it=lines.find(lineNumber);
  if(it==lines.end()) return "";
  else {
    return line_strings[lines[lineNumber]];
  }
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
  auto it = lines.find(lineNumber);
  if (it == lines.end()) {
    error("No such line number to set");
  } else {
    while (lines[lineNumber] >= line_statements.size())
      line_statements.push_back(nullptr);
    line_statements[lines[lineNumber]] = stmt;
  }
}

//void Program::removeSourceLine(int lineNumber) {

Statement *Program::getParsedStatement(int lineNumber) {
  auto it = lines.find(lineNumber);
  if (it == lines.end()) {
    error("LINE NUMBER ERROR");
  } else {
    if (lines[lineNumber] >= line_statements.size())
      line_statements.resize(lines[lineNumber] + 1);
    return line_statements[lines[lineNumber]];
  }
}

int Program::getFirstLineNumber() {
  if (lines.empty()) return -1;
  return (*lines.begin()).first;
}

int Program::getNextLineNumber(int lineNumber) {
  auto it = lines.find(lineNumber);
  if (it == lines.end()) return -1;
  it++;
  if (it == lines.end()) return -1;
  return (*it).first;
}

void Program::run(EvalState &state, Program &program) {
  if (lines.empty()) return;
  cur_line = getFirstLineNumber();
  while (true) {
    line_statements[lines[cur_line]]->execute(state, program);//execute can change the cur_line
    cur_line = getNextLineNumber(cur_line);
    if(cur_line==-1) return;
  }
}

void Program::list() {
  auto it=lines.begin();
  for(;it!=lines.end();it++) {
    std::cout<<getSourceLine((*it).first)<<'\n';
  }
}


