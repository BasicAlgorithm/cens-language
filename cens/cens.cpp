#include <iostream>
#include <string>
#include <vector>

#include "LL1Settings.hpp"
#include "parser.hpp"
#include "scanner.hpp"

void HowToUse() {
  std::cout << "error: Not enough arguments.\n" << std::endl;
  std::cout << "Instructions:" << std::endl;
  std::cout << "\tcens.exe <commands> <optional>" << std::endl;
  std::cout << "Commands:" << std::endl;
  std::cout << "\t-i [input_file]\t\t To define input file with CENS code."
            << std::endl;
  std::cout
      << "\t-o [output_file]\t To define output file with tokens and lexemas."
      << std::endl;
  std::cout << "\t-graph \t To show matlab results." << std::endl;
  std::cout << "Optional:" << std::endl;
  std::cout << "\t-debug\t\t\t Print differents couts." << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    HowToUse();
  } else {
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
      args.push_back(argv[i]);
    }
    std::vector<std::string>::iterator it;
    std::string source_file;
    std::string out_file;

    // Arguments for specific use
    it = find(args.begin(), args.end(), "-i");
    if (it != args.end()) {
      it++;
      source_file = *it;
    } else {
      std::cout << "ERROR: You need specify source file" << std::endl;
      HowToUse();
      return 0;
    }

    // Argument for output file
    it = find(args.begin(), args.end(), "-o");
    if (it != args.end()) {
      it++;
      out_file = *it;
    } else {
      std::cout << "ERROR: You need specify output scanner file" << std::endl;
      HowToUse();
      return 0;
    }

    // Argument for developer
    it = find(args.begin(), args.end(), "-graph");
    if (it != args.end()) {
      CENS::print_matlab_graph = true;
    }

    // Argument for print couts
    it = find(args.begin(), args.end(), "-debug");
    if (it != args.end()) {
      CENS::print_info = true;
    }

    // running CENS
    CENS::RunScanner(source_file, out_file);
    CENS::RunParser();

    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}