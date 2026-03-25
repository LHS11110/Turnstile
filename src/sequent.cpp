#include "sequent.hpp"
#include <stdexcept>

Sequent::Sequent(std::vector<Prop> antecedents, std::vector<Prop> succedents)
    : antecedents(antecedents), succedents(succedents) {}

std::string Sequent::toString() const {
  std::string result = "";
  for (int i = 0; i < static_cast<int>(antecedents.size()); i++) {
    result += antecedents[i]->toString() +
              (i != static_cast<int>(antecedents.size()) - 1 ? ", " : "");
  }
  result += " |- ";
  for (int i = 0; i < static_cast<int>(succedents.size()); i++) {
    result += succedents[i]->toString() +
              (i != static_cast<int>(succedents.size()) - 1 ? ", " : "");
  }
  return result;
}

PropTree::PropTree(TokenType type) : nodeType(type) {}

TokenType PropTree::getNodeType() const { return nodeType; }

Var::Var(int var) : PropTree(TokenType::IDENTIFIER), var(var) {}

bool Var::isEqual(const Prop other) const {
  if (other->getNodeType() != TokenType::IDENTIFIER)
    return false;
  return static_cast<const Var &>(*other).var == var;
}

Not::Not(Prop prop) : PropTree(TokenType::NOT), prop(prop) {}

bool Not::isEqual(const Prop other) const {
  if (other->getNodeType() != TokenType::NOT)
    return false;
  return static_cast<const Not &>(*other).prop->isEqual(prop);
}

And::And(Prop left, Prop right)
    : PropTree(TokenType::AND), left(left), right(right) {}

bool And::isEqual(const Prop other) const {
  if (other->getNodeType() != TokenType::AND)
    return false;
  else if (!static_cast<const And &>(*other).left->isEqual(left))
    return false;
  else if (!static_cast<const And &>(*other).right->isEqual(right))
    return false;
  return true;
}

Or::Or(Prop left, Prop right)
    : PropTree(TokenType::OR), left(left), right(right) {}

bool Or::isEqual(const Prop other) const {
  if (other->getNodeType() != TokenType::OR)
    return false;
  else if (!static_cast<const Or &>(*other).left->isEqual(left))
    return false;
  else if (!static_cast<const Or &>(*other).right->isEqual(right))
    return false;
  return true;
}

Implies::Implies(Prop left, Prop right)
    : PropTree(TokenType::RIGHTARROW), left(left), right(right) {}

bool Implies::isEqual(const Prop other) const {
  if (other->getNodeType() != TokenType::RIGHTARROW)
    return false;
  else if (!static_cast<const Implies &>(*other).left->isEqual(left))
    return false;
  else if (!static_cast<const Implies &>(*other).right->isEqual(right))
    return false;
  return true;
}

Equiv::Equiv(Prop left, Prop right)
    : PropTree(TokenType::EQUIV), left(left), right(right) {}

bool Equiv::isEqual(const Prop other) const {
  if (other->getNodeType() != TokenType::EQUIV)
    return false;
  else if (!static_cast<const Equiv &>(*other).left->isEqual(left))
    return false;
  else if (!static_cast<const Equiv &>(*other).right->isEqual(right))
    return false;
  return true;
}

int Var::getVar() const { return var; }

In::In(Var left, Var right)
    : PropTree(TokenType::IN), left(left), right(right) {}

bool In::isEqual(const Prop other) const {
  if (other->getNodeType() != TokenType::IN)
    return false;
  else if (static_cast<const In &>(*other).left.getVar() != left.getVar())
    return false;
  else if (static_cast<const In &>(*other).right.getVar() != right.getVar())
    return false;
  return true;
}

Forall::Forall(Prop var, Prop prop)
    : PropTree(TokenType::FORALL), var(var), prop(prop) {
  if (var->getNodeType() != TokenType::IDENTIFIER &&
      var->getNodeType() != TokenType::IN)
    throw std::runtime_error(
        "Forall requires an identifier or 'in' expression as the first "
        "argument");
}

bool Forall::isEqual(const Prop other) const {
  if (other->getNodeType() != TokenType::FORALL)
    return false;
  else if (!static_cast<const Forall &>(*other).var->isEqual(var))
    return false;
  else if (!static_cast<const Forall &>(*other).prop->isEqual(prop))
    return false;
  return true;
}

Exist::Exist(Prop var, Prop prop)
    : PropTree(TokenType::EXIST), var(var), prop(prop) {
  if (var->getNodeType() != TokenType::IDENTIFIER &&
      var->getNodeType() != TokenType::IN)
    throw std::runtime_error(
        "Exist requires an identifier or 'in' expression as the first "
        "argument");
}

bool Exist::isEqual(const Prop other) const {
  if (other->getNodeType() != TokenType::EXIST)
    return false;
  else if (!static_cast<const Exist &>(*other).var->isEqual(var))
    return false;
  else if (!static_cast<const Exist &>(*other).prop->isEqual(prop))
    return false;
  return true;
}

std::map<std::string, int> Var::nameToId;
std::map<int, std::string> Var::idToName;
int Var::nextId = 256;

int Var::registerName(const std::string &name) {
  if (name.length() == 1) {
    return static_cast<int>(name[0]);
  }
  if (nameToId.find(name) != nameToId.end()) {
    return nameToId[name];
  }
  int id = nextId++;
  nameToId[name] = id;
  idToName[id] = name;
  return id;
}

std::string Var::getName(int id) {
  if (id < 256) {
    if ((id >= 'A' && id <= 'Z') || (id >= 'a' && id <= 'z')) {
      return std::string(1, static_cast<char>(id));
    }
    return std::to_string(id);
  }
  auto it = idToName.find(id);
  if (it != idToName.end()) {
    return it->second;
  }
  return "?" + std::to_string(id);
}

std::string Var::toString() const { return getName(var); }

std::string Not::toString() const { return "~" + prop->toString(); }

std::string And::toString() const {
  return "(" + left->toString() + " /\\ " + right->toString() + ")";
}

Prop And::getLeft() const { return left; }
Prop And::getRight() const { return right; }

std::string Or::toString() const {
  return "(" + left->toString() + " \\/ " + right->toString() + ")";
}

Prop Or::getLeft() const { return left; }
Prop Or::getRight() const { return right; }

std::string Implies::toString() const {
  return "(" + left->toString() + " -> " + right->toString() + ")";
}

Prop Implies::getLeft() const { return left; }
Prop Implies::getRight() const { return right; }

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

Equal::Equal(Var left, Var right)
    : PropTree(TokenType::EQUAL), left(left), right(right) {}

bool Equal::isEqual(const Prop other) const {
  if (other->getNodeType() != TokenType::EQUAL)
    return false;
  else if (!(static_cast<const Equal &>(*other).left.getVar() == left.getVar()))
    return false;
  else if (!(static_cast<const Equal &>(*other).right.getVar() ==
             right.getVar()))
    return false;
  return true;
}

std::string Equal::toString() const {
  return "(" + left.toString() + " = " + right.toString() + ")";
}

const std::vector<Prop> &Sequent::getAntecedents() const { return antecedents; }
const std::vector<Prop> &Sequent::getSuccedents() const { return succedents; }

Sequent Sequent::addAntecedent(Prop p) const {
  std::vector<Prop> newAnt = antecedents;
  newAnt.push_back(p);
  return Sequent(newAnt, succedents);
}

Sequent Sequent::addSuccedent(Prop p) const {
  std::vector<Prop> newSuc = succedents;
  newSuc.push_back(p);
  return Sequent(antecedents, newSuc);
}

Sequent Sequent::removeAntecedent(size_t index) const {
  std::vector<Prop> newAnt = antecedents;
  if (index < newAnt.size())
    newAnt.erase(newAnt.begin() + index);
  return Sequent(newAnt, succedents);
}

Sequent Sequent::removeSuccedent(size_t index) const {
  std::vector<Prop> newSuc = succedents;
  if (index < newSuc.size())
    newSuc.erase(newSuc.begin() + index);
  return Sequent(antecedents, newSuc);
}

AndL_1::AndL_1(Prop p) : Rule(), p(p) {}

Sequent AndL_1::apply(const Sequent &seq) const {
  auto ants = seq.getAntecedents();
  ants.back() = static_pointer_cast<PropTree>(make_shared<And>(ants.back(), p));
  return {ants, seq.getSuccedents()};
}