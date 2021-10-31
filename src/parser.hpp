#ifndef CENS_PARSER_HPP
#define CENS_PARSER_HPP

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

#include "LL1.hpp"
#include "scanner.hpp"
#include "trees/beautytree.h"
#include "trees/censnode.hpp"
#include "trees/censstack.hpp"
#include "trees/normaltree.hpp"

namespace CENS {

void HandlingError(std::vector<std::string>::iterator word, int index,
                   std::string tos, std::vector<std::string> error_logs,
                   bool type) {
  std::string type_error;
  if (type) {
    type_error = "TOS is Terminal";
  } else {
    type_error = "TOS is not Terminal";
  }
  std::cout << "error " << type_error << ". " << std::endl;
  // error handling
  int q_spaces = 0;
  bool flag = true;
  int distance = 0;
  std::cout << "Line ";
  q_spaces += 5;
  std::vector<std::string>::iterator it_out = output_scanner[index].begin();
  for (std::vector<std::string>::iterator it_clean = clean_code[index].begin();
       it_clean != clean_code[index].end(); it_clean++, it_out++) {
    std::cout << *it_clean << " ";
    if (it_clean == clean_code[index].begin()) {
      std::cout << " | ";
      q_spaces += 3;
    }
    if (it_out != word && flag) {
      distance++;
      q_spaces += it_clean->size();
      q_spaces++;
    } else {
      flag = false;
    }
  }
  std::cout << std::endl;
  std::string spaces(q_spaces, ' ');
  std::string signs((clean_code[index].begin() + distance)->size(), '~');

  std::cout << spaces << signs << std::endl;
  std::cout << spaces << "|" << std::endl;
  std::cout << spaces << "This " << tos
            << " token is not possible, maybe you try: ";

  // error cause by terminal
  if (type) {
    if (tos == "id") {
      std::cout << "identifier: [name of neuron]" << std::endl;
    } else if (IsOperator(tos)) {
      auto it = std::find(output_scanner[index].begin(),
                          output_scanner[index].end(), ")");
      if (it == output_scanner[index].end()) {
        if (output_scanner[index].size() == 4) {
          std::cout << "RunSystem" << std::endl;
        } else {
          std::cout << ">>" << std::endl;
        }

      } else {
        std::cout << "SendCurrent" << std::endl;
      }
    } else {
      std::cout << "We can't help you yet." << std::endl;
    }

    // error cause by not terminal
  } else {
    for (auto &token : error_logs) {
      std::cout << token << " ";
    }
    std::cout << std::endl;
  }
}

void RunParser() {
  // Loop to analize each line of code
  for (int i = 0; i < static_cast<int>(output_scanner.size()); i++) {
    CensStack stack("$");
    NormalTree tree;
    stack.PushStartSymbol();
    std::vector<std::string>::iterator word = output_scanner[i].begin() + 1;

    // beauty tree
    CensNode *head_beauty_tree = new CensNode(gramaticaLL1[0][0]);
    std::list<CensNode *> stack_nodes{head_beauty_tree};

    // couts
    bool well_finish = true;
    std::vector<std::string> error_logs;

    if (print_info) {
      std::cout << "\n--> Parsing: ";
      for (int j = 1; j < static_cast<int>(output_scanner[i].size() - 1); j++) {
        std::cout << output_scanner[i][j] << " ";
      }
      std::cout << std::endl;
    }

    // Analizing one line of code
    while (true) {
      tree.Add(stack.main_stack);
      if ((stack.Size() == 1 && (word == --output_scanner[i].end())) &&
          (stack.TOS() == *word)) { /* end of reading file */
        if (print_info)
          std::cout << "Parser finished successfully" << std::endl;
        break;

      } else if (stack.IsTOSTerminal()) { /* TOS is terminal */
        if (stack.TOS() == *word) {
          // add a beauty tree
          (*stack_nodes.begin())->data =
              "[" + (*stack_nodes.begin())->data + "]";
          stack_nodes.pop_front();

          stack.PopStack();
          word++;
        } else {
          well_finish = false;
          HandlingError(word, i, stack.TOS(), error_logs, true);
          break;
        }

      } else { /* TOS is non terminal */
        int line_of_gramatica = -1;
        if (stack.ExistTOSExpand(*word, line_of_gramatica, error_logs)) {
          stack.PopStack();
          stack.AddExpand(line_of_gramatica);

          // add a beauty tree
          CensNode *tmp_tmp = NULL;
          tmp_tmp = *stack_nodes.begin();
          stack_nodes.pop_front();
          int size_lines_gramatica = gramaticaLL1[line_of_gramatica].size();
          for (int i = size_lines_gramatica - 1; i > 0; i--) {
            CensNode *tmp = new CensNode(gramaticaLL1[line_of_gramatica][i]);
            tmp_tmp->addChildren(tmp);
            stack_nodes.push_front(tmp);
          }
        } else {
          well_finish = false;
          HandlingError(word, i, stack.TOS(), error_logs, false);
          break;
        }
      }
    }
    if (well_finish && print_info) {
      tree.Print();
      BeautyTree<CensNode> printer(head_beauty_tree, &CensNode::getChildren,
                                   &CensNode::getData);
      printer.print();
    }
  }
}

}  // namespace CENS

#endif  // CENS_PARSER_HPP