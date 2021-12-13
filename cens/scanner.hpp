#ifndef CENS_SCANNER_HPP
#define CENS_SCANNER_HPP

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

#include "LL1Settings.hpp"

namespace CENS {

int lines_of_code = 1;
int lines_out = 1;
bool was_added = false;

std::vector<std::vector<std::string>> output_scanner;
std::vector<std::vector<std::string>> clean_code;

bool IsIdentifier(const std::string &str) {
  if (std::isdigit(str[0])) return false;

  int counter = 0;
  int size = str.size();
  for (; counter < size; counter++)
    if (!(isalnum(str[counter]))) return false;

  return true;
}

bool IsComment(const std::string &str) { return str == "/*" || str == "//"; }

bool IsDigit(const std::string &str) {
  return std::all_of(str.begin(), str.end(), ::isdigit);
}

bool IsLiteral(const std::string &str) {
  return str[0] == '"' && str[str.size() - 1] == '"';
}

bool isLiteral(const std::string &str) {
  return IsDigit(str) || IsLiteral(str);
}

bool isKeyword(const std::string &str) {
  for (const auto &keyword : CENS_keywords)
    if (keyword == str) return true;

  return false;
}

bool IsOperator(const std::string &str) {
  for (const auto &op : CENS_operators)
    if (op == str) return true;

  return false;
}

bool IsDelimiter(const std::string &str) {
  for (const auto &separate : CENS_delimiters)
    if (separate == str) return true;

  return false;
}

bool IsSpaceOrNextLine(const std::string &str, std::string out_file) {
  if (str == "\n") {
    std::ofstream outputFile(out_file, std::fstream::app);
    lines_of_code++;
    outputFile << "\n";
    outputFile.close();
    if (was_added && (static_cast<int>(output_scanner.size()) == lines_out)) {
      output_scanner[lines_out - 1].push_back("$");
      lines_out++;
      was_added = false;
    }
  }
  return str == " " || str == "\n";
}

void AvoidOutOfRange() {
  if (static_cast<int>(output_scanner.size()) != lines_out) {
    std::string line(std::to_string(lines_of_code));
    std::vector<std::string> tmp_v{line};
    output_scanner.push_back(tmp_v);
    clean_code.push_back(tmp_v);
  }
}

void AnalyzeToken(const std::string &token, std::string out_file) {
  std::ofstream outputFile(out_file, std::fstream::app);

  if (IsOperator(token)) {
    AvoidOutOfRange();
    outputFile << "<operator|" << token << "> ";
    output_scanner[lines_out - 1].push_back(token);
    clean_code[lines_out - 1].push_back(token);
    was_added = true;
  } else if (IsDelimiter(token)) {
    AvoidOutOfRange();
    outputFile << "<separator|" << token << "> ";
    output_scanner[lines_out - 1].push_back(token);
    clean_code[lines_out - 1].push_back(token);
    was_added = true;
  } else if (isKeyword(token)) {
    AvoidOutOfRange();
    outputFile << "<keyword|" << token << "> ";
    output_scanner[lines_out - 1].push_back(token);
    clean_code[lines_out - 1].push_back(token);
    was_added = true;
  } else if (IsLiteral(token)) {
    AvoidOutOfRange();
    outputFile << "<literal|" << token << "> ";
    output_scanner[lines_out - 1].push_back("literal");
    clean_code[lines_out - 1].push_back(token);
    was_added = true;
  } else if (IsDigit(token)) {
    AvoidOutOfRange();
    outputFile << "<literal|" << token << "> ";
    output_scanner[lines_out - 1].push_back("num");
    clean_code[lines_out - 1].push_back(token);
    was_added = true;
  } else if (IsIdentifier(token)) {
    AvoidOutOfRange();
    outputFile << "<identifier|" << token << "> ";
    output_scanner[lines_out - 1].push_back("id");
    clean_code[lines_out - 1].push_back(token);
    was_added = true;
  } else if (IsComment(token))
    outputFile << "<comment|" << token << "> ";
  else
    std::cout << "invalid token " << token << std::endl;
  // throw std::runtime_error("Invalid token: " + token);
  outputFile.close();
}

void lexicalAnalyze(const std::string &in_file, std::string out_file) {
  char ch;
  bool multi_comment = false, single_comment = false;
  std::string buffer;
  std::fstream file(in_file, std::fstream::in);

  if (!file.is_open()) {
    std::cout << "no open file" << std::endl;
  }

  while (file >> std::noskipws >> ch) {
    if (single_comment || multi_comment) {
      if (single_comment && ch == '\n') {
        single_comment = false;
        std::ofstream outputFile(out_file, std::fstream::app);
        lines_of_code++;
        outputFile << "\n";
        outputFile.close();
      }

      if (multi_comment) {
        if (ch == '*') {
          file >> ch;
          if (ch == EOF) {
            output_scanner[lines_out - 1].push_back("$");
            break;
          }

          if (ch == '/') multi_comment = false;
        } else if (ch == '\n') {
          single_comment = false;
          std::ofstream outputFile(out_file, std::fstream::app);
          lines_of_code++;
          outputFile << "\n";
          outputFile.close();
        }
      }
      continue;
    }

    if (ch == '/') {
      std::string comment(1, ch);
      file >> ch;
      if (ch == EOF) {
        AnalyzeToken(comment, out_file);
        output_scanner[lines_out - 1].push_back("$");
        break;
      }

      if (ch == '*') {
        multi_comment = true;
        comment += ch;
      } else if (ch == '/') {
        single_comment = true;
        comment += ch;
      }
      if (multi_comment || single_comment) {
        AnalyzeToken(comment, out_file);
        continue;
      }
    }

    if (IsSpaceOrNextLine(std::string(1, ch), out_file)) {
      if (!buffer.empty()) {
        AnalyzeToken(buffer, out_file);
        buffer = "";
      }
      continue;
    }

    if (IsOperator(std::string(1, ch)) && !IsOperator(buffer)) {
      if (!buffer.empty()) {
        AnalyzeToken(buffer, out_file);
        buffer = "";
      }
    }

    if (!IsOperator(std::string(1, ch)) && IsOperator(buffer)) {
      AnalyzeToken(buffer, out_file);
      buffer = "";
    }

    if (IsDelimiter(std::string(1, ch))) {
      if (!buffer.empty()) {
        AnalyzeToken(buffer, out_file);
        buffer = "";
      }
      if (IsDelimiter(std::string(1, ch))) {
        AnalyzeToken(std::string(1, ch), out_file);
        continue;
      }
    }

    buffer += ch;
  }
  output_scanner[lines_out - 1].push_back("$");
  file.close();

  if (print_info) {
    std::cout << "Scanner finished successfully" << std::endl;
    std::cout << "**************** SCANNER COUTS ****************" << std::endl;
    std::cout << "\tLines scanned: " << lines_of_code << std::endl;
    std::cout << "\tlines out: " << lines_out << std::endl;
    std::cout << "\tblank lines: " << lines_of_code - lines_out << std::endl;
    std::cout << "\toutput_scanner info: \n" << std::endl;
    std::cout << "size: " << output_scanner.size() << std::endl;
    for (int i = 0; i < static_cast<int>(output_scanner.size()); i++) {
      for (int j = 0; j < static_cast<int>(output_scanner[i].size()); j++) {
        std::cout << output_scanner[i][j] << " ";
      }
      std::cout << std::endl;
    }
    std::cout << "\tclean_code info: \n" << std::endl;
    std::cout << "size: " << clean_code.size() << std::endl;
    for (int i = 0; i < static_cast<int>(clean_code.size()); i++) {
      for (int j = 0; j < static_cast<int>(clean_code[i].size()); j++) {
        std::cout << clean_code[i][j] << " ";
      }
      std::cout << std::endl;
    }
    std::cout << "**********************************************\n"
              << std::endl;
  }
}

void RunScanner(std::string in_file, std::string out_file) {
  lexicalAnalyze(in_file, out_file);
}

}  // namespace CENS

#endif  // CENS_SCANNER_HPP