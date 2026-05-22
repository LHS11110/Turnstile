#include "turnstile.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

int main() {
  bool interactive = isatty(STDIN_FILENO);

  if (interactive) {
    std::cout << "=========================================\n";
    std::cout << "  Turnstile Proof Assistant (System LK)  \n";
    std::cout << "=========================================\n";
    std::cout << "Enter your theorem and proof steps line-by-line.\n";
    std::cout << "Type 'exit' to quit.\n\n";
  }

  Turnstile turnstile(nullptr);
  std::string line;
  size_t lineNumber = 0;

  while (true) {
    if (interactive) {
      std::cout << "> " << std::flush;
    }
    if (!std::getline(std::cin, line)) {
      break;
    }
    lineNumber++;

    if (interactive && line == "exit") {
      break;
    }

    // Skip completely empty lines or comment-only lines
    if (line.empty()) {
      continue;
    }
    bool only_whitespace = true;
    for (char c : line) {
      if (!std::isspace(c)) {
        only_whitespace = false;
        break;
      }
    }
    if (only_whitespace) {
      continue;
    }

    try {
      turnstile.interpret(line);
      if (interactive) {
        std::cout << "  [OK] Step accepted." << std::endl;
      }
    } catch (const std::exception &e) {
      std::cerr << "  [ERROR] Line " << lineNumber << ": " << e.what() << std::endl;
    }
  }

  return 0;
}
