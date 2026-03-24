#pragma once

#include "lexer.hpp"
#include <memory>

class PropTree {
private:
  TokenType nodeType;

public:
  PropTree(TokenType);
  TokenType getNodeType() const;
  virtual bool isEqual(PropTree &) const = 0;
};

class PropVar : public PropTree {
public:
  int var;

public:
  PropVar(int var);
  bool isEqual(PropTree &) const override;
};

class Not : public PropTree {
private:
  std::shared_ptr<PropTree> prop;

public:
  Not(std::shared_ptr<PropTree> prop);
  bool isEqual(PropTree &) const override;
};

class Sequent {
private:
  std::vector<int> antecedents;
  std::vector<int> succedents;

public:
  Sequent(std::vector<int> antecedents, std::vector<int> succedents);
  std::string toString() const;

  friend class AndL_1;
};

class Rule {
public:
  virtual Sequent apply(std::vector<Sequent> sequents) = 0;
};

class AndL_1 : public Rule {
private:
  int num1;
  int num2;

public:
  AndL_1(int num1, int num2);
  Sequent apply(std::vector<Sequent> sequents) override;
};