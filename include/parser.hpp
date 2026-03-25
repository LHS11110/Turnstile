#pragma once

#include "ast.hpp"
#include "lexer.hpp"
#include <memory>
#include <stdexcept>
#include <vector>

class ParserError : public std::runtime_error {
public:
  ParserError(const std::string &msg, const Token &token);
};

class Parser {
public:
  explicit Parser(std::vector<Token> tokens);
  std::vector<std::shared_ptr<TheoremNode>> parse();

private:
  std::vector<Token> tokens;
  size_t pos;

  const Token &peek() const;
  const Token &previous() const;
  bool isAtEnd() const;
  Token advance();
  bool check(TokenType type) const;
  bool match(TokenType type);
  Token consume(TokenType type, const std::string &message);

  std::shared_ptr<TheoremNode> parseTheorem();
  std::shared_ptr<Sequent> parseSequent();
  std::shared_ptr<PropTree> parseProposition();
  std::shared_ptr<PropTree> parseEquivalence();
  std::shared_ptr<PropTree> parseImplication();
  std::shared_ptr<PropTree> parseOr();
  std::shared_ptr<PropTree> parseAnd();
  std::shared_ptr<PropTree> parseUnary();
  std::shared_ptr<PropTree> parsePrimary();
};
