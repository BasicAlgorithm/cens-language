#ifndef CENS_AST_HPP_
#define CENS_AST_HPP_

#include <math.h>

#include <cstring>
#include <future>
#include <iostream>
#include <list>
#include <thread>
#include <vector>

#include "../LL1Settings.hpp"
#include "../scanner.hpp"
#include "../trees/beautytree.h"
#include "../trees/censnode.hpp"
#include "ASTExecutor.hpp"

class ASTNode {
 public:
  std::string node_name;
  std::vector<ASTNode *> childrens;

  ASTNode() {}
  ASTNode(std::string info) : node_name(info) {}
};

bool ParallelSearch(std::vector<ASTNode> &ast_nodes, int t_id, int size,
                    std::string neuron_name) {
  int init = size * t_id;
  int end = init + size;
  for (int i = init; i < end; i++) {
    if (ast_nodes[i].node_name == neuron_name) return true;
  }
  return false;
}

struct OutputNode {
  std::string name;
  int value;
  OutputNode(std::string n, int v) : name(n), value(v) {}
};

class AST {
 public:
  ASTNode *head = NULL;
  std::vector<ASTNode *> ast_nodes;
  std::vector<ASTExecutorNode *> executable_neurons;
  std::vector<OutputNode> neurons_output;
  std::vector<std::string> matlab_files;

  void PrintAllNeurons() {
    std::cout << "PRINT ALL NEURON" << std::endl;
    for (auto &neuron : ast_nodes) {
      std::cout << "NODE: " << neuron->node_name << "("
                << neuron->childrens.size() << ")" << std::endl;
      for (auto &neuron_id : neuron->childrens) {
        std::cout << "->: " << neuron_id->node_name << "("
                  << neuron_id->childrens.size() << ")" << std::endl;
        for (auto &n : neuron_id->childrens) {
          std::cout << "->->: " << n->node_name << " " << std::endl;
        }
      }
    }
  }

  bool ExistSendCurrentNode(ASTNode *&p) {
    std::string sc = "SendCurrent";
    for (auto &neuron : ast_nodes) {
      if (neuron->node_name == sc) {
        p = neuron;
        return true;
      }
    }
    return false;
  }

  void MakeNothing(int rule) {
    std::cout << "MakeNothing() Rule " << rule << std::endl;
  }

  // Neuron create Methods
  bool ExistNeuron(std::string neuron_name) {
    std::cout << "DEBUG" << std::endl;
    auto atomic_count = [&](volatile std::atomic<bool> *exist_n,
                            const auto &t_id, int size,
                            std::string neuron_name) -> void {
      int init = size * t_id;
      int end = init + size;
      for (int i = init; i < end; i++) {
        for (auto &n : ast_nodes[i]->childrens) {
          if (n->node_name == neuron_name) *exist_n = true;
        }
      }
    };

    int num_threads = 2;
    int size = static_cast<int>(ast_nodes.size()) / num_threads;
    size = static_cast<int>(floor(size));
    std::vector<std::thread> threads;

    std::atomic<bool> exist_neuron(false);

    for (int id = 0; id < num_threads; id++)
      threads.emplace_back(atomic_count, &exist_neuron, id, size, neuron_name);

    for (auto &thread : threads) thread.join();

    return exist_neuron ? true : false;
  }
  void MakeNeuronNode() {
    ASTNode *neuron_node = new ASTNode("neuron");
    ast_nodes.emplace_back(std::move(neuron_node));
  }
  void MakeNeuronIdNode(std::string neuron_name) {
    ASTNode *neuron_node = new ASTNode(neuron_name);
    auto tmp = ast_nodes.rbegin();
    (*tmp)->childrens.emplace_back(std::move(neuron_node));
  }
  void MakeInhibitoryNode() {
    ASTNode *type_node = new ASTNode("Inhibitory");
    auto tmp = ast_nodes.rbegin();
    (*tmp)->childrens.emplace_back(std::move(type_node));
  }
  void MakeExcitatoryNode() {
    ASTNode *type_node = new ASTNode("Excitatory");
    auto tmp = ast_nodes.rbegin();
    (*tmp)->childrens.emplace_back(std::move(type_node));
  }
  void MakeIntensityNeuronNode(std::string intensity) {
    ASTNode *intensity_node = new ASTNode(intensity);
    auto tmp = ast_nodes.rbegin();
    (*tmp)->childrens.emplace_back(std::move(intensity_node));
  }
  void MakeInnervationNode() {
    ASTNode *innveration_node = new ASTNode("innervation");
    auto tmp = ast_nodes.rbegin();
    (*tmp)->childrens.emplace_back(std::move(innveration_node));
  }

  // Innervation Methods
  bool ExistNeuronPointer(std::string neuron_name, ASTNode *&p) {
    for (auto &neuron : ast_nodes) {
      p = neuron;
      for (auto &neuron_id : neuron->childrens) {
        // std::cout << "LOOP: " << neuron_id->node_name << " = " << neuron_name
        //           << std::endl;
        if (neuron_id->node_name == neuron_name) {
          return true;
        }
      }
    }
    p = NULL;
    return false;
  }

  bool MakeInnervationConnection(std::string neuron_id_1,
                                 std::string neuron_id_2) {
    ASTNode *node_1 = NULL;
    ASTNode *node_2 = NULL;
    if (ExistNeuronPointer(neuron_id_1, node_1) &&
        ExistNeuronPointer(neuron_id_2, node_2)) {
      //  cout to delete
      std::cout << "What We found node_1: \n"
                << node_1->node_name << " " << node_1->childrens[0]->node_name
                << std::endl;
      std::cout << "What We found node_2: \n"
                << node_2->node_name << " " << node_2->childrens[0]->node_name
                << std::endl;

      int s_n1 = node_1->childrens.size();
      node_1 = node_1->childrens[s_n1 - 1];
      node_1->childrens.emplace_back(std::move(node_2));
      return true;
    } else {
      if (node_1 == NULL)
        std::cout << "ERROR: MakeInnervationConnection() 1 Not exist "
                  << neuron_id_1 << " neuron" << std::endl;
      if (node_2 == NULL)
        std::cout << "ERROR: MakeInnervationConnection() 2 Not exist "
                  << neuron_id_2 << " neuron" << std::endl;
    }
    return false;
  }

  // SendCurrent Methods
  bool MakeSendCurrentNode() {
    // if already SendCurrent node exist
    auto tmp = ast_nodes.rbegin();
    if ((*tmp)->node_name == "SendCurrent") {
      return false;
    }
    // if not, then create a unique SendCurrent Node
    ASTNode *sendcurrent_node = new ASTNode("SendCurrent");
    ast_nodes.emplace_back(std::move(sendcurrent_node));
    return true;
  }
  bool MakeSendCurrentConnection(std::string neuron_id) {
    ASTNode *node_1 = NULL;
    if (ExistNeuronPointer(neuron_id, node_1)) {
      auto sendcurrent_node = ast_nodes.rbegin();
      (*sendcurrent_node)->childrens.emplace_back(std::move(node_1));
      return true;
    } else {
      std::cout << "ERROR: MakeSendCurrentConnection() Not exist " << neuron_id
                << " neuron" << std::endl;
    }
    return false;
  }

  // RunSystem Methods
  void MakeRunSystemNode() {
    auto sendcurrent_node = ast_nodes.rbegin();

    ASTNode *runsystem_node = new ASTNode("RunSystem");
    ast_nodes.emplace_back(std::move(runsystem_node));

    auto rs_node = ast_nodes.rbegin();
    ASTNode *sc_n = &(**sendcurrent_node);
    (*rs_node)->childrens.emplace_back(std::move(sc_n));
  }

  bool ExecuteRule(int rule_id, int steps, int line) {
    int line_size = static_cast<int>(CENS::clean_code[line].size());
    std::string p0 = CENS::clean_code[line][steps - 1];
    std::string p1 = "--";
    std::string p2 = "--";
    std::string p3 = "--";
    if (steps < line_size) {
      p1 = CENS::clean_code[line][steps];
    }
    steps++;
    if (steps < line_size) {
      p2 = CENS::clean_code[line][steps];
    }
    steps++;
    if (steps < line_size) {
      p3 = CENS::clean_code[line][steps];
    }
    // std::cout << "Analizing: p0 = " << p0) <<
    // std::endl; std::cout << "Analizing: p1 = " << p1)
    // << std::endl; std::cout << "Analizing: p2 = " <<
    // p2) << std::endl; std::cout << "Analizing: p3 = "
    // << p3) << std::endl;
    bool is_success = true;
    switch (rule_id + 1) {
      case 1: {
        MakeNothing(1);
        break;
      }
      case 2: {
        std::cout << "AST: Rule 2" << std::endl;
        MakeNeuronNode();
        break;
      }
      case 3: {
        MakeNothing(3);
        break;
      }
      case 4: {
        std::cout << "AST: Rule 4" << std::endl;
        MakeSendCurrentNode();
        break;
      }
      case 5: {
        MakeNothing(5);
        break;
      }
      case 6: {
        std::cout << "AST: Rule 6" << std::endl;
        MakeNeuronNode();
        break;
      }
      case 7: {
        MakeNothing(7);
        break;
      }
      case 8: {
        std::cout << "AST: Rule 8" << std::endl;
        MakeSendCurrentNode();
        break;
      }
      case 9: {
        MakeNothing(9);
        break;
      }
      case 10: {
        std::cout << "AST: Rule 10" << std::endl;
        if (!ExistNeuron(p2))
          MakeNeuronIdNode(p2);
        else {
          std::cout << "ERROR: " << p2 << " neuron has already been created."
                    << std::endl;
          return false;
        }
        break;
      }
      case 11: {
        std::cout << "AST: Rule 11" << std::endl;
        MakeInhibitoryNode();
        break;
      }
      case 12: {
        std::cout << "AST: Rule 12" << std::endl;
        MakeExcitatoryNode();
        break;
      }
      case 13: {
        std::cout << "AST: Rule 13" << std::endl;
        MakeIntensityNeuronNode(p2);
        MakeInnervationNode();
        break;
      }
      case 14: {
        std::cout << "AST: Rule 14" << std::endl;
        MakeInnervationNode();
        break;
      }
      case 15: {
        std::cout << "AST: Rule 15" << std::endl;
        std::cout << "case 15: p1 = " << p1 << std::endl;
        std::cout << "case 15: p3 = " << p3 << std::endl;

        is_success = MakeInnervationConnection(p1, p3);
        break;
      }
      case 16: {
        std::cout << "AST: Rule 16" << std::endl;
        std::cout << "case 16: p0 = " << p0 << std::endl;
        std::cout << "case 16: p1 = " << p2 << std::endl;
        is_success = MakeInnervationConnection(p0, p2);
        break;
      }
      case 17: {
        MakeNothing(17);
        break;
      }
      case 18: {
        std::cout << "AST: Rule 18" << std::endl;
        std::cout << "case 18: p3 = " << p3 << std::endl;
        is_success = MakeSendCurrentConnection(p3);
        break;
      }
      case 19: {
        std::cout << "AST: Rule 19" << std::endl;
        std::cout << "case 19: p2 = " << p2 << std::endl;
        is_success = MakeSendCurrentConnection(p2);
        break;
      }
      case 20: {
        MakeNothing(20);
        break;
      }
      case 21: {
        std::cout << "AST: Rule 21" << std::endl;
        MakeRunSystemNode();
        break;
      }
      default:
        break;
    }
    return is_success;
  }

  // Calculating real graph and results
  void AddChildrens(ASTExecutorNode *&father, ASTNode *ast_node) {
    // TODO first looking if node on executables_neurons already exist
    if (ast_node->node_name == "SendCurrent") {
      for (auto &child : ast_node->childrens) {
        AddChildrens(father, child);
      }
    } else if (ast_node->node_name == "neuron") {
      ASTExecutorNode *tmp_child = new ASTExecutorNode;
      tmp_child->node_name = ast_node->childrens[0]->node_name;
      tmp_child->type = ast_node->childrens[1]->node_name;
      if (ast_node->childrens.size() == 3) {
        executable_neurons.push_back(tmp_child);
        father->childrens.push_back(tmp_child);
        for (auto &child : ast_node->childrens[2]->childrens) {
          AddChildrens(tmp_child, child);
        }
      } else if (ast_node->childrens.size() == 4) {
        tmp_child->intensity = std::stoi(ast_node->childrens[2]->node_name);
        executable_neurons.push_back(tmp_child);
        father->childrens.push_back(tmp_child);
        for (auto &child : ast_node->childrens[3]->childrens) {
          AddChildrens(tmp_child, child);
        }
      } else {
        std::cout << "ERROR: AddChildrens() Unknow size of childrens  "
                  << std::endl;
      }
    } else {
      std::cout << "ERROR: AddChildrens() Not SendCurrent or Neuron  "
                << std::endl;
    }
  }

  std::list<ASTExecutorNode *> amplitud_input;
  bool ExistNeuronOnAmplitudInput(ASTExecutorNode *neuron) {
    for (auto &i : amplitud_input) {
      if (i->node_name == neuron->node_name) return true;
    }
    return false;
  }
  void InputStimulus() {
    if (amplitud_input.size() == 0) return;
    ASTExecutorNode *father_node = &(**amplitud_input.begin());
    for (auto &neuron : father_node->childrens) {
      if (father_node->type == "Inhibitory") {
        neuron->input -= father_node->intensity + father_node->input;

      } else if (father_node->type == "Excitatory") {
        neuron->input += father_node->intensity + father_node->input;

      } else {
        std::cout << "ERROR: InputStimulus() Type not identified " << std::endl;
      }
      if (!ExistNeuronOnAmplitudInput(neuron)) {
        amplitud_input.push_back(neuron);
      }
    }
    amplitud_input.pop_front();
    InputStimulus();
  }

  void OutputResults(ASTExecutorNode *&head) {
    if (head->childrens.size() == 0) {
      neurons_output.push_back(
          OutputNode(head->node_name, head->input + head->intensity));
    }
    for (auto &neuron : head->childrens) {
      std::cout << "Neuron id = " << neuron->node_name
                << " | input = " << neuron->input
                << " | intensity = " << neuron->intensity
                << " | output = " << neuron->input + neuron->intensity
                << " | Threahold = "
                << ((neuron->input + neuron->intensity) >= -40 ? "yes" : "not")
                << std::endl;
      OutputResults(neuron);
    }
  }

  template <typename T>
  void AddChildrensBeautyTreeAST(T *head_ast, CensNode *head_beauty_tree) {
    CensNode *tmp = new CensNode(head_ast->node_name);
    head_beauty_tree->addChildren(tmp);
    for (auto &node : head_ast->childrens) {
      AddChildrensBeautyTreeAST(node, tmp);
    }
  }

  void CreateNeuronGraphExecutable() {
    ASTNode *sendcurrent = NULL;
    if (!ExistSendCurrentNode(sendcurrent)) {
      std::cout
          << "ERROR: CreateNeuronGraphExecutable() no SendCurrent node found"
          << std::endl;
      return;
    }

    // beauty tree AST
    CensNode *head_beauty_tree = new CensNode("RunSystem");
    AddChildrensBeautyTreeAST<ASTNode>(sendcurrent, head_beauty_tree);
    BeautyTree<CensNode> printer(head_beauty_tree, &CensNode::getChildren,
                                 &CensNode::getData);
    std::cout << "\n\t===== ABSTRACT SYNTAX TREE =====\n" << std::endl;
    printer.print();

    // Executor Graph
    ASTExecutorNode *head = new ASTExecutorNode;
    head->node_name = "CENS-Root";
    head->intensity = 10;
    head->type = "Excitatory";
    executable_neurons.push_back(head);

    AddChildrens(head, sendcurrent);

    // beauty tree Executor Graph
    CensNode *head_beauty_tree_g = new CensNode("");
    AddChildrensBeautyTreeAST<ASTExecutorNode>(head, head_beauty_tree_g);
    BeautyTree<CensNode> printer_g(head_beauty_tree_g, &CensNode::getChildren,
                                   &CensNode::getData);
    std::cout << "\n\t===== EXECUTABLE GRAPH =====\n" << std::endl;
    printer_g.print();

    amplitud_input.push_back(head);
    InputStimulus();

    // Output print
    std::cout << head->node_name << " | Initial Stimulus = " << head->intensity
              << std::endl;
    OutputResults(head);

    // Output Matlab File

    for (auto &out : neurons_output) {
      std::string file_results_path_ = "neuron_" + out.name + "_graph.m";
      std::ofstream file_results(file_results_path_, std::ofstream::out);
      std::string base_code_matlab;
      int lenght = 0;
      if (out.value >= -40) {
        lenght = out.value + 50;
        lenght = std::min(lenght, 200);
      }
      base_code_matlab =
          "function cens_neuron_graph\n"
          "Vrest = 0;\n"
          "dt = 0.01;\n"
          "totalTime = " +
          std::to_string(lenght) + "; % ms\n" + CENS::parte_1;
      std::string tmp_title = "figure('Name', 'Neuron " + out.name + "')\n";
      std::string tmp_graph = tmp_title + CENS::graphic;
      base_code_matlab += tmp_graph + CENS::parte_2;
      file_results << base_code_matlab;

      // creating bash code to run matlab graph
      matlab_files.push_back(
          "matlab -nodisplay -nosplash -nodesktop -r \"run('" +
          file_results_path_ + "');\"");
      file_results.close();
    }
  }

  void MatlabGraphics() {
    for (auto &run : matlab_files) {
      int size = run.size();
      char *to_run = new char[size];
      strcpy(to_run, run.c_str());
      std::cout << " execute: " << to_run << std::endl;
      system(to_run);
    }
  }
};

//========================================================================

#endif  // CENS_AST_HPP_
