#include "turnstile.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "sequent.hpp"
#include <functional>
#include <memory>

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
    if (proof[0].back().isEqual(
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
  } else {
    std::shared_ptr<Eval> eval = std::static_pointer_cast<Eval>(ast);
    if (proof.size() != eval->getIndentLevel())
      throw std::runtime_error("Indent level does not match");
    if (eval->getNodeType() == TokenType::NEW_BRANCH) {
      proof.push_back({});
    } else if (eval->getNodeType() == TokenType::END_BRANCH) {
      if (proof.size() < 2)
        throw std::runtime_error("Invalid proof");
      proof[proof.size() - 2].push_back(proof.back().back());
      proof.pop_back();
    } else {
      switch (eval->getNodeType()) {
      case TokenType::ANDL1: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::ANDL2: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::ORL: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::IMPLL: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::NOTL: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::FORALLL: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::EXISTL: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::WL: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::CL: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::PL: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::ORR1: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::ORR2: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::ANDR: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::IMPLR: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::NOTR: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::FORALLR: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::EXISTR: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::WR: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::CR: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::PR: {
        Sequent sequent = eval->apply({proof.back().back()});
        proof.back().push_back(sequent);
        break;
      }
      case TokenType::ID: {
        Sequent sequent = eval->apply({});
        proof.back().push_back(sequent);
        break;
      }
      default:
        throw std::runtime_error("Invalid proof");
      }
    }
  }
}
