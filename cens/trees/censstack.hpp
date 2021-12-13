#ifndef CENS_CENSSTACK_HPP_
#define CENS_CENSSTACK_HPP_

#include <string>
#include <vector>

#include "../LL1Settings.hpp"

namespace CENS {

class CensStack {
 private:
  std::vector<std::vector<std::string>> gramatica;
  std::vector<std::vector<std::string>> table;

 public:
  std::vector<std::string> main_stack;

  CensStack(std::string first) {
    main_stack.resize(1);
    main_stack[0] = first;
    gramatica = gramaticaLL1;
    table = tablaLL1;
  }
  std::string TOS() {
    return main_stack[static_cast<int>(main_stack.size()) - 1];
  }
  bool IsTOSTerminal() {
    int size_stack = main_stack.size();
    int size_non_terminals = table.size();
    for (int i = 1; i < size_non_terminals; i++) {
      if (main_stack[size_stack - 1] == table[i][0]) {
        return false;
      }
    }
    return true;
  }
  bool ExistTOSExpand(std::string word, int &line_of_gramatica,
                      std::vector<std::string> &error_logs) {
    int column = -1;
    int row = -1;
    int size_non_terminals = table.size();
    int size_terminals = table[0].size();
    int size_stack = main_stack.size();
    for (int i = 1; i < size_non_terminals; i++) {
      if (main_stack[size_stack - 1] == table[i][0]) {
        row = i;
        break;
      }
    }
    for (int i = 0; i < size_terminals; i++) {
      if (word == table[0][i]) {
        column = i;
        break;
      }
    }
    if (row == -1 || column == -1) {
      error_logs.push_back("scanner error detected");
      return false;
    }
    if (table[row][column] != " ") {
      line_of_gramatica = stoi(table[row][column]);
      return true;
    } else {
      for (int i = 1; i < size_terminals; i++) {
        if (table[row][i] != " ") {
          error_logs.push_back(table[0][i] + ",");
        }
      }
      return false;
    }
  }
  void RecoverError(std::string word, std::vector<std::string> &error_logs) {
    int column = -1;
    int row = -1;
    int size_non_terminals = table.size();
    int size_terminals = table[0].size();
    int size_stack = main_stack.size();
    for (int i = 1; i < size_non_terminals; i++) {
      if (main_stack[size_stack - 1] == table[i][0]) {
        row = i;
        break;
      }
    }
    for (int i = 0; i < size_terminals; i++) {
      if (word == table[0][i]) {
        column = i;
        break;
      }
    }
    if (row == -1 || column == -1) {
      error_logs.push_back("scanner error detected");
    }

    for (int i = 1; i < size_terminals; i++) {
      if (table[row][i] != " ") {
        error_logs.push_back(table[0][i] + ",");
      }
    }
  }
  void AddExpand(const int line_of_gramatica) {
    int size_lines_gramatica = gramatica[line_of_gramatica].size();
    for (int i = size_lines_gramatica - 1; i >= 1; i--) {
      main_stack.push_back(gramatica[line_of_gramatica][i]);
    }
  }
  void PopStack() { main_stack.pop_back(); }
  void PushStartSymbol() { main_stack.push_back(gramatica[0][0]); }
  int Size() { return main_stack.size(); }
};

}  // namespace CENS

#endif  // CENS_CENSSTACK_HPP_