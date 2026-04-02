#include "sequent.hpp"
#include "parser.hpp"
#include "lexer.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <sstream>

void runPerfTest() {
    std::cout << "\n=== Parsing Performance Test ===\n";
    // Generate a very long proposition: P0 \/ P1 \/ P2 ...
    std::stringstream ss;
    int N = 10000;
    for(int i = 0; i < 100; i++) {
        ss << "theorem Massive_Theorem_" << i << " := |- ";
        for(int j = 0; j < N; j++) {
            ss << "P" << j;
            if(j < N - 1) ss << " \\/ ";
        }
        ss << "\n    prove\nqed\n";
    }
    
    std::string code = ss.str();
    std::cout << "Generated test code of length: " << code.length() << " characters.\n";

    // 1. Lexer timing
    auto start_lex = std::chrono::high_resolution_clock::now();
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    auto end_lex = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> lex_time = end_lex - start_lex;
    std::cout << "Lexer Time (" << tokens.size() << " tokens): " << lex_time.count() << " ms\n";

    // 2. Parser timing
    auto start_parse = std::chrono::high_resolution_clock::now();
    Parser parser(tokens);
    try {
        auto ast = parser.parse();
        auto end_parse = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> parse_time = end_parse - start_parse;
        std::cout << "Parser Time: " << parse_time.count() << " ms\n";
    } catch(const std::exception& e) {
        std::cout << "Parser error during test: " << e.what() << "\n";
    }
}

int main() {
  runPerfTest();

  /* 
  // User's WIP Proof Rules
  Prop a = std::make_shared<Var>("A"); // 명제 상수 도입
  auto id = Identity(a);
  Sequent idSeq = id.apply({});
  std::cout << idSeq.toString() << '\n';

  auto notr = NotR();
  Sequent notrSeq = notr.apply({idSeq});
  std::cout << notrSeq.toString() << '\n';

  auto orr2 = OrR_2(a);
  Sequent orr2Seq = orr2.apply({notrSeq});
  std::cout << orr2Seq.toString() << '\n';

  auto permr = PermutationR(0);
  Sequent permrSeq = permr.apply({orr2Seq});
  std::cout << permrSeq.toString() << '\n';

  auto orr1 = OrR_1(std::make_shared<Not>(a));
  Sequent orr1Seq = orr1.apply({permrSeq});
  std::cout << orr1Seq.toString() << '\n';

  auto contrr = ContractionR();
  Sequent contrrSeq = contrr.apply({orr1Seq});
  std::cout << contrrSeq.toString() << '\n';
  */

  return 0;
}
