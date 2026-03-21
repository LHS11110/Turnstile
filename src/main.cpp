#include "lexer.hpp"
#include <iostream>
#include <string>

int main() {
  std::cout << "Turnstile: System LK Proof Assistant Lexer Test\n\n";

  std::string source = "theorem P -> P;\n    axiom A;\nqed";
  Lexer lexer(source);
  std::vector<Token> tokens = lexer.tokenize();

  std::cout << "Tokens:\n";
  for (const auto &token : tokens) {
    std::cout << "  " << tokenTypeToString(token.type) << "\t" << token.value
              << "\t(" << token.line << ", " << token.column << ")\n";
  }

  return 0;
}
