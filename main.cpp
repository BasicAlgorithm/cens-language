#include <iostream>
#include <string>
#include <vector>

#include "src/LL1.hpp"
#include "src/parser.hpp"
#include "src/scanner.hpp"

void HowToUse() {
  std::cout << "error: Not enough arguments.\n" << std::endl;
  std::cout << "Instructions:" << std::endl;
  std::cout << "\tcens.exe <commands> <optional>" << std::endl;
  std::cout << "Commands:" << std::endl;
  std::cout
      << "\t-default\t\t Consider in_file.txt out_file.txt. No full print.\n";
  std::cout << "\t\t\t\t Not necessary other commands or optional."
            << std::endl;
  std::cout << "\t-dev\t\t\t Default files names + full print." << std::endl;
  std::cout << "\t\t\t\t Not necessary other commands or optional."
            << std::endl;
  std::cout << "\t-i [input_file]\t\t To define input file with CENS code."
            << std::endl;
  std::cout
      << "\t-o [output_file]\t To define output file with tokens and lexemas."
      << std::endl;
  std::cout << "Optional:" << std::endl;
  std::cout << "\t-debug\t\t\t Print couts of arrays." << std::endl;
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

    // Argument for developer
    it = find(args.begin(), args.end(), "-dev");
    if (it != args.end()) {
      CENS::print_info = true;
      CENS::RunScanner("in_file.txt", "out_file.txt");
      CENS::RunParser();
      return 0;
    }

    // Argument for user run
    it = find(args.begin(), args.end(), "-default");
    if (it != args.end()) {
      CENS::print_info = false;
      CENS::RunScanner("in_file.txt", "out_file.txt");
      CENS::RunParser();
      return 0;
    }

    // Arguments for specific use
    it = find(args.begin(), args.end(), "-i");
    if (it != args.end()) {
      it++;
      std::string source_file(*it);
      std::string out_file;
      // Argument for output file
      it = find(args.begin(), args.end(), "-o");
      if (it != args.end()) {
        it++;
        out_file = *it;
      } else {
        out_file = "out_file.txt";
      }

      // Argument for print couts
      it = find(args.begin(), args.end(), "-debug");
      if (it != args.end()) {
        CENS::print_info = true;
      } else {
        CENS::print_info = false;
      }

      // running CENS
      CENS::RunScanner(source_file, out_file);
      CENS::RunParser();
      return 0;
    }
    return 0;
  }
  return 0;
}