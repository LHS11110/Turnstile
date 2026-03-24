#pragma once

#include "lexer.hpp"
#include <memory>
#include <string>
#include <vector>

// Base Expr class
struct Expr {
  Token token; // Representative token
  Expr(Token token);
  virtual ~Expr() = default;
  virtual std::string toString() const = 0;
};

// Variable (e.g., P, Q)
struct VarExpr : public Expr {
  std::string name;
  VarExpr(Token token, std::string name);
  std::string toString() const override;
};

// Unary Expression (e.g., ~P)
struct UnaryExpr : public Expr {
  std::string op;
  std::shared_ptr<Expr> right;
  UnaryExpr(Token token, std::string op, std::shared_ptr<Expr> right);
  std::string toString() const override;
};

// Binary Expression (e.g., P \/ Q)
struct BinaryExpr : public Expr {
  std::string op;
  std::shared_ptr<Expr> left;
  std::shared_ptr<Expr> right;
  BinaryExpr(Token token, std::string op, std::shared_ptr<Expr> left,
             std::shared_ptr<Expr> right);
  std::string toString() const override;
};

// Theorem Block
struct TheoremNode {
  std::string name;
  std::shared_ptr<Expr> proposition;
  std::vector<Token> proofTokens; // Raw tokens between `:=` and `qed`

  std::string toString() const;
};
