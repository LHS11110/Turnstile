#pragma once

#include <string>
#include <vector>

enum class TokenType {
  DELIMITER,   // 구분자 (e.g., '(', ')', '{', '}', '[', ']', ',', ';')
  STRING,      // 문자열 (e.g., "hello")
  EQUAL,       // '='
  IN,          // '\in', 'in'
  COLON_EQ,    // ':='
  EQUIV,       // '<=>'
  OR,          // '\/'
  AND,         // '/\'
  RIGHTARROW,  // '->'
  NOT,         // '~'
  FORALL,      // '\forall', 'forall'
  EXIST,       // '\exists', 'exists'
  LITERAL,     // 리터럴 (정수, 실수 등)
  IDENTIFIER,  // 식별자 (변수명, 함수명 등)
  THEOREM,     // "theorem" 키워드
  AXIOM,       // "axiom" 키워드
  QED,         // "qed" 키워드
  SORRY,       // "sorry" 키워드
  INDENT,      // 처음 4번의 공백
  END_OF_FILE, // EOF
  UNKNOWN,     // 알 수 없는 토큰
  TURNSTILE,   // |-
  COMMA,       // ,
  // Proof Rules
  ID, CUT,
  ANDL1, ANDL2, ORL, IMPLL, NOTL, FORALLL, EXISTL, WL, CL, PL,
  ORR1, ORR2, ANDR, IMPLR, NOTR, FORALLR, EXISTR, WR, CR, PR
};

struct Token {
  TokenType type;
  std::string value;
  size_t line;
  size_t column;
};

class Lexer {
public:
  Lexer(const std::string &source);
  std::vector<Token> tokenize();

private:
  std::string source;
  size_t pos;
  size_t line;
  size_t column;

  char peek(size_t offset = 0) const;
  char advance();
  bool isAtEnd() const;
  void skipWhitespace();

  Token nextToken();
  Token scanString();
  Token scanNumber();
  Token scanIdentifierOrKeyword();
  Token scanOperatorOrDelimiter();

  bool isDelimiter(char c) const;
  bool isOperatorChar(char c) const;
};

std::string tokenTypeToString(TokenType type);