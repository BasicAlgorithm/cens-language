#ifndef CENS_ASTEXECUTOR_HPP_
#define CENS_ASTEXECUTOR_HPP_

#include <iostream>
#include <vector>

class ASTExecutorNode {
 public:
  std::string node_name;
  std::string type;
  int intensity = -65;
  int input = 0;

  std::vector<ASTExecutorNode *> childrens;
};

class ASTExecutor {
  ASTExecutorNode *head = NULL;

 public:
  ASTExecutor() {}
};

#endif  // CENS_ASTEXECUTOR_HPP_