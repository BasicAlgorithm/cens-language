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

#include "AST/AST.hpp"
#include "LL1Settings.hpp"
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
  std::cout << "ERROR " << type_error << ". " << std::endl;
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
  // AST
  AST ast_tree;
  bool well_finish_semantic = true;

  // Loop to analize each line of code
  for (int i = 0; i < static_cast<int>(output_scanner.size()); i++) {
    CensStack stack("$");
    NormalTree tree;
    stack.PushStartSymbol();
    std::vector<std::string>::iterator word = output_scanner[i].begin() + 1;

    // beauty tree
    CensNode *head_beauty_tree = new CensNode(gramaticaLL1[0][0]);
    std::list<CensNode *> tmp_list_beauty_tree{head_beauty_tree};

    // couts
    bool well_finish_sintactic = true;
    bool ast_success = true;
    std::vector<std::string> error_logs;

    if (print_info) {
      std::cout << "\n--> Parsing: ";
      for (int j = 1; j < static_cast<int>(output_scanner[i].size() - 1); j++) {
        std::cout << output_scanner[i][j] << " ";
      }
      std::cout << std::endl;
    }

    // Analizing one line of code
    int steps = 1;
    while (true) {
      tree.Add(stack.main_stack);

      if ((stack.Size() == 1 && (word == --output_scanner[i].end())) &&
          (stack.TOS() == *word)) { /* end of reading file */
        if (print_info)
          std::cout << "Parser finished successfully" << std::endl;
        break;

      } else if (stack.IsTOSTerminal()) { /* TOS is terminal */
        if (stack.TOS() == *word) {
          // add a node to beauty tree
          (*tmp_list_beauty_tree.begin())->data =
              "[" + (*tmp_list_beauty_tree.begin())->data + "]";
          tmp_list_beauty_tree.pop_front();

          stack.PopStack();
          word++;
          steps++;
        } else {
          well_finish_sintactic = false;
          well_finish_semantic = false;
          HandlingError(word, i, stack.TOS(), error_logs, true);
          break;
        }

      } else { /* TOS is non terminal */
        int line_of_gramatica = -1;
        if (stack.ExistTOSExpand(*word, line_of_gramatica, error_logs)) {
          stack.PopStack();
          stack.AddExpand(line_of_gramatica);

          // AST
          ast_success = ast_tree.ExecuteRule(line_of_gramatica, steps, i);
          if (CENS::print_info)
            std::cout << "ast_log: " << ast_success << std::endl;
          if (!ast_success) {
            well_finish_semantic = false;
            break;
          }

          // add a node to beauty tree
          CensNode *tmp_tmp = NULL;
          tmp_tmp = *tmp_list_beauty_tree.begin();
          tmp_list_beauty_tree.pop_front();
          int size_lines_gramatica = gramaticaLL1[line_of_gramatica].size();
          for (int i = size_lines_gramatica - 1; i > 0; i--) {
            CensNode *tmp = new CensNode(gramaticaLL1[line_of_gramatica][i]);
            tmp_tmp->addChildren(tmp);
            tmp_list_beauty_tree.push_front(tmp);
          }
        } else {
          well_finish_sintactic = false;
          well_finish_semantic = false;
          HandlingError(word, i, stack.TOS(), error_logs, false);
          break;
        }
      }
    }
    if (well_finish_sintactic && print_info) {
      tree.Print();
      BeautyTree<CensNode> printer(head_beauty_tree, &CensNode::getChildren,
                                   &CensNode::getData);
      printer.print();
    }
  }
  /* === SEMANTIC ANALYSIS ===*/
  if (!well_finish_semantic) return;

  // Because AST Graph is ready. Then, we can create executor graph
  ast_tree.CreateExecutorGraph();

  // When executor graph is ready. We can start simulation
  ast_tree.StartInitialStimulus();

  // When simulation finished. We can show results.
  ast_tree.PrintSimulationResults();

  // Optionally, we can create matlab code to each output neuron
  ast_tree.GenerateMatlabCode();

  // if -debug command was set up
  if (CENS::print_info) {
    ast_tree.PrintAST();
    ast_tree.PrintExecutorGraph();
  }

  // Only if user put a =graph command, automatically print matlab graphics
  if (CENS::print_matlab_graph) {
    ast_tree.MatlabGraphics();
  }
}

}  // namespace CENS

#endif  // CENS_PARSER_HPP