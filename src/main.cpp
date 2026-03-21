#include <iostream>
#include "lexer.hpp"

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::DELIMITER: return "DELIMITER";
        case TokenType::STRING: return "STRING";
        case TokenType::OPERATOR: return "OPERATOR";
        case TokenType::QUANTIFIER: return "QUANTIFIER";
        case TokenType::LITERAL: return "LITERAL";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::THEOREM: return "THEOREM";
        case TokenType::AXIOM: return "AXIOM";
        case TokenType::END_OF_FILE: return "EOF";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "???";
    }
}

void testLexer(const std::string& input) {
    std::cout << "Input: " << input << "\n";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    for (const auto& token : tokens) {
        std::cout << "[" << tokenTypeToString(token.type) << "] '" 
                  << token.value << "' (Line: " << token.line 
                  << ", Col: " << token.column << ")\n";
    }
    std::cout << "----------------------------------------\n";
}

int main() {
    std::cout << "Turnstile: System LK Proof Assistant Lexer Test\n\n";

    testLexer("theorem ModusPonens: A /\\ (A -> B) -> B;");
    testLexer("forall x, \\exists y. P(x) \\/ ~Q(y, 123) = \"hello\"");
    testLexer("axiom my_axiom");
    testLexer("3.14 + (2 * x)");

    return 0;
}
