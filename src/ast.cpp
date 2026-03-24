#include "ast.hpp"

Expr::Expr(Token token) : token(std::move(token)) {}

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

std::string BinaryExpr::toString() const {
  return "(" + left->toString() + " " + op + " " + right->toString() + ")";
}

BinaryExpr::BinaryExpr(Token token, std::string op, std::shared_ptr<Expr> left,
                       std::shared_ptr<Expr> right)
    : Expr(std::move(token)), op(std::move(op)), left(std::move(left)),
      right(std::move(right)) {}

VarExpr::VarExpr(Token token, std::string name)
    : Expr(std::move(token)), name(std::move(name)) {}

UnaryExpr::UnaryExpr(Token token, std::string op, std::shared_ptr<Expr> expr)
    : Expr(std::move(token)), op(std::move(op)), right(std::move(expr)) {}

std::string UnaryExpr::toString() const {
  return "(" + op + right->toString() + ")";
}

std::string VarExpr::toString() const { return name; }
