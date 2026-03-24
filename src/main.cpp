#include "lexer.hpp"
#include "parser.hpp"
#include "sequent.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

void runTest(const std::string &name, const std::string &source) {
  std::cout << "=== Test: " << name << " ===\n";
  try {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    auto theorems = parser.parse();
    std::cout << "[SUCCESS] Parsed " << theorems.size() << " theorem(s).\n";
    for (const auto &thm : theorems) {
      std::cout << thm->toString();
    }
  } catch (const std::exception &e) {
    std::cout << "[ERROR] Parser failed: " << e.what() << "\n";
  }
  std::cout << "\n";
}

int main() {
  std::cout << "Turnstile: System LK Proof Assistant Parser Tests\n\n";

  auto start_time = std::chrono::high_resolution_clock::now();

  // 1. 성공 테스트: 드모르간 법칙
  std::string valid1 = "theorem De_Morgans_laws := ~(P \\/ Q) <=> ~P /\\ ~Q\n"
                       "    prove...\n"
                       "qed";
  runTest("Valid De Morgan's Laws", valid1);

  // 2. 성공 테스트: 간단한 함의
  std::string valid2 = "theorem Identity := P -> P\n"
                       "    intros P\n"
                       "    exact P\n"
                       "    \"asd\"\n"
                       "qed\n";
  runTest("Valid Identity Implication", valid2);

  // 3. 실패 테스트: ':=' 기호 누락 (또는 오타 '=')
  std::string invalid1 = "theorem Typo_Equals = P -> P\n"
                         "    prove\n"
                         "qed";
  runTest("Invalid Missing ':='", invalid1);

  // 4. 실패 테스트: 종료 키워드 'qed' 누락
  std::string invalid2 = "theorem Unfinished := P \\/ ~P\n"
                         "    prove";
  runTest("Invalid Missing 'qed'", invalid2);

  // 5. 실패 테스트: 선언부의 명제가 누락됨
  std::string invalid3 = "theorem No_Prop :=\n"
                         "    prove\n"
                         "qed";
  runTest("Invalid Missing Proposition", invalid3);

  // 6. 실패 테스트: 괄호 짝 불일치
  std::string invalid4 = "theorem Bad_Parens := (P \\/ Q\n"
                         "    prove\n"
                         "qed";
  runTest("Invalid Unmatched Parentheses", invalid4);

  // 7. 실패 테스트: 증명 라인에 들여쓰기 누락
  std::string invalid5 = "theorem No_Indent := P -> P\n"
                         "prove\n"
                         "qed";
  runTest("Invalid Missing Indent", invalid5);

  std::string valid6 = "theorem Identity := P -> P \\/ a\n"
                       "    intros P\n"
                       "    sorry\n"
                       "    exact P\n"
                       "qed";
  runTest("Valid Identity Implication", valid6);

  // 8. 실패 테스트: qed가 들여쓰기 됨
  std::string invalid6 = "theorem Identity := P -> P \\/ a\n"
                         "    intros P\n"
                         "    sorry\n"
                         "    exact P\n"
                         " qed";
  runTest("Invalid Indented QED", invalid6);

  // 9. 실패 테스트: qed 이후 같은 줄에 다른 토큰 존재
  std::string invalid7 = "theorem Extra_Tok := P -> P\n"
                         "    intros P\n"
                         "    exact P\n"
                         "qed a";
  runTest("Invalid QED with Extra Tokens", invalid7);

  std::string valid8 = "theorem Identity := d /\\ c \\/ P -> P \\/ a /\\ b\n"
                       "    intros P\n"
                       "    sorry\n"
                       "    exact P\n"
                       "qed";
  runTest("Valid Identity Implication", valid8);

  // 10. 성공 테스트: 문자열로 된 정리 이름
  std::string valid9 = "theorem \"My Theorem: P implies P\" := P -> P\n"
                       "    intros P\n"
                       "    exact P\n"
                       "qed\n"
                       "theorem \"test theorem\" := P -> P\n"
                       "    P = Q\n"
                       "    exact P\n"
                       "qed\n";
  runTest("Valid String Theorem Name", valid9);

  Lexer lexer = Lexer(valid9);
  Parser parser = Parser(lexer.tokenize());
  std::vector<std::shared_ptr<TheoremNode>> theorems = parser.parse();
  std::cout << (int)theorems[1]->proofTokens[0].type << "\n";
  std::cout << (int)theorems[1]->proofTokens[1].type << "\n";
  std::cout << (int)theorems[1]->proofTokens[2].type << "\n";
  std::cout << (int)theorems[1]->proofTokens[3].type << "\n";

  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> total_time = end_time - start_time;
  std::cout << "전체 파싱 소요 시간: " << total_time.count() << " ms\n";

  std::shared_ptr<PropTree> a =
      std::make_shared<Not>(std::make_shared<PropVar>(1));
  std::shared_ptr<PropTree> b =
      std::make_shared<Not>(std::make_shared<PropVar>(1));

  std::cout << a->isEqual(*b) << "\n";

  return 0;
}
