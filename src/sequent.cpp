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

PropVar::PropVar(int var) : PropTree(TokenType::IDENTIFIER), var(var) {}

bool PropVar::isEqual(PropTree &other) const {
  if (other.getNodeType() != TokenType::IDENTIFIER)
    return false;
  return static_cast<PropVar &>(other).var == var;
}

Not::Not(std::shared_ptr<PropTree> prop)
    : PropTree(TokenType::NOT), prop(prop) {}

bool Not::isEqual(PropTree &other) const {
  if (other.getNodeType() != TokenType::NOT)
    return false;
  return static_cast<Not &>(other).prop->isEqual(*prop);
}
