#include "turnstile.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "sequent.hpp"
#include <functional>
#include <memory>
#include <iostream>

std::vector<std::vector<Sequent>> Turnstile::proof = {};
std::unordered_map<const Var, const Sequent> Turnstile::theorems = {};

std::size_t std::hash<const Var>::operator()(const Var &k) const {
  return std::hash<int>()(k.getVar());
}

bool Turnstile::isTheoremName(const Var &var) {
  return theorems.find(var) != theorems.end();
}

const Sequent Turnstile::getTheorem(const Var &var) {
  if (!isTheoremName(var))
    throw std::runtime_error("Theorem name does not exist");
  return theorems.at(var);
}

void Turnstile::addTheorem(const Var &var, const Sequent &sequent) {
  if (isTheoremName(var))
    throw std::runtime_error("Theorem name already exists");
  theorems.insert({var, sequent});
}

const Sequent Head::getSequent() const { return *seq; }

Turnstile::Turnstile(const std::shared_ptr<const Head> &head)
    : head(head), isProvable(false) {}

void Turnstile::interpret(const std::string &input) {
  Lexer lexer(input);
  auto tokens = lexer.tokenize();
  Parser parser(tokens);
  auto ast = parser.parse();
  if (ast == nullptr) { // QED가 파싱된 경우
    if (!isProvable)    // 증명 모드가 아닌데 QED가 입력된 경우
      throw std::runtime_error("Invalid proof");
    isProvable = false; // 증명 모드 종료
    if (proof.size() != 1) {
      proof.clear(); // 증명 초기화
      throw std::runtime_error("Invalid proof");
    }
    if (!proof[0].back().isEqual(
            head->getSequent())) { // 증명이 끝났는데 증명하려는 명제와 같지
                                   // 않은 경우
      proof.clear();               // 증명 초기화
      throw std::runtime_error("Invalid proof");
    }
    addTheorem(head->getName(), proof[0].back());
    proof.clear(); // 증명 초기화
    return;
  }
  if (ast->getNodeType() == TokenType::THEOREM && isProvable)
    throw std::runtime_error("Theorem cannot be defined in a provable context");
  if (ast->getNodeType() != TokenType::THEOREM && !isProvable)
    throw std::runtime_error("Provable cannot be defined in a theorem context");
  if (ast->getNodeType() == TokenType::THEOREM) {
    const std::shared_ptr<const Head> head =
        std::static_pointer_cast<const Head>(ast);
    if (isTheoremName(head->getName()))
      throw std::runtime_error("Theorem name already exists");
    isProvable = true;
    this->head = head;
    proof.clear();
    proof.push_back({});
  } else {
    const std::shared_ptr<const Eval> eval =
        std::static_pointer_cast<const Eval>(ast);
    size_t expectedIndent = proof.size();
    if (eval->getNodeType() == TokenType::END_BRANCH) {
      if (proof.empty())
        throw std::runtime_error("Invalid proof: no active theorem");
      expectedIndent = proof.size() - 1;
    }
    if (expectedIndent != eval->getIndentLevel())
      throw std::runtime_error("Indent level does not match");
    if (eval->getNodeType() == TokenType::NEW_BRANCH) {
      proof.push_back({});
      std::cout << "  [Branch " << proof.size() - 1 << " opened]" << std::endl;
    } else if (eval->getNodeType() == TokenType::END_BRANCH) {
      if (proof.size() < 2)
        throw std::runtime_error("Invalid proof");
      Sequent merged = proof.back().back();
      proof[proof.size() - 2].push_back(merged);
      proof.pop_back();
      std::cout << "  [Branch closed. Pushed sequent: " << merged.toString() << "]" << std::endl;
    } else {
      switch (eval->getNodeType()) {
      // 0-premise rules (Axioms)
      case TokenType::ID:
      case TokenType::USE: {
        Sequent sequent = eval->apply({});
        proof.back().push_back(sequent);
        std::cout << "  () -> (" << sequent.toString() << ")" << std::endl;
        break;
      }

      // 2-premise rules
      case TokenType::ORL:
      case TokenType::IMPLL:
      case TokenType::ANDR:
      case TokenType::CUT: {
        if (proof.back().size() < 2)
          throw std::runtime_error("Invalid proof: not enough premises on stack");
        Sequent seq2 = proof.back().back();
        proof.back().pop_back();
        Sequent seq1 = proof.back().back();
        proof.back().pop_back();
        Sequent sequent = eval->apply({seq1, seq2});
        proof.back().push_back(sequent);
        std::cout << "  ((" << seq1.toString() << "), (" << seq2.toString() << ")) -> (" << sequent.toString() << ")" << std::endl;
        break;
      }

      // 1-premise rules
      case TokenType::ANDL1:
      case TokenType::ANDL2:
      case TokenType::NOTL:
      case TokenType::FORALLL:
      case TokenType::EXISTL:
      case TokenType::WL:
      case TokenType::CL:
      case TokenType::PL:
      case TokenType::ORR1:
      case TokenType::ORR2:
      case TokenType::IMPLR:
      case TokenType::NOTR:
      case TokenType::FORALLR:
      case TokenType::EXISTR:
      case TokenType::WR:
      case TokenType::CR:
      case TokenType::PR: {
        if (proof.back().empty())
          throw std::runtime_error("Invalid proof: no premise on stack");
        Sequent seq = proof.back().back();
        proof.back().pop_back();
        Sequent sequent = eval->apply({seq});
        proof.back().push_back(sequent);
        std::cout << "  (" << seq.toString() << ") -> (" << sequent.toString() << ")" << std::endl;
        break;
      }

      default:
        throw std::runtime_error("Invalid proof: unknown rule type");
      }
    }
  }
}
