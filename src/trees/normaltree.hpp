#ifndef CENS_NORMALTREE_HPP_
#define CENS_NORMALTREE_HPP_

#include <string>
#include <vector>
#include <iostream>

namespace CENS {
class NormalTree {
 private:
  std::vector<std::string> tree;

 public:
  NormalTree() { tree.push_back(" "); }
  void Add(std::vector<std::string> &stack) {
    std::string instance_of_stack = " ";
    for (auto &item : stack) {
      instance_of_stack += item + " ";
    }
    tree.push_back(instance_of_stack);
  }
  void Print() {
    for (auto &item : tree) {
      std::cout << item << std::endl;
    }
  }
};
}  // namespace CENS

#endif  // CENS_NORMALTREE_HPP_