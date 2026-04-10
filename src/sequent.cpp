#include "sequent.hpp"
#include "lexer.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

Sequent::Sequent(const std::vector<Prop> &antecedents,
                 const std::vector<Prop> &succedents)
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

PropNode::PropNode(TokenType type) : nodeType(type) {}

TokenType PropNode::getNodeType() const { return nodeType; }

Var::Var(int var) : PropNode(TokenType::IDENTIFIER), var(var) {}

Var::Var(const std::string &name)
    : PropNode(TokenType::IDENTIFIER), var(registerName(name)) {}

bool Var::isEqual(const Prop &other) const {
  if (other->getNodeType() != TokenType::IDENTIFIER)
    return false;
  return static_cast<const Var &>(*other).var == var;
}

Not::Not(Prop prop) : PropNode(TokenType::NOT), prop(prop) {}

bool Not::isEqual(const Prop &other) const {
  if (other->getNodeType() != TokenType::NOT)
    return false;
  return static_cast<const Not &>(*other).prop->isEqual(prop);
}

And::And(Prop left, Prop right)
    : PropNode(TokenType::AND), left(left), right(right) {}

bool And::isEqual(const Prop &other) const {
  if (other->getNodeType() != TokenType::AND)
    return false;
  else if (!static_cast<const And &>(*other).left->isEqual(left))
    return false;
  else if (!static_cast<const And &>(*other).right->isEqual(right))
    return false;
  return true;
}

Or::Or(Prop left, Prop right)
    : PropNode(TokenType::OR), left(left), right(right) {}

bool Or::isEqual(const Prop &other) const {
  if (other->getNodeType() != TokenType::OR)
    return false;
  else if (!static_cast<const Or &>(*other).left->isEqual(left))
    return false;
  else if (!static_cast<const Or &>(*other).right->isEqual(right))
    return false;
  return true;
}

Implies::Implies(Prop left, Prop right)
    : PropNode(TokenType::RIGHTARROW), left(left), right(right) {}

bool Implies::isEqual(const Prop &other) const {
  if (other->getNodeType() != TokenType::RIGHTARROW)
    return false;
  else if (!static_cast<const Implies &>(*other).left->isEqual(left))
    return false;
  else if (!static_cast<const Implies &>(*other).right->isEqual(right))
    return false;
  return true;
}

Equiv::Equiv(Prop left, Prop right)
    : PropNode(TokenType::EQUIV), left(left), right(right) {}

bool Equiv::isEqual(const Prop &other) const {
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
    : PropNode(TokenType::IN), left(left), right(right) {}

bool In::isEqual(const Prop &other) const {
  if (other->getNodeType() != TokenType::IN)
    return false;
  else if (static_cast<const In &>(*other).left.getVar() != left.getVar())
    return false;
  else if (static_cast<const In &>(*other).right.getVar() != right.getVar())
    return false;
  return true;
}

Forall::Forall(Prop var, Prop prop)
    : PropNode(TokenType::FORALL), var(var), prop(prop) {
  if (var->getNodeType() != TokenType::IDENTIFIER &&
      var->getNodeType() != TokenType::IN)
    throw std::runtime_error(
        "Forall requires an identifier or 'in' expression as the first "
        "argument");
}

bool Forall::isEqual(const Prop &other) const {
  if (other->getNodeType() != TokenType::FORALL)
    return false;
  else if (!static_cast<const Forall &>(*other).var->isEqual(var))
    return false;
  else if (!static_cast<const Forall &>(*other).prop->isEqual(prop))
    return false;
  return true;
}

Exist::Exist(Prop var, Prop prop)
    : PropNode(TokenType::EXIST), var(var), prop(prop) {
  if (var->getNodeType() != TokenType::IDENTIFIER &&
      var->getNodeType() != TokenType::IN)
    throw std::runtime_error(
        "Exist requires an identifier or 'in' expression as the first "
        "argument");
}

bool Exist::isEqual(const Prop &other) const {
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
int Var::nextId = 0;

int Var::registerName(const std::string &name) {
  if (nameToId.find(name) != nameToId.end())
    return nameToId[name];
  nameToId[name] = nextId;
  idToName[nextId++] = name;
  return nextId - 1;
}

std::string Var::getName(int id) {
  auto it = idToName.find(id);
  if (it != idToName.end())
    return it->second;
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
    : PropNode(TokenType::EQUAL), left(left), right(right) {}

bool Equal::isEqual(const Prop &other) const {
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
  if (index >= antecedents.size())
    throw std::runtime_error("removeAntecedent: index out of bounds");
  std::vector<Prop> newAnt = antecedents;
  newAnt.erase(newAnt.begin() + index);
  return Sequent(newAnt, succedents);
}

Sequent Sequent::removeSuccedent(size_t index) const {
  if (index >= succedents.size())
    throw std::runtime_error("removeSuccedent: index out of bounds");
  std::vector<Prop> newSuc = succedents;
  newSuc.erase(newSuc.begin() + index);
  return Sequent(antecedents, newSuc);
}

AndL_1::AndL_1(Prop p) : Rule(), p(p) {}

Sequent AndL_1::apply(
    const std::vector<Sequent> &seqs) const { // seqs := {Gamma, A |- Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid AndL_1 application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma, A
  if (ants.size() < 1)
    throw std::runtime_error("Invalid AndL_1 application");
  ants.back() = static_pointer_cast<PropNode>(
      make_shared<And>(ants.back(), p));  // Gamma, A /\ B
  return {ants, seqs[0].getSuccedents()}; // Gamma, A /\ B |- Delta
}

AndL_2::AndL_2(Prop p) : Rule(), p(p) {}

Sequent AndL_2::apply(
    const std::vector<Sequent> &seqs) const { // seqs := {Gamma, B |- Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid AndL_2 application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma, B
  if (ants.size() < 1)
    throw std::runtime_error("Invalid AndL_2 application");
  ants.back() = static_pointer_cast<PropNode>(
      make_shared<And>(p, ants.back()));  // Gamma, A /\ B
  return {ants, seqs[0].getSuccedents()}; // Gamma, A /\ B |- Delta
}

Sequent OrL::apply(const std::vector<Sequent> &seqs)
    const { // seqs := {Gamma, A |- Delta}, {Gamma, B |- Delta}
  if (seqs.size() != 2)
    throw std::runtime_error("Invalid OrL application");
  std::vector<Prop> ants_1 = seqs[0].getAntecedents(); // Gamma, A
  std::vector<Prop> ants_2 = seqs[1].getAntecedents(); // Gamma, B
  std::vector<Prop> sucs_1 = seqs[0].getSuccedents();  // Delta
  std::vector<Prop> sucs_2 = seqs[1].getSuccedents();  // Delta
  if (ants_1.size() < 1 || ants_2.size() < 1 ||
      sucs_1.size() != sucs_2.size() || ants_1.size() != ants_2.size())
    throw std::runtime_error("Invalid OrL application");
  for (size_t i = 0; i < sucs_1.size(); i++)
    if (!sucs_1[i]->isEqual(sucs_2[i]))
      throw std::runtime_error("Invalid OrL application");
  for (size_t i = 0; i < ants_1.size() - 1; i++)
    if (!ants_1[i]->isEqual(ants_2[i]))
      throw std::runtime_error("Invalid OrL application");
  Prop p_1 = ants_1.back(); // A
  Prop p_2 = ants_2.back(); // B
  ants_1.pop_back();        // Gamma
  ants_1.push_back(static_pointer_cast<PropNode>(
      make_shared<Or>(p_1, p_2))); // Gamma, A \/ B
  return {ants_1, sucs_1};         // Gamma, A \/ B |- Delta
}

Sequent ImpliesL::apply(const std::vector<Sequent> &seqs)
    const { // seqs := {Gamma |- A, Delta}, {Sigma, B |- Pi}
  if (seqs.size() != 2)
    throw std::runtime_error("Invalid ImpliesL application");
  std::vector<Prop> ants_1 = seqs[0].getAntecedents(); // Gamma
  std::vector<Prop> ants_2 = seqs[1].getAntecedents(); // Sigma, B
  std::vector<Prop> sucs_1 = seqs[0].getSuccedents();  // A, Delta
  std::vector<Prop> sucs_2 = seqs[1].getSuccedents();  // Pi
  if (sucs_1.size() < 1 || ants_2.size() < 1)
    throw std::runtime_error("Invalid ImpliesL application");
  Prop p_1 = sucs_1.front();                                 // A
  Prop p_2 = ants_2.back();                                  // B
  sucs_1.erase(sucs_1.begin());                              // Delta
  ants_2.pop_back();                                         // Sigma
  ants_1.insert(ants_1.end(), ants_2.begin(), ants_2.end()); // Gamma, Sigma
  sucs_1.insert(sucs_1.end(), sucs_2.begin(), sucs_2.end()); // Delta, Pi
  ants_1.push_back(static_pointer_cast<PropNode>(
      make_shared<Implies>(p_1, p_2))); // Gamma, Sigma, A -> B
  return {ants_1, sucs_1};              // Gamma, Sigma, A -> B |- Delta, Pi
}

Sequent NotL::apply(
    const std::vector<Sequent> &seqs) const { // seqs := {Gamma |- A, Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid NotL application");
  std::vector<Prop> ants_1 = seqs[0].getAntecedents(); // Gamma
  std::vector<Prop> sucs_1 = seqs[0].getSuccedents();  // A, Delta
  if (sucs_1.size() < 1)
    throw std::runtime_error("Invalid NotL application");
  Prop p_1 = sucs_1.front();    // A
  sucs_1.erase(sucs_1.begin()); // Delta
  ants_1.push_back(
      static_pointer_cast<PropNode>(make_shared<Not>(p_1))); // Gamma, ~A
  return {ants_1, sucs_1}; // Gamma, ~A |- Delta
}

const Prop replaceVar(const Var &oldVar, const Var &newVar, const Prop &prop) {
  return prop->replaceVar(oldVar, newVar, prop);
}

const Prop Var::replaceVar(const Var &oldVar, const Var &newVar,
                           const Prop &me) const {
  if (var == oldVar.getVar() && oldVar.getVar() != newVar.getVar())
    return std::static_pointer_cast<const PropNode>(
        std::make_shared<Var>(newVar));
  else
    return me;
}

const Prop Not::replaceVar(const Var &oldVar, const Var &newVar,
                           const Prop &me) const {
  const Prop newProp = prop->replaceVar(oldVar, newVar, prop);
  if (newProp ==
      prop) // 주소값 비교이므로, 새로운 객체가 생성되지 않았다면 그대로 반환
    return me;
  return std::static_pointer_cast<const PropNode>(
      std::make_shared<Not>(newProp));
}

const Prop And::replaceVar(const Var &oldVar, const Var &newVar,
                           const Prop &me) const {
  const Prop newLeft = left->replaceVar(oldVar, newVar, left);
  const Prop newRight = right->replaceVar(oldVar, newVar, right);
  if (newLeft == left && newRight == right)
    return me;
  return std::static_pointer_cast<const PropNode>(
      std::make_shared<And>(newLeft, newRight));
}

const Prop Or::replaceVar(const Var &oldVar, const Var &newVar,
                          const Prop &me) const {
  const Prop newLeft = left->replaceVar(oldVar, newVar, left);
  const Prop newRight = right->replaceVar(oldVar, newVar, right);
  if (newLeft == left && newRight == right)
    return me;
  return std::static_pointer_cast<const PropNode>(
      std::make_shared<Or>(newLeft, newRight));
}

const Prop Implies::replaceVar(const Var &oldVar, const Var &newVar,
                               const Prop &me) const {
  const Prop newLeft = left->replaceVar(oldVar, newVar, left);
  const Prop newRight = right->replaceVar(oldVar, newVar, right);
  if (newLeft == left && newRight == right)
    return me;
  return std::static_pointer_cast<const PropNode>(
      std::make_shared<Implies>(newLeft, newRight));
}

const Prop Equiv::replaceVar(const Var &oldVar, const Var &newVar,
                             const Prop &me) const {
  const Prop newLeft = left->replaceVar(oldVar, newVar, left);
  const Prop newRight = right->replaceVar(oldVar, newVar, right);
  if (newLeft == left && newRight == right)
    return me;
  return std::static_pointer_cast<const PropNode>(
      std::make_shared<Equiv>(newLeft, newRight));
}

const Var Var::replaceVar(const Var &oldVar, const Var &newVar) const {
  if (var == oldVar.getVar() && oldVar.getVar() != newVar.getVar())
    return newVar;
  else
    return *this;
}

const Prop In::replaceVar(const Var &oldVar, const Var &newVar,
                          const Prop &me) const {
  const Var newLeft = left.replaceVar(oldVar, newVar);
  const Var newRight = right.replaceVar(oldVar, newVar);
  if (newLeft.getVar() == left.getVar() && newRight.getVar() == right.getVar())
    return me;
  return std::static_pointer_cast<const PropNode>(
      std::make_shared<In>(newLeft, newRight));
}

const Prop Forall::replaceVar(const Var &oldVar, const Var &newVar,
                              const Prop &me) const {
  const Prop newVar_ = var->replaceVar(oldVar, newVar, var);
  const Prop newProp = prop->replaceVar(oldVar, newVar, prop);
  if (newVar_ == var && newProp == prop)
    return me;
  return std::static_pointer_cast<const PropNode>(
      std::make_shared<Forall>(newVar_, newProp));
}

const Prop Exist::replaceVar(const Var &oldVar, const Var &newVar,
                             const Prop &me) const {
  const Prop newVar_ = var->replaceVar(oldVar, newVar, var);
  const Prop newProp = prop->replaceVar(oldVar, newVar, prop);
  if (newVar_ == var && newProp == prop)
    return me;
  return std::static_pointer_cast<const PropNode>(
      std::make_shared<Exist>(newVar_, newProp));
}

const Prop Equal::replaceVar(const Var &oldVar, const Var &newVar,
                             const Prop &me) const {
  const Var newLeft = left.replaceVar(oldVar, newVar);
  const Var newRight = right.replaceVar(oldVar, newVar);
  if (newLeft.getVar() == left.getVar() && newRight.getVar() == right.getVar())
    return me;
  return std::static_pointer_cast<const PropNode>(
      std::make_shared<Equal>(newLeft, newRight));
}

ForallL::ForallL(Var t, Var x) : Rule(), t(t), x(x) {}

Sequent ForallL::apply(const std::vector<Sequent> &seqs)
    const { // seqs := {Gamma, A[t/x] |- Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid ForallL application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma, A[t/x]
  if (ants.size() < 1)
    throw std::runtime_error("Invalid ForallL application");
  Prop last = ants.back(); // A[t/x]
  ants.pop_back();         // Gamma
  Prop original =
      last->replaceVar(t, x, last); // A[t/x] -> A (undo substitution)
  ants.push_back(std::make_shared<Forall>(std::make_shared<Var>(x),
                                          original)); // Gamma, forall x. A
  return {ants, seqs[0].getSuccedents()}; // Gamma, forall x. A |- Delta
}

ExistL::ExistL(Var y, Var x) : Rule(), y(y), x(x) {}

Sequent ExistL::apply(const std::vector<Sequent> &seqs)
    const { // seqs := {Gamma, A[y/x] |- Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid ExistL application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma, A[y/x]
  if (ants.size() < 1)
    throw std::runtime_error("Invalid ExistL application");
  Prop last = ants.back();
  ants.pop_back();
  Prop original =
      last->replaceVar(y, x, last); // A[y/x] -> A (undo substitution)
  ants.push_back(std::make_shared<Exist>(std::make_shared<Var>(x),
                                         original)); // Gamma, exists x. A
  for (auto &iter : ants)
    if (iter->checkVar(y))
      throw std::runtime_error("Invalid ExistL application");
  for (auto &iter : seqs[0].getSuccedents())
    if (iter->checkVar(y))
      throw std::runtime_error("Invalid ExistL application");
  return {ants, seqs[0].getSuccedents()}; // Gamma, exists x. A |- Delta
}

Identity::Identity(Prop p) : Rule(), p(p) {}

Sequent Identity::apply(const std::vector<Sequent> &seqs) const {
  if (seqs.size() != 0)
    throw std::runtime_error("Invalid Identity application");
  return Sequent({p}, {p}); // A |- A
}

Sequent Cut::apply(const std::vector<Sequent> &seqs)
    const { // seqs := {Gamma |- Delta, A}, {A, Sigma |- Pi}
  if (seqs.size() != 2)
    throw std::runtime_error("Invalid Cut application");
  std::vector<Prop> ants_1 = seqs[0].getAntecedents(); // Gamma
  std::vector<Prop> sucs_1 = seqs[0].getSuccedents();  // Delta, A
  std::vector<Prop> ants_2 = seqs[1].getAntecedents(); // A, Sigma
  std::vector<Prop> sucs_2 = seqs[1].getSuccedents();  // Pi
  Prop p_1 = sucs_1.back();                            // A
  sucs_1.pop_back();                                   // Delta
  Prop p_2 = ants_2.front();                           // A
  if (!p_1->isEqual(p_2))
    throw std::runtime_error("Invalid Cut application");
  ants_2.erase(ants_2.begin()); // Sigma
  std::vector<Prop> ants = ants_1;
  ants.insert(ants.end(), ants_2.begin(), ants_2.end()); // Gamma, Sigma
  std::vector<Prop> sucs = sucs_1;
  sucs.insert(sucs.end(), sucs_2.begin(), sucs_2.end()); // Delta, Pi
  return {ants, sucs}; // Gamma, Sigma |- Delta, Pi
}

WeakeningL::WeakeningL(Prop p) : Rule(), p(p) {}

Sequent WeakeningL::apply(
    const std::vector<Sequent> &seqs) const { // seqs := {Gamma |- Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid WeakeningL application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma
  std::vector<Prop> sucs = seqs[0].getSuccedents();  // Delta
  ants.push_back(p);                                 // Gamma, A
  return {ants, sucs};                               // Gamma, A |- Delta
}

Sequent ContractionL::apply(
    const std::vector<Sequent> &seqs) const { // seqs := {Gamma, A, A |- Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid ContractionL application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma, A, A
  std::vector<Prop> sucs = seqs[0].getSuccedents();  // Delta
  if (ants.size() < 2)
    throw std::runtime_error("Invalid ContractionL application");
  if (!(ants.back()->isEqual(ants[ants.size() - 2])))
    throw std::runtime_error("Invalid ContractionL application");
  ants.pop_back();     // Gamma, A
  return {ants, sucs}; // Gamma, A |- Delta
}

PermutationL::PermutationL(size_t i) : Rule(), i(i) {}

Sequent PermutationL::apply(const std::vector<Sequent> &seqs)
    const { // seqs := {Gamma, A, B, Delta |- Pi}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid PermutationL application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma, A, B, Delta
  std::vector<Prop> sucs = seqs[0].getSuccedents();  // Pi
  if (ants.size() < 2 || i + 1 >= ants.size())
    throw std::runtime_error("Invalid PermutationL application");
  std::swap(ants[i], ants[i + 1]); // Gamma, B, A, Delta
  return {ants, sucs};             // Gamma, B, A, Delta |- Pi
}

OrR_1::OrR_1(Prop p) : Rule(), p(p) {}

Sequent OrR_1::apply(
    const std::vector<Sequent> &seqs) const { // seqs := {Gamma |- A, Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid OrR_1 application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma
  std::vector<Prop> sucs = seqs[0].getSuccedents();  // A, Delta
  if (sucs.size() < 1)
    throw std::runtime_error("Invalid OrR_1 application");
  sucs.front() = std::make_shared<Or>(sucs.front(), p); // A \/ B, Delta
  return {ants, sucs}; // Gamma |- A or B, Delta
}

OrR_2::OrR_2(Prop p) : Rule(), p(p) {}

Sequent OrR_2::apply(
    const std::vector<Sequent> &seqs) const { // seqs := {Gamma |- A, Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid OrR_2 application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma
  std::vector<Prop> sucs = seqs[0].getSuccedents();  // A, Delta
  if (sucs.size() < 1)
    throw std::runtime_error("Invalid OrR_2 application");
  sucs.front() = std::make_shared<Or>(p, sucs.front()); // B \/ A, Delta
  return {ants, sucs}; // Gamma |- B \/ A, Delta
}

Sequent ImpliesR::apply(
    const std::vector<Sequent> &seqs) const { // seqs := {Gamma, A |- B, Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid ImpliesR application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma, A
  std::vector<Prop> sucs = seqs[0].getSuccedents();  // B, Delta
  if (ants.size() < 1 || sucs.size() < 1)
    throw std::runtime_error("Invalid ImpliesR application");
  Prop last = ants.back();                                      // A
  ants.pop_back();                                              // Gamma
  sucs.front() = std::make_shared<Implies>(last, sucs.front()); // A -> B, Delta
  return {ants, sucs}; // Gamma |- A -> B, Delta
}

Sequent AndR::apply(const std::vector<Sequent> &seqs)
    const { // seqs := {Gamma |- A, Delta}, {Gamma |- B, Delta}
  if (seqs.size() != 2)
    throw std::runtime_error("Invalid AndR application");
  else if (seqs[0].getAntecedents().size() != seqs[1].getAntecedents().size())
    throw std::runtime_error("Invalid AndR application");
  for (size_t i = 0; i < seqs[0].getAntecedents().size(); i++) {
    if (!seqs[0].getAntecedents()[i]->isEqual(seqs[1].getAntecedents()[i]))
      throw std::runtime_error("Invalid AndR application");
  }
  std::vector<Prop> ants = seqs[0].getAntecedents();  // Gamma
  std::vector<Prop> sucs_1 = seqs[0].getSuccedents(); // A, Delta
  std::vector<Prop> sucs_2 = seqs[1].getSuccedents(); // B, Delta
  if (sucs_1.size() < 1 || sucs_2.size() < 1)
    throw std::runtime_error("Invalid AndR application");
  Prop p_1 = sucs_1.front(); // A
  Prop p_2 = sucs_2.front(); // B
  if (sucs_1.size() != sucs_2.size())
    throw std::runtime_error("Invalid AndR application");
  for (size_t i = 1; i < sucs_1.size(); i++)
    if (!sucs_1[i]->isEqual(sucs_2[i]))
      throw std::runtime_error("Invalid AndR application");
  sucs_1.front() = std::make_shared<And>(p_1, p_2); // A /\ B, Delta
  return {ants, sucs_1};                            // Gamma |- A /\ B, Delta
}

Sequent NotR::apply(
    const std::vector<Sequent> &seqs) const { // seqs := {Gamma, A |- Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid NotR application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma, A
  std::vector<Prop> sucs = seqs[0].getSuccedents();  // Delta
  if (ants.size() < 1)
    throw std::runtime_error("Invalid NotR application");
  Prop last = ants.back();                                   // A
  ants.pop_back();                                           // Gamma
  std::vector<Prop> newSucs = {std::make_shared<Not>(last)}; // not A
  newSucs.insert(newSucs.end(), sucs.begin(), sucs.end());   // not A, Delta
  return {ants, newSucs}; // Gamma |- not A, Delta
}

ForallR::ForallR(Var y, Var x) : Rule(), y(y), x(x) {}

Sequent ForallR::apply(const std::vector<Sequent> &seqs)
    const { // seqs := {Gamma |- A[y/x], Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid ForallR application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma
  std::vector<Prop> sucs = seqs[0].getSuccedents();  // A[y/x], Delta
  if (sucs.size() < 1)
    throw std::runtime_error("Invalid ForallR application");
  // eigenvariable condition: y must not appear in Gamma or Delta (rest of sucs)
  for (auto &iter : ants)
    if (iter->checkVar(y))
      throw std::runtime_error("Invalid ForallR application");
  for (size_t i = 1; i < sucs.size(); i++)
    if (sucs[i]->checkVar(y))
      throw std::runtime_error("Invalid ForallR application");
  Prop p = sucs.front();               // A[y/x]
  Prop original = replaceVar(y, x, p); // A[y/x] -> A
  sucs.front() = make_shared<Forall>(std::make_shared<Var>(x),
                                     original); // (forall x, A), Delta
  return {ants, sucs};                          // Gamma |- (forall x, A), Delta
}

ExistR::ExistR(Var t, Var x) : Rule(), t(t), x(x) {}

Sequent ExistR::apply(const std::vector<Sequent> &seqs)
    const { // seqs := {Gamma |- A[t/x], Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid ExistR application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma
  std::vector<Prop> sucs = seqs[0].getSuccedents();  // A[t/x], Delta
  if (sucs.size() < 1)
    throw std::runtime_error("Invalid ExistR application");
  Prop p = sucs.front();               // A[t/x]
  Prop original = replaceVar(t, x, p); // A[t/x] -> A
  sucs.front() = make_shared<Exist>(std::make_shared<Var>(x),
                                    original); // (exists x, A), Delta
  return {ants, sucs};                         // Gamma |- (exists x, A), Delta
}

WeakeningR::WeakeningR(Prop p) : Rule(), p(p) {}

Sequent WeakeningR::apply(
    const std::vector<Sequent> &seqs) const { // seqs := {Gamma |- Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid WeakeningR application");
  std::vector<Prop> ants = seqs[0].getAntecedents();       // Gamma
  std::vector<Prop> sucs = seqs[0].getSuccedents();        // Delta
  std::vector<Prop> newSucs = {p};                         // A
  newSucs.insert(newSucs.end(), sucs.begin(), sucs.end()); // A, Delta
  return {ants, newSucs};                                  // Gamma |- A, Delta
}

Sequent ContractionR::apply(
    const std::vector<Sequent> &seqs) const { // seqs := {Gamma |- A, A, Delta}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid ContractionR application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma
  std::vector<Prop> sucs = seqs[0].getSuccedents();  // A, A, Delta
  if (sucs.size() < 2)
    throw std::runtime_error("Invalid ContractionR application");
  if (!sucs[0]->isEqual(sucs[1]))
    throw std::runtime_error("Invalid ContractionR application");
  sucs.erase(sucs.begin() + 1); // A, Delta
  return {ants, sucs};          // Gamma |- A, Delta
}

PermutationR::PermutationR(size_t i) : Rule(), i(i) {}

Sequent PermutationR::apply(const std::vector<Sequent> &seqs)
    const { // seqs := {Gamma |- Delta, A, B, Pi}
  if (seqs.size() != 1)
    throw std::runtime_error("Invalid PermutationR application");
  std::vector<Prop> ants = seqs[0].getAntecedents(); // Gamma
  std::vector<Prop> sucs = seqs[0].getSuccedents();  // Delta, A, B, Pi
  if (sucs.size() < 2 || i + 1 >= sucs.size())
    throw std::runtime_error("Invalid PermutationR application");
  std::swap(sucs[i], sucs[i + 1]);
  return {ants, sucs}; // Gamma |- Delta, B, A, Pi
}

bool Var::checkVar(const Var &oldVar) const { return this->var == oldVar.var; }

bool Not::checkVar(const Var &oldVar) const { return prop->checkVar(oldVar); }

bool And::checkVar(const Var &oldVar) const {
  return left->checkVar(oldVar) || right->checkVar(oldVar);
}

bool Or::checkVar(const Var &oldVar) const {
  return left->checkVar(oldVar) || right->checkVar(oldVar);
}

bool Implies::checkVar(const Var &oldVar) const {
  return left->checkVar(oldVar) || right->checkVar(oldVar);
}

bool Equiv::checkVar(const Var &oldVar) const {
  return left->checkVar(oldVar) || right->checkVar(oldVar);
}

bool In::checkVar(const Var &oldVar) const {
  return left.checkVar(oldVar) || right.checkVar(oldVar);
}

bool Forall::checkVar(const Var &oldVar) const {
  return var->checkVar(oldVar) || prop->checkVar(oldVar);
}

bool Exist::checkVar(const Var &oldVar) const {
  return var->checkVar(oldVar) || prop->checkVar(oldVar);
}

bool Equal::checkVar(const Var &oldVar) const {
  return left.checkVar(oldVar) || right.checkVar(oldVar);
}

bool Head::isEqual(const Prop &) const { return false; }

bool Head::checkVar(const Var &) const { return false; }

std::string Head::toString() const {
  return name.toString() + " := " + seq->toString();
}

Head::Head(Var name, std::shared_ptr<Sequent> seq)
    : PropNode(TokenType::THEOREM), name(name), seq(seq) {}

const Prop Head::replaceVar(const Var &, const Var &, const Prop &me) const {
  return me;
}

Eval::Eval(std::function<Sequent(const std::vector<Sequent> &)> eval,
           TokenType type, size_t indentLevel)
    : PropNode(type), indentLevel(indentLevel), eval(eval) {}

bool Eval::isEqual(const Prop &other) const {
  return other->getNodeType() == this->getNodeType();
}

std::string Eval::toString() const {
  return "Eval<" + tokenTypeToString(getNodeType()) + ">";
}

const Prop Eval::replaceVar(const Var &, const Var &, const Prop &me) const {
  return me;
}

bool Eval::checkVar(const Var &) const { return false; }

Sequent Eval::apply(const std::vector<Sequent> &seq) const { return eval(seq); }

bool Var::operator==(const Var &other) const { return this->var == other.var; }
