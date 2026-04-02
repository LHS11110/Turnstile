#include "lexer.hpp"
#include <cctype>

Lexer::Lexer(const std::string &source)
    : source(source), pos(0), line(1), column(1) {}

char Lexer::peek(size_t offset) const {
  if (pos + offset >= source.length())
    return '\0';
  return source[pos + offset];
}

char Lexer::advance() {
  if (isAtEnd())
    return '\0';
  char c = source[pos++];
  if (c == '\n') {
    line++;
    column = 1;
  } else {
    column++;
  }
  return c;
}

bool Lexer::isAtEnd() const { return pos >= source.length(); }

void Lexer::skipWhitespace() {
  while (!isAtEnd()) {
    char c = peek();

    // 처음에 4개의 공백이 나오면 INDENT 토큰 처리를 위해 스킵 중단
    if (c == ' ' && column == 1 && peek(1) == ' ' && peek(2) == ' ' &&
        peek(3) == ' ') {
      break;
    }

    if (std::isspace(c)) {
      advance();
    } else if (c == '/' && peek(1) == '/') {
      // 한 줄 주석 처리 (// ...)
      while (!isAtEnd() && peek() != '\n') {
        advance();
      }
    } else {
      break;
    }
  }
}

bool Lexer::isDelimiter(char c) const {
  return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
         c == ',' || c == ';' || c == '.';
}

bool Lexer::isOperatorChar(char c) const {
  // 논리 연산자나 산술 연산자에 쓰일 수 있는 특수 기호들
  std::string opChars = "+-*/=<>!&|~^\\:";
  return opChars.find(c) != std::string::npos;
}

Token Lexer::scanString() {
  size_t startCol = column;
  size_t startLine = line;
  advance(); // 여는 큰따옴표 소비
  std::string value;

  while (!isAtEnd() && peek() != '"') {
    if (peek() == '\\' && pos + 1 < source.length()) { // 이스케이프 문자 처리
      advance();
      char escaped = advance();
      switch (escaped) {
      case 'n':
        value += '\n';
        break;
      case 't':
        value += '\t';
        break;
      case '\\':
        value += '\\';
        break;
      case '"':
        value += '"';
        break;
      default:
        value += '\\';
        value += escaped;
        break;
      }
    } else {
      value += advance();
    }
  }

  if (!isAtEnd()) {
    advance(); // 닫는 큰따옴표 소비
  }

  return {TokenType::STRING, value, startLine, startCol};
}

Token Lexer::scanNumber() {
  size_t startCol = column;
  size_t startLine = line;
  std::string value;

  while (!isAtEnd() && std::isdigit(peek())) {
    value += advance();
  }

  // 소수점 처리 (예: 3.14)
  if (peek() == '.' && std::isdigit(peek(1))) {
    value += advance(); // '.' 소비
    while (!isAtEnd() && std::isdigit(peek())) {
      value += advance();
    }
  }

  return {TokenType::LITERAL, value, startLine, startCol}; // 정수 및 실수
}

Token Lexer::scanIdentifierOrKeyword() {
  size_t startCol = column;
  size_t startLine = line;
  std::string value;

  // 식별자는 문자나 밑줄로 시작하여 영숫자 및 밑줄로 구성됨
  while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
    value += advance();
  }

  TokenType type = TokenType::IDENTIFIER;

  // 예약어 확인
  if (value == "theorem")
    type = TokenType::THEOREM;
  else if (value == "axiom")
    type = TokenType::AXIOM;
  else if (value == "qed")
    type = TokenType::QED;
  else if (value == "sorry")
    type = TokenType::SORRY;
  else if (value == "forall")
    type = TokenType::FORALL;
  else if (value == "exists")
    type = TokenType::EXIST;
  else if (value == "in")
    type = TokenType::IN;
  // Proof Rules
  else if (value == "id") type = TokenType::ID;
  else if (value == "cut") type = TokenType::CUT;
  else if (value == "andL1") type = TokenType::ANDL1;
  else if (value == "andL2") type = TokenType::ANDL2;
  else if (value == "orL") type = TokenType::ORL;
  else if (value == "implL") type = TokenType::IMPLL;
  else if (value == "notL") type = TokenType::NOTL;
  else if (value == "forallL") type = TokenType::FORALLL;
  else if (value == "existL") type = TokenType::EXISTL;
  else if (value == "wl") type = TokenType::WL;
  else if (value == "cl") type = TokenType::CL;
  else if (value == "pl") type = TokenType::PL;
  else if (value == "orR1") type = TokenType::ORR1;
  else if (value == "orR2") type = TokenType::ORR2;
  else if (value == "andR") type = TokenType::ANDR;
  else if (value == "implR") type = TokenType::IMPLR;
  else if (value == "notR") type = TokenType::NOTR;
  else if (value == "forallR") type = TokenType::FORALLR;
  else if (value == "existR") type = TokenType::EXISTR;
  else if (value == "wr") type = TokenType::WR;
  else if (value == "cr") type = TokenType::CR;
  else if (value == "pr") type = TokenType::PR;

  return {type, value, startLine, startCol};
}

Token Lexer::scanOperatorOrDelimiter() {
  size_t startCol = column;
  size_t startLine = line;
  char c = advance();
  std::string value(1, c);

  if (isDelimiter(c)) {
    if (c == ',') return {TokenType::COMMA, value, startLine, startCol};
    return {TokenType::DELIMITER, value, startLine, startCol};
  }

  // '\'로 시작하는 양화사 처리 (예: \forall, \exists)
  if (c == '\\' && std::isalpha(peek())) {
    while (!isAtEnd() && std::isalpha(peek())) {
      value += advance();
    }
    if (value == "\\forall") return {TokenType::FORALL, value, startLine, startCol};
    if (value == "\\exists") return {TokenType::EXIST, value, startLine, startCol};
    if (value == "\\in") return {TokenType::IN, value, startLine, startCol};
    // 그 외의 경우 일반 텍스트로 간주
  }

  // 다른 연산자 기호들을 묶음 (예: ->, <=, /\, \/)
  while (!isAtEnd() && isOperatorChar(peek()) && !isDelimiter(peek())) {
    value += advance();
  }

  if (value == "\\/") return {TokenType::OR, value, startLine, startCol};
  if (value == "/\\") return {TokenType::AND, value, startLine, startCol};
  if (value == "->") return {TokenType::RIGHTARROW, value, startLine, startCol};
  if (value == "~") return {TokenType::NOT, value, startLine, startCol};
  if (value == "=") return {TokenType::EQUAL, value, startLine, startCol};
  if (value == ":=") return {TokenType::COLON_EQ, value, startLine, startCol};
  if (value == "<=>") return {TokenType::EQUIV, value, startLine, startCol};
  if (value == "|-") return {TokenType::TURNSTILE, value, startLine, startCol};

  return {TokenType::UNKNOWN, value, startLine, startCol};
}

Token Lexer::nextToken() {
  skipWhitespace();

  if (isAtEnd()) {
    return {TokenType::END_OF_FILE, "", line, column};
  }

  char c = peek();

  // 줄 번호와 컬럼 번호 1인 상태에서 첫 4개의 공백을 만났을 때 INDENT 토큰 반환
  if (c == ' ' && column == 1 && peek(1) == ' ' && peek(2) == ' ' &&
      peek(3) == ' ') {
    size_t startLine = line;
    size_t startCol = column;
    advance();
    advance();
    advance();
    advance();
    return {TokenType::INDENT, "    ", startLine, startCol};
  }

  if (c == '"') {
    return scanString();
  }

  if (std::isdigit(c) || (c == '.' && std::isdigit(peek(1)))) {
    return scanNumber();
  }

  if (std::isalpha(c) || c == '_') {
    return scanIdentifierOrKeyword();
  }

  if (isDelimiter(c) || isOperatorChar(c)) {
    return scanOperatorOrDelimiter();
  }

  // 알 수 없는 문자 처리
  size_t startCol = column;
  size_t startLine = line;
  std::string val(1, advance());
  return {TokenType::UNKNOWN, val, startLine, startCol};
}

std::vector<Token> Lexer::tokenize() {
  std::vector<Token> tokens;
  Token t;
  do {
    t = nextToken();
    tokens.push_back(t);
  } while (t.type != TokenType::END_OF_FILE);

  return tokens;
}

std::string tokenTypeToString(TokenType type) {
  switch (type) {
  case TokenType::DELIMITER:
    return "DELIMITER";
  case TokenType::STRING:
    return "STRING";
  case TokenType::EQUAL:
    return "EQUAL";
  case TokenType::IN:
    return "IN";
  case TokenType::COLON_EQ:
    return "COLON_EQ";
  case TokenType::EQUIV:
    return "EQUIV";
  case TokenType::OR:
    return "OR";
  case TokenType::AND:
    return "AND";
  case TokenType::RIGHTARROW:
    return "RIGHTARROW";
  case TokenType::NOT:
    return "NOT";
  case TokenType::FORALL:
    return "FORALL";
  case TokenType::EXIST:
    return "EXIST";
  case TokenType::LITERAL:
    return "LITERAL";
  case TokenType::IDENTIFIER:
    return "IDENTIFIER";
  case TokenType::THEOREM:
    return "THEOREM";
  case TokenType::AXIOM:
    return "AXIOM";
  case TokenType::QED:
    return "QED";
  case TokenType::SORRY:
    return "SORRY";
  case TokenType::INDENT:
    return "INDENT";
  case TokenType::END_OF_FILE:
    return "EOF";
  case TokenType::UNKNOWN:
    return "UNKNOWN";
  case TokenType::TURNSTILE:
    return "TURNSTILE";
  case TokenType::COMMA:
    return "COMMA";
  case TokenType::ID: return "ID";
  case TokenType::CUT: return "CUT";
  case TokenType::ANDL1: return "ANDL1";
  case TokenType::ANDL2: return "ANDL2";
  case TokenType::ORL: return "ORL";
  case TokenType::IMPLL: return "IMPLL";
  case TokenType::NOTL: return "NOTL";
  case TokenType::FORALLL: return "FORALLL";
  case TokenType::EXISTL: return "EXISTL";
  case TokenType::WL: return "WL";
  case TokenType::CL: return "CL";
  case TokenType::PL: return "PL";
  case TokenType::ORR1: return "ORR1";
  case TokenType::ORR2: return "ORR2";
  case TokenType::ANDR: return "ANDR";
  case TokenType::IMPLR: return "IMPLR";
  case TokenType::NOTR: return "NOTR";
  case TokenType::FORALLR: return "FORALLR";
  case TokenType::EXISTR: return "EXISTR";
  case TokenType::WR: return "WR";
  case TokenType::CR: return "CR";
  case TokenType::PR: return "PR";
  default:
    return "???";
  }
}