#pragma once

#include "lexer.hpp"
#include "sequent.hpp"
#include <memory>
#include <stdexcept>
#include <vector>

class ParserError : public std::runtime_error {
public:
  ParserError(const std::string &msg, const Token &token);
};

class Parser {
public:
  explicit Parser(const std::vector<Token> &tokens);
  Prop parse();

private:
  const std::vector<Token> &tokens;
  size_t pos;

  const Token &peek() const;
  const Token &previous() const;
  bool isAtEnd() const;
  Token advance();
  bool check(TokenType type) const;
  bool match(TokenType type);
  Token consume(TokenType type, const std::string &message);

  std::shared_ptr<Sequent> parseSequent();
  Prop parseProposition();
  Prop parseEquivalence();
  Prop parseImplication();
  Prop parseOr();
  Prop parseAnd();
  Prop parseUnary();
  Prop parsePrimary();
};
