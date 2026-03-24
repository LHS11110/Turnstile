#include "sequent.hpp"

Sequent::Sequent(std::vector<int> antecedents, std::vector<int> succedents)
    : antecedents(antecedents), succedents(succedents) {}

std::string Sequent::toString() const {
  std::string result = "";
  for (int i = 0; i < static_cast<int>(antecedents.size()); i++) {
    result += std::to_string(antecedents[i]) +
              (i != static_cast<int>(antecedents.size()) - 1 ? ", " : "");
  }
  result += " |- ";
  for (int i = 0; i < static_cast<int>(succedents.size()); i++) {
    result += std::to_string(succedents[i]) +
              (i != static_cast<int>(succedents.size()) - 1 ? ", " : "");
  }
  return result;
}

Sequent AndL_1::apply(std::vector<Sequent> sequents) {
  return Sequent(sequents[0].antecedents, sequents[0].succedents);
}

PropTree::PropTree(TokenType type) : nodeType(type) {}

TokenType PropTree::getNodeType() const { return nodeType; }

Var::Var(int var) : PropTree(TokenType::IDENTIFIER), var(var) {}

bool Var::isEqual(std::shared_ptr<PropTree> other) const {
  if (other->getNodeType() != TokenType::IDENTIFIER)
    return false;
  return static_cast<Var &>(*other).var == var;
}

Not::Not(std::shared_ptr<PropTree> prop)
    : PropTree(TokenType::NOT), prop(prop) {}

bool Not::isEqual(std::shared_ptr<PropTree> other) const {
  if (other->getNodeType() != TokenType::NOT)
    return false;
  return static_cast<Not &>(*other).prop->isEqual(prop);
}

And::And(std::shared_ptr<PropTree> left, std::shared_ptr<PropTree> right)
    : PropTree(TokenType::AND), left(left), right(right) {}

bool And::isEqual(std::shared_ptr<PropTree> other) const {
  if (other->getNodeType() != TokenType::AND)
    return false;
  else if (!static_cast<And &>(*other).left->isEqual(left))
    return false;
  else if (!static_cast<And &>(*other).right->isEqual(right))
    return false;
  return true;
}

Or::Or(std::shared_ptr<PropTree> left, std::shared_ptr<PropTree> right)
    : PropTree(TokenType::OR), left(left), right(right) {}

bool Or::isEqual(std::shared_ptr<PropTree> other) const {
  if (other->getNodeType() != TokenType::OR)
    return false;
  else if (!static_cast<Or &>(*other).left->isEqual(left))
    return false;
  else if (!static_cast<Or &>(*other).right->isEqual(right))
    return false;
  return true;
}

Implies::Implies(std::shared_ptr<PropTree> left,
                 std::shared_ptr<PropTree> right)
    : PropTree(TokenType::RIGHTARROW), left(left), right(right) {}

bool Implies::isEqual(std::shared_ptr<PropTree> other) const {
  if (other->getNodeType() != TokenType::RIGHTARROW)
    return false;
  else if (!static_cast<Implies &>(*other).left->isEqual(left))
    return false;
  else if (!static_cast<Implies &>(*other).right->isEqual(right))
    return false;
  return true;
}

Equiv::Equiv(std::shared_ptr<PropTree> left, std::shared_ptr<PropTree> right)
    : PropTree(TokenType::EQUIV), left(left), right(right) {}

bool Equiv::isEqual(std::shared_ptr<PropTree> other) const {
  if (other->getNodeType() != TokenType::EQUIV)
    return false;
  else if (!static_cast<Equiv &>(*other).left->isEqual(left))
    return false;
  else if (!static_cast<Equiv &>(*other).right->isEqual(right))
    return false;
  return true;
}

int Var::getVar() const { return var; }

In::In(Var left, Var right)
    : PropTree(TokenType::IN), left(left), right(right) {}

bool In::isEqual(std::shared_ptr<PropTree> other) const {
  if (other->getNodeType() != TokenType::IN)
    return false;
  else if (static_cast<In &>(*other).left.getVar() != left.getVar())
    return false;
  else if (static_cast<In &>(*other).right.getVar() != right.getVar())
    return false;
  return true;
}

Forall::Forall(std::shared_ptr<PropTree> var, std::shared_ptr<PropTree> prop)
    : PropTree(TokenType::FORALL), var(var), prop(prop) {}

bool Forall::isEqual(std::shared_ptr<PropTree> other) const {
  if (other->getNodeType() != TokenType::FORALL)
    return false;
  else if (!static_cast<Forall &>(*other).var->isEqual(var))
    return false;
  else if (!static_cast<Forall &>(*other).prop->isEqual(prop))
    return false;
  return true;
}

Exist::Exist(std::shared_ptr<PropTree> var, std::shared_ptr<PropTree> prop)
    : PropTree(TokenType::EXIST), var(var), prop(prop) {}

bool Exist::isEqual(std::shared_ptr<PropTree> other) const {
  if (other->getNodeType() != TokenType::EXIST)
    return false;
  else if (!static_cast<Exist &>(*other).var->isEqual(var))
    return false;
  else if (!static_cast<Exist &>(*other).prop->isEqual(prop))
    return false;
  return true;
}

std::string Var::toString() const { return std::to_string(var); }

std::string Not::toString() const { return "~" + prop->toString(); }

std::string And::toString() const {
  return "(" + left->toString() + " /\\ " + right->toString() + ")";
}

std::string Or::toString() const {
  return "(" + left->toString() + " \\/ " + right->toString() + ")";
}

std::string Implies::toString() const {
  return "(" + left->toString() + " -> " + right->toString() + ")";
}

std::string Equiv::toString() const {
  return "(" + left->toString() + " <=> " + right->toString() + ")";
}

std::string In::toString() const {
  return "(" + left.toString() + " in " + right.toString() + ")";
}

std::string Forall::toString() const {
  return "(forall " + var->toString() + ", " + prop->toString() + ")";
}

std::string Exist::toString() const {
  return "(exists " + var->toString() + ", " + prop->toString() + ")";
}
