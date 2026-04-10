#include "parser.hpp"
#include "sequent.hpp"

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), pos(0) {}

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

/*
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
*/
Prop Parser::parse() {
  int indentLevel = 0;
  while (check(TokenType::INDENT)) {
    advance();
    indentLevel++;
  }

  if (indentLevel == 0) {
    if (check(TokenType::QED)) {
      if (peek().column != 1) {
        throw ParserError("'qed' must be the only token on its line without "
                          "any leading spaces",
                          peek());
      }
      Token qedToken = consume(TokenType::QED, "Expected 'qed'");
      if (!isAtEnd() && peek().line == qedToken.line) {
        throw ParserError("Expected end of line after 'qed'", peek());
      }
      return nullptr;
    }

    Token startToken = peek();
    consume(TokenType::THEOREM, "Expected 'theorem' keyword");

    Token nameToken;
    if (check(TokenType::IDENTIFIER) || check(TokenType::STRING))
      nameToken = advance();
    else
      throw ParserError("Expected theorem name (identifier or string)", peek());

    consume(TokenType::COLON_EQ, "Expected ':=' after theorem name");

    auto head = std::make_shared<Head>(Var(std::string(nameToken.value)),
                                       parseSequent());

    if (!isAtEnd() && peek().line == startToken.line) {
      throw ParserError("Unexpected token after theorem definition", peek());
    }
    return head;
  } else {
    if (isAtEnd())
      throw ParserError("Expected proof rule", peek());

    Token ruleToken = advance();
    TokenType type = ruleToken.type;
    std::function<Sequent(const std::vector<Sequent> &)> evalFunc;

    switch (type) {
    case TokenType::ID: {
      Prop p = parseProposition();
      evalFunc = [p](const std::vector<Sequent> &seqs) {
        return Identity(p).apply(seqs);
      };
      break;
    }
    case TokenType::CUT: {
      evalFunc = [](const std::vector<Sequent> &seqs) {
        return Cut().apply(seqs);
      };
      break;
    }
    case TokenType::ANDL1: {
      Prop p = parseProposition();
      evalFunc = [p](const std::vector<Sequent> &seqs) {
        return AndL_1(p).apply(seqs);
      };
      break;
    }
    case TokenType::ANDL2: {
      Prop p = parseProposition();
      evalFunc = [p](const std::vector<Sequent> &seqs) {
        return AndL_2(p).apply(seqs);
      };
      break;
    }
    case TokenType::ORL: {
      evalFunc = [](const std::vector<Sequent> &seqs) {
        return OrL().apply(seqs);
      };
      break;
    }
    case TokenType::IMPLL: {
      evalFunc = [](const std::vector<Sequent> &seqs) {
        return ImpliesL().apply(seqs);
      };
      break;
    }
    case TokenType::NOTL: {
      evalFunc = [](const std::vector<Sequent> &seqs) {
        return NotL().apply(seqs);
      };
      break;
    }
    case TokenType::FORALLL: {
      Token t1 = consume(TokenType::IDENTIFIER,
                         "Expected first identifier for forallL");
      Token t2 = consume(TokenType::IDENTIFIER,
                         "Expected second identifier for forallL");
      Var v1(std::string(t1.value));
      Var v2(std::string(t2.value));
      evalFunc = [v1, v2](const std::vector<Sequent> &seqs) {
        return ForallL(v1, v2).apply(seqs);
      };
      break;
    }
    case TokenType::EXISTL: {
      Token t1 = consume(TokenType::IDENTIFIER,
                         "Expected first identifier for existL");
      Token t2 = consume(TokenType::IDENTIFIER,
                         "Expected second identifier for existL");
      Var v1(std::string(t1.value));
      Var v2(std::string(t2.value));
      evalFunc = [v1, v2](const std::vector<Sequent> &seqs) {
        return ExistL(v1, v2).apply(seqs);
      };
      break;
    }
    case TokenType::WL: {
      Prop p = parseProposition();
      evalFunc = [p](const std::vector<Sequent> &seqs) {
        return WeakeningL(p).apply(seqs);
      };
      break;
    }
    case TokenType::CL: {
      evalFunc = [](const std::vector<Sequent> &seqs) {
        return ContractionL().apply(seqs);
      };
      break;
    }
    case TokenType::PL: {
      Token t = consume(TokenType::LITERAL, "Expected integer for pl");
      int i = std::stoi(std::string(t.value));
      evalFunc = [i](const std::vector<Sequent> &seqs) {
        return PermutationL(i).apply(seqs);
      };
      break;
    }
    case TokenType::ORR1: {
      Prop p = parseProposition();
      evalFunc = [p](const std::vector<Sequent> &seqs) {
        return OrR_1(p).apply(seqs);
      };
      break;
    }
    case TokenType::ORR2: {
      Prop p = parseProposition();
      evalFunc = [p](const std::vector<Sequent> &seqs) {
        return OrR_2(p).apply(seqs);
      };
      break;
    }
    case TokenType::ANDR: {
      evalFunc = [](const std::vector<Sequent> &seqs) {
        return AndR().apply(seqs);
      };
      break;
    }
    case TokenType::IMPLR: {
      evalFunc = [](const std::vector<Sequent> &seqs) {
        return ImpliesR().apply(seqs);
      };
      break;
    }
    case TokenType::NOTR: {
      evalFunc = [](const std::vector<Sequent> &seqs) {
        return NotR().apply(seqs);
      };
      break;
    }
    case TokenType::FORALLR: {
      Token t1 = consume(TokenType::IDENTIFIER,
                         "Expected first identifier for forallR");
      Token t2 = consume(TokenType::IDENTIFIER,
                         "Expected second identifier for forallR");
      Var v1(std::string(t1.value));
      Var v2(std::string(t2.value));
      evalFunc = [v1, v2](const std::vector<Sequent> &seqs) {
        return ForallR(v1, v2).apply(seqs);
      };
      break;
    }
    case TokenType::EXISTR: {
      Token t1 = consume(TokenType::IDENTIFIER,
                         "Expected first identifier for existR");
      Token t2 = consume(TokenType::IDENTIFIER,
                         "Expected second identifier for existR");
      Var v1(std::string(t1.value));
      Var v2(std::string(t2.value));
      evalFunc = [v1, v2](const std::vector<Sequent> &seqs) {
        return ExistR(v1, v2).apply(seqs);
      };
      break;
    }
    case TokenType::WR: {
      Prop p = parseProposition();
      evalFunc = [p](const std::vector<Sequent> &seqs) {
        return WeakeningR(p).apply(seqs);
      };
      break;
    }
    case TokenType::CR: {
      evalFunc = [](const std::vector<Sequent> &seqs) {
        return ContractionR().apply(seqs);
      };
      break;
    }
    case TokenType::PR: {
      Token t = consume(TokenType::LITERAL, "Expected integer for pr");
      int i = std::stoi(std::string(t.value));
      evalFunc = [i](const std::vector<Sequent> &seqs) {
        return PermutationR(i).apply(seqs);
      };
      break;
    }
    case TokenType::NEW_BRANCH: {
      evalFunc = [](const std::vector<Sequent> &) { return Sequent({}, {}); };
      break;
    }
    case TokenType::END_BRANCH: {
      evalFunc = [](const std::vector<Sequent> &) { return Sequent({}, {}); };
      break;
    }
    default:
      throw ParserError("Unexpected token, expected proof rule", ruleToken);
    }

    if (!isAtEnd() && peek().line == ruleToken.line) {
      throw ParserError("Unexpected token after proof rule", peek());
    }

    return std::make_shared<Eval>(evalFunc, type, indentLevel);
  }
}

std::shared_ptr<Sequent> Parser::parseSequent() {
  std::vector<Prop> antecedents;
  std::vector<Prop> succedents;

  size_t declLine = peek().line;

  if (!check(TokenType::TURNSTILE)) {
    antecedents.push_back(parseProposition());
    while (match(TokenType::COMMA) && peek().line == declLine) {
      antecedents.push_back(parseProposition());
    }
  }

  Token turnstile = consume(TokenType::TURNSTILE, "Expected '|-' in Sequent");
  size_t seqLine = turnstile.line;

  while (!check(TokenType::INDENT) && !check(TokenType::QED) && !isAtEnd() &&
         peek().line == seqLine) {
    succedents.push_back(parseProposition());
    if (!match(TokenType::COMMA)) {
      break;
    }
  }

  return std::make_shared<Sequent>(antecedents, succedents);
}

Prop Parser::parseProposition() {
  return parseEquivalence(); // lowest priority
}

Prop Parser::parseEquivalence() {
  auto expr = parseImplication();
  while (match(TokenType::EQUIV)) {
    previous();
    auto right = parseImplication();
    expr = std::make_shared<Equiv>(expr, right);
  }
  return expr;
}

Prop Parser::parseImplication() {
  auto expr = parseOr();
  if (match(TokenType::RIGHTARROW)) {
    auto right = parseImplication();
    expr = std::make_shared<Implies>(expr, right);
  }
  return expr;
}

Prop Parser::parseOr() {
  auto expr = parseAnd();
  while (match(TokenType::OR)) {
    previous();
    auto right = parseAnd();
    expr = std::make_shared<Or>(expr, right);
  }
  return expr;
}

Prop Parser::parseAnd() {
  auto expr = parseUnary();
  while (match(TokenType::AND)) {
    previous();
    auto right = parseUnary();
    expr = std::make_shared<And>(expr, right);
  }
  return expr;
}

Prop Parser::parseUnary() {
  if (match(TokenType::NOT)) {
    previous();
    auto right = parseUnary(); // Right-associative or recursive desc
    return std::make_shared<Not>(right);
  }
  if (match(TokenType::FORALL)) {
    if (!match(TokenType::IDENTIFIER))
      throw ParserError("Expected variable after forall", peek());
    auto varNode = std::make_shared<Var>(std::string(previous().value));
    match(TokenType::COMMA);
    auto prop = parseProposition();
    return std::make_shared<Forall>(varNode, prop);
  }
  if (match(TokenType::EXIST)) {
    if (!match(TokenType::IDENTIFIER))
      throw ParserError("Expected variable after exists", peek());
    auto varNode = std::make_shared<Var>(std::string(previous().value));
    match(TokenType::COMMA);
    auto prop = parseProposition();
    return std::make_shared<Exist>(varNode, prop);
  }
  return parsePrimary();
}

Prop Parser::parsePrimary() {
  if (match(TokenType::IDENTIFIER)) {
    auto varNode = std::make_shared<Var>(std::string(previous().value));

    if (match(TokenType::EQUAL)) {
      if (match(TokenType::IDENTIFIER)) {
        return std::make_shared<Equal>(*varNode,
                                       Var(std::string(previous().value)));
      } else {
        throw ParserError("Expected identifier after '='", peek());
      }
    } else if (match(TokenType::IN)) {
      if (match(TokenType::IDENTIFIER)) {
        return std::make_shared<In>(*varNode,
                                    Var(std::string(previous().value)));
      } else {
        throw ParserError("Expected identifier after 'in'", peek());
      }
    }
    return varNode;
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
