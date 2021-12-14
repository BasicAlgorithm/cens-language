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

struct OutputNode {
  std::string name;
  int value;
  OutputNode(std::string n, int v) : name(n), value(v) {}
};

class AST {
 private:
  ASTExecutorNode *head_executor = new ASTExecutorNode;
  std::vector<ASTNode *> ast_nodes;
  std::vector<ASTExecutorNode *> executable_neurons;
  std::vector<OutputNode> neurons_output;
  std::vector<std::string> matlab_files;
  std::vector<std::string> matlab_paths;
  int current_line;
  int current_step;

 public:
  void BasicPrintASTNodes() {
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

  void MakeNothing(int rule) {}

  // Neuron create Methods
  bool ExistNeuron(std::string neuron_name) {
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
  bool MakeNeuronNode() {
    ASTNode *neuron_node = new ASTNode("neuron");
    ast_nodes.emplace_back(std::move(neuron_node));
    return true;
  }
  bool MakeNeuronIdNode(std::string neuron_name) {
    if (!ExistNeuron(neuron_name)) {
      ASTNode *neuron_node = new ASTNode(neuron_name);
      auto tmp = ast_nodes.rbegin();
      (*tmp)->childrens.emplace_back(std::move(neuron_node));
      return true;
    } else {
      std::cout << "ERROR: " << neuron_name
                << " neuron has already been created." << std::endl;
      return false;
    }
  }
  bool MakeInhibitoryNode() {
    ASTNode *type_node = new ASTNode("Inhibitory");
    auto tmp = ast_nodes.rbegin();
    (*tmp)->childrens.emplace_back(std::move(type_node));
    return true;
  }
  bool MakeExcitatoryNode() {
    ASTNode *type_node = new ASTNode("Excitatory");
    auto tmp = ast_nodes.rbegin();
    (*tmp)->childrens.emplace_back(std::move(type_node));
    return true;
  }
  bool MakeIntensityNeuronNode(std::string intensity) {
    ASTNode *intensity_node = new ASTNode(intensity);
    auto tmp = ast_nodes.rbegin();
    (*tmp)->childrens.emplace_back(std::move(intensity_node));
    return true;
  }
  bool MakeInnervationNode() {
    ASTNode *innveration_node = new ASTNode("innervation");
    auto tmp = ast_nodes.rbegin();
    (*tmp)->childrens.emplace_back(std::move(innveration_node));
    return true;
  }

  // Innervation Methods
  bool ExistNeuronPointer(std::string neuron_name, ASTNode *&p) {
    for (auto &neuron : ast_nodes) {
      p = neuron;
      for (auto &neuron_id : neuron->childrens) {
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
      int s_n1 = node_1->childrens.size();
      node_1 = node_1->childrens[s_n1 - 1];
      node_1->childrens.emplace_back(std::move(node_2));
      return true;
    } else {
      // handling error
      ExistNeuronPointer(neuron_id_1, node_1);
      if (node_1 == NULL) {
        std::cout << "ERROR: When innervate neurons\nLine ";
        // new handling
        int q_spaces = 0;
        bool flag = true;
        int distance = 0;
        q_spaces += 5;
        for (std::vector<std::string>::iterator it_clean =
                 CENS::clean_code[current_line].begin();
             it_clean != CENS::clean_code[current_line].end(); it_clean++) {
          std::cout << *it_clean << " ";
          if (it_clean == CENS::clean_code[current_line].begin()) {
            std::cout << " | ";
            q_spaces += 3;
          }
          if (*it_clean != neuron_id_1 && flag) {
            distance++;
            q_spaces += it_clean->size();
            q_spaces++;
          } else {
            flag = false;
          }
        }
        std::cout << std::endl;
        std::string spaces(q_spaces, ' ');
        std::string signs(
            (CENS::clean_code[current_line].begin() + distance)->size(), '~');

        std::cout << spaces << signs << std::endl;
        std::cout << spaces << "|" << std::endl;
        std::cout << spaces << "Neuron id: " << neuron_id_1
                  << " have not been created. Try: neuron " << neuron_id_1
                  << " ( parameters )" << std::endl;
        // new handling
      }

      ExistNeuronPointer(neuron_id_2, node_2);
      if (node_2 == NULL) {
        std::cout << "ERROR: When innervate neurons\nLine ";
        // new handling
        int q_spaces = 0;
        bool flag = true;
        int distance = 0;
        q_spaces += 5;
        for (std::vector<std::string>::iterator it_clean =
                 CENS::clean_code[current_line].begin();
             it_clean != CENS::clean_code[current_line].end(); it_clean++) {
          std::cout << *it_clean << " ";
          if (it_clean == CENS::clean_code[current_line].begin()) {
            std::cout << " | ";
            q_spaces += 3;
          }
          if (*it_clean != neuron_id_2 && flag) {
            distance++;
            q_spaces += it_clean->size();
            q_spaces++;
          } else {
            flag = false;
          }
        }
        std::cout << std::endl;
        std::string spaces(q_spaces, ' ');
        std::string signs(
            (CENS::clean_code[current_line].begin() + distance)->size(), '~');

        std::cout << spaces << signs << std::endl;
        std::cout << spaces << "|" << std::endl;
        std::cout << spaces << "Neuron id: " << neuron_id_2
                  << " have not been created. Try: neuron " << neuron_id_2
                  << " ( parameters )" << std::endl;
        // new handling
      }
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
      ExistNeuronPointer(neuron_id, node_1);
      if (node_1 == NULL) {
        std::cout << "ERROR: When define input neurons\nLine ";
        // new handling
        int q_spaces = 0;
        bool flag = true;
        int distance = 0;
        q_spaces += 5;
        for (std::vector<std::string>::iterator it_clean =
                 CENS::clean_code[current_line].begin();
             it_clean != CENS::clean_code[current_line].end(); it_clean++) {
          std::cout << *it_clean << " ";
          if (it_clean == CENS::clean_code[current_line].begin()) {
            std::cout << " | ";
            q_spaces += 3;
          }
          if (*it_clean != neuron_id && flag) {
            distance++;
            q_spaces += it_clean->size();
            q_spaces++;
          } else {
            flag = false;
          }
        }
        std::cout << std::endl;
        std::string spaces(q_spaces, ' ');
        std::string signs(
            (CENS::clean_code[current_line].begin() + distance)->size(), '~');

        std::cout << spaces << signs << std::endl;
        std::cout << spaces << "|" << std::endl;
        std::cout << spaces << "Neuron id: " << neuron_id
                  << " have not been created. Try: neuron " << neuron_id
                  << " ( parameters )" << std::endl;
        // new handling
      }
    }
    return false;
  }

  // RunSystem Methods
  bool MakeRunSystemNode() {
    auto sendcurrent_node = ast_nodes.rbegin();
    if (!((*sendcurrent_node)->node_name == "SendCurrent")) {
      std::cout << "ERROR: MakeRunSystemNode() Last node must be SendCurrent"
                << std::endl;
      return false;
    }

    ASTNode *runsystem_node = new ASTNode("RunSystem");
    ast_nodes.emplace_back(std::move(runsystem_node));

    auto rs_node = ast_nodes.rbegin();
    ASTNode *sc_n = &(**sendcurrent_node);
    (*rs_node)->childrens.emplace_back(std::move(sc_n));
    return true;
  }

  bool ExecuteRule(int rule_id, int steps, int line) {
    // upgrade global variables
    current_line = line;
    current_step = steps;

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

    if (CENS::print_info) std::cout << "AST: Rule " << rule_id + 1 << std::endl;
    bool is_success = true;

    switch (rule_id + 1) {
      case 1: {
        MakeNothing(1);
        break;
      }
      case 2: {
        is_success = MakeNeuronNode();
        break;
      }
      case 3: {
        MakeNothing(3);
        break;
      }
      case 4: {
        MakeSendCurrentNode();
        break;
      }
      case 5: {
        MakeNothing(5);
        break;
      }
      case 6: {
        is_success = MakeNeuronNode();
        break;
      }
      case 7: {
        MakeNothing(7);
        break;
      }
      case 8: {
        MakeSendCurrentNode();
        break;
      }
      case 9: {
        MakeNothing(9);
        break;
      }
      case 10: {
        is_success = MakeNeuronIdNode(p2);
        break;
      }
      case 11: {
        is_success = MakeInhibitoryNode();
        break;
      }
      case 12: {
        is_success = MakeExcitatoryNode();
        break;
      }
      case 13: {
        is_success = MakeIntensityNeuronNode(p2);
        is_success = MakeInnervationNode();
        break;
      }
      case 14: {
        is_success = MakeInnervationNode();
        break;
      }
      case 15: {
        is_success = MakeInnervationConnection(p1, p3);
        break;
      }
      case 16: {
        is_success = MakeInnervationConnection(p0, p2);
        break;
      }
      case 17: {
        MakeNothing(17);
        break;
      }
      case 18: {
        is_success = MakeSendCurrentConnection(p3);
        break;
      }
      case 19: {
        is_success = MakeSendCurrentConnection(p2);
        break;
      }
      case 20: {
        MakeNothing(20);
        break;
      }
      case 21: {
        MakeRunSystemNode();
        break;
      }
      default:
        break;
    }
    return is_success;
  }

  bool ExistOnExecutableNeurons(std::string neuron_name,
                                ASTExecutorNode *&node) {
    for (auto &n : executable_neurons) {
      if (n->node_name == neuron_name) {
        node = n;
        return true;
      }
    }
    return false;
  }
  // Calculating real graph and results
  void AddExecutorNode(ASTExecutorNode *&father, ASTNode *ast_node) {
    if (ast_node->node_name == "SendCurrent") {
      for (auto &child : ast_node->childrens) {
        AddExecutorNode(father, child);
      }
    } else if (ast_node->node_name == "neuron") {
      // check if neuron was already created
      ASTExecutorNode *tmp_child = NULL;
      if (ExistOnExecutableNeurons(ast_node->childrens[0]->node_name,
                                   tmp_child)) {
        father->childrens.push_back(tmp_child);
        for (auto &child :
             ast_node->childrens[ast_node->childrens.size() - 1]->childrens) {
          AddExecutorNode(tmp_child, child);
        }
        // if not, we create a new ASTExecutorNode
      } else {
        tmp_child = new ASTExecutorNode;
        tmp_child->node_name = ast_node->childrens[0]->node_name;
        tmp_child->type = ast_node->childrens[1]->node_name;
        if (ast_node->childrens.size() == 4) {
          tmp_child->intensity = std::stoi(ast_node->childrens[2]->node_name);
        }
        executable_neurons.push_back(tmp_child);
        father->childrens.push_back(tmp_child);
        for (auto &child :
             ast_node->childrens[ast_node->childrens.size() - 1]->childrens) {
          AddExecutorNode(tmp_child, child);
        }
      }
    } else {
      std::cout << "ERROR: AddExecutorNode() Not SendCurrent or Neuron  "
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
  void StartInitialStimulus() {
    amplitud_input.push_back(head_executor);
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
                << " | intensity = " << neuron->intensity;
      if (!neuron->childrens.size()) {
        std::cout << " | output = " << neuron->input + neuron->intensity
                  << " | Threahold = "
                  << ((neuron->input + neuron->intensity) >= -40 ? "yes"
                                                                 : "not");
      }
      std::cout << std::endl;
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

  void PrintAST() {
    ASTNode *sendcurrent = NULL;
    if (!ExistSendCurrentNode(sendcurrent)) {
      std::cout << "ERROR: PrintAST() no SendCurrent node found" << std::endl;
      return;
    }

    // beauty tree AST
    CensNode *head_beauty_tree = new CensNode("RunSystem");
    AddChildrensBeautyTreeAST<ASTNode>(sendcurrent, head_beauty_tree);
    BeautyTree<CensNode> printer(head_beauty_tree, &CensNode::getChildren,
                                 &CensNode::getData);
    std::cout << "\n\t=========================== ABSTRACT SYNTAX TREE "
                 "===========================\n"
              << std::endl;
    printer.print();
  }

  void PrintExecutorGraph() {
    CensNode *head_beauty_tree_g = new CensNode("");
    AddChildrensBeautyTreeAST<ASTExecutorNode>(head_executor,
                                               head_beauty_tree_g);
    BeautyTree<CensNode> printer_g(head_beauty_tree_g, &CensNode::getChildren,
                                   &CensNode::getData);
    std::cout << "\n\t=========================== EXECUTABLE GRAPH "
                 "==========================="
              << std::endl;
    printer_g.print();

    std::cout << "Neuron on simulation: " << executable_neurons.size() - 1
              << std::endl;
  }

  void PrintSimulationResults() {
    std::cout << "\n--> Simulation results: (To see them graphically, put "
                 "-graph command.)"
              << std::endl;
    std::cout << head_executor->node_name
              << " | Initial Stimulus = " << head_executor->intensity
              << std::endl;
    OutputResults(head_executor);
  }

  void GenerateMatlabCode() {
    // for each output neuron, not all
    for (auto &out : neurons_output) {
      std::string file_results_path_ = "neuron_" + out.name + "_graph.m";
      matlab_paths.push_back(file_results_path_);
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

  void CreateExecutorGraph() {
    ASTNode *sendcurrent = NULL;
    if (!ExistSendCurrentNode(sendcurrent)) {
      std::cout << "ERROR: CreateExecutorGraph() no SendCurrent node found"
                << std::endl;
      return;
    }

    head_executor->node_name = "CENS-Root";
    head_executor->intensity = 10;
    head_executor->type = "Excitatory";
    executable_neurons.push_back(head_executor);

    AddExecutorNode(head_executor, sendcurrent);
  }

  void MatlabGraphics() {
    for (auto &run : matlab_files) {
      int size = run.size();
      char *to_run = new char[size];
      strcpy(to_run, run.c_str());
      system(to_run);
    }
    for (auto &run : matlab_paths) {
      std::cout << " Running Matlab Graphic to: " << run << std::endl;
    }

    
  }
};

//========================================================================

#endif  // CENS_AST_HPP_
