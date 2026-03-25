#include "ast.hpp"

std::string TheoremNode::toString() const {
  std::string res = "Theorem " + name + " := " + proposition->toString() + "\n";
  res += "  Proof tokens:\n";

  size_t currentLine = 0;
  for (const auto &t : proofTokens) {
    if (t.line != currentLine) {
      if (currentLine != 0)
        res += "\n";
      res += "    [Line " + std::to_string(t.line) + "] ";
      currentLine = t.line;
    }
    if (t.type == TokenType::END_OF_FILE)
      res += "[EOF]";
    else
      res += t.value + " ";
  }
  res += "\n";
  return res;
}
