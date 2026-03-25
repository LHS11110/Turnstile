#include "parser.hpp"

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), pos(0) {}

const Token &Parser::peek() const { return tokens[pos]; }

const Token &Parser::previous() const { return tokens[pos - 1]; }

bool Parser::isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }

Token Parser::advance() {
  if (!isAtEnd())
    pos++;
  return previous();
}

bool Parser::check(TokenType type) const {
  if (isAtEnd())
    return false;
  return peek().type == type;
}

bool Parser::match(TokenType type) {
  if (check(type)) {
    advance();
    return true;
  }
  return false;
}

Token Parser::consume(TokenType type, const std::string &message) {
  if (check(type))
    return advance();
  throw ParserError(message, peek());
}

std::vector<std::shared_ptr<TheoremNode>> Parser::parse() {
  std::vector<std::shared_ptr<TheoremNode>> theorems;
  while (!isAtEnd()) {
    // Skip stray indents or newlines if any
    if (match(TokenType::INDENT))
      continue;

    if (match(TokenType::THEOREM)) {
      theorems.push_back(parseTheorem());
    } else {
      throw ParserError("Expected 'theorem' keyword", peek());
    }
  }
  return theorems;
}

std::shared_ptr<TheoremNode> Parser::parseTheorem() {
  auto theorem = std::make_shared<TheoremNode>();

  Token nameToken;
  if (check(TokenType::IDENTIFIER) || check(TokenType::STRING)) {
    nameToken = advance();
  } else {
    throw ParserError("Expected theorem name (identifier or string)", peek());
  }
  theorem->name = nameToken.value;

  if (!match(TokenType::COLON_EQ)) {
    throw ParserError("Expected ':=' after theorem name", peek());
  }

  theorem->proposition = parseSequent();

  size_t currentLine = previous().line;
  bool hasStartedProof = false;

  // Collect proof tokens until \nqed
  while (!isAtEnd() && !check(TokenType::QED)) {
    Token t = peek();

    // 새 줄이 시작될 때마다 반드시 INDENT가 있어야 함
    if (t.line > currentLine) {
      if (t.type != TokenType::INDENT) {
        throw ParserError("Proof lines must be indented with 4 spaces", t);
      }
    } else if (!hasStartedProof && t.type != TokenType::INDENT) {
      // 첫 증명 토큰이 새로운 줄(INDENT)에서 시작하지 않은 경우
      throw ParserError("Proof must start on a new indented line", t);
    }

    currentLine = t.line;
    t = advance();
    hasStartedProof = true;

    // INDENT 토큰은 증명 토큰 리스트에서 생략함
    if (t.type == TokenType::INDENT) {
      continue;
    }

    theorem->proofTokens.push_back(t);
  }

  if (check(TokenType::QED)) {
    if (peek().column != 1) {
      throw ParserError(
          "'qed' must be the only token on its line without any leading spaces",
          peek());
    }
  }

  Token qedToken =
      consume(TokenType::QED, "Expected 'qed' to close theorem block");

  // qed 이후 같은 줄에 다른 토큰이 오면 에러 처리
  if (!isAtEnd() && peek().line == qedToken.line) {
    throw ParserError("'qed' must be the only token on its line", peek());
  }

  return theorem;
}

std::shared_ptr<Sequent> Parser::parseSequent() {
  std::vector<Prop> antecedents;
  std::vector<Prop> succedents;

  if (!check(TokenType::TURNSTILE)) {
    antecedents.push_back(parseProposition());
    while (match(TokenType::COMMA)) {
      antecedents.push_back(parseProposition());
    }
  }

  consume(TokenType::TURNSTILE, "Expected '|-' in Sequent");

  while (!check(TokenType::INDENT) && !check(TokenType::QED) && !isAtEnd()) {
    succedents.push_back(parseProposition());
    if (!match(TokenType::COMMA)) {
      break;
    }
  }

  return std::make_shared<Sequent>(antecedents, succedents);
}

std::shared_ptr<PropTree> Parser::parseProposition() {
  return parseEquivalence(); // lowest priority
}

std::shared_ptr<PropTree> Parser::parseEquivalence() {
  auto expr = parseImplication();
  while (match(TokenType::EQUIV)) {
    Token op = previous();
    auto right = parseImplication();
    expr = std::make_shared<Equiv>(expr, right);
  }
  return expr;
}

std::shared_ptr<PropTree> Parser::parseImplication() {
  auto expr = parseOr();
  while (match(TokenType::RIGHTARROW)) {
    Token op = previous();
    auto right = parseOr();
    expr = std::make_shared<Implies>(expr, right);
  }
  return expr;
}

std::shared_ptr<PropTree> Parser::parseOr() {
  auto expr = parseAnd();
  while (match(TokenType::OR)) {
    Token op = previous();
    auto right = parseAnd();
    expr = std::make_shared<Or>(expr, right);
  }
  return expr;
}

std::shared_ptr<PropTree> Parser::parseAnd() {
  auto expr = parseUnary();
  while (match(TokenType::AND)) {
    Token op = previous();
    auto right = parseUnary();
    expr = std::make_shared<And>(expr, right);
  }
  return expr;
}

std::shared_ptr<PropTree> Parser::parseUnary() {
  if (match(TokenType::NOT)) {
    Token op = previous();
    auto right = parseUnary(); // Right-associative or recursive desc
    return std::make_shared<Not>(right);
  }
  return parsePrimary();
}

std::shared_ptr<PropTree> Parser::parsePrimary() {
  if (match(TokenType::IDENTIFIER)) {
    return std::make_shared<Var>(static_cast<int>(previous().value[0]));
  }
  if (match(TokenType::DELIMITER) && previous().value == "(") {
    auto expr = parseProposition();
    if (!(match(TokenType::DELIMITER) && previous().value == ")")) {
      throw ParserError("Expected ')' after expression", peek());
    }
    return expr;
  }
  throw ParserError("Expected expression", peek());
}

ParserError::ParserError(const std::string &msg, const Token &token)
    : std::runtime_error(msg + " at line " + std::to_string(token.line) + ":" +
                         std::to_string(token.column)) {}
