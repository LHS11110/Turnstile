#pragma once

#include "lexer.hpp"
#include <memory>
#include <string>

class PropTree;

using Prop = std::shared_ptr<PropTree>;

class PropTree {
private:
  TokenType nodeType;

public:
  PropTree(TokenType);
  TokenType getNodeType() const;
  virtual bool isEqual(std::shared_ptr<PropTree>) const = 0;
  virtual std::string toString() const = 0;
};

class Var : public PropTree {
private:
  int var;

public:
  Var(int var);
  bool isEqual(std::shared_ptr<PropTree>) const override;
  std::string toString() const override;
  int getVar() const;
};

class Not : public PropTree {
private:
  std::shared_ptr<PropTree> prop;

public:
  Not(std::shared_ptr<PropTree> prop);
  bool isEqual(std::shared_ptr<PropTree>) const override;
  std::string toString() const override;
};

class And : public PropTree {
private:
  std::shared_ptr<PropTree> left;
  std::shared_ptr<PropTree> right;

public:
  And(std::shared_ptr<PropTree> left, std::shared_ptr<PropTree> right);
  bool isEqual(std::shared_ptr<PropTree>) const override;
  std::string toString() const override;
};

class Or : public PropTree {
private:
  std::shared_ptr<PropTree> left;
  std::shared_ptr<PropTree> right;

public:
  Or(std::shared_ptr<PropTree> left, std::shared_ptr<PropTree> right);
  bool isEqual(std::shared_ptr<PropTree>) const override;
  std::string toString() const override;
};

class Implies : public PropTree {
private:
  std::shared_ptr<PropTree> left;
  std::shared_ptr<PropTree> right;

public:
  Implies(std::shared_ptr<PropTree> left, std::shared_ptr<PropTree> right);
  bool isEqual(std::shared_ptr<PropTree>) const override;
  std::string toString() const override;
};

class Equiv : public PropTree {
private:
  std::shared_ptr<PropTree> left;
  std::shared_ptr<PropTree> right;

public:
  Equiv(std::shared_ptr<PropTree> left, std::shared_ptr<PropTree> right);
  bool isEqual(std::shared_ptr<PropTree>) const override;
  std::string toString() const override;
};

class In : public PropTree {
private:
  Var left;
  Var right;

public:
  In(Var left, Var right);
  bool isEqual(std::shared_ptr<PropTree>) const override;
  std::string toString() const override;
};

class Forall : public PropTree {
private:
  std::shared_ptr<PropTree> var;
  std::shared_ptr<PropTree> prop;

public:
  Forall(std::shared_ptr<PropTree> var, std::shared_ptr<PropTree> prop);
  bool isEqual(std::shared_ptr<PropTree>) const override;
  std::string toString() const override;
};

class Exist : public PropTree {
private:
  std::shared_ptr<PropTree> var;
  std::shared_ptr<PropTree> prop;

public:
  Exist(std::shared_ptr<PropTree> var, std::shared_ptr<PropTree> prop);
  bool isEqual(std::shared_ptr<PropTree>) const override;
  std::string toString() const override;
};

class Equal : public PropTree {
private:
  Var left;
  Var right;

public:
  Equal(Var left, Var right);
  bool isEqual(std::shared_ptr<PropTree>) const override;
  std::string toString() const override;
};

class Sequent {
private:
  std::vector<Prop> antecedents;
  std::vector<Prop> succedents;

public:
  Sequent(std::vector<Prop> antecedents, std::vector<Prop> succedents);
  std::string toString() const;

  friend class AndL_1;
};

class Rule {
public:
  virtual Sequent apply(std::vector<Sequent> sequents) const = 0;
};

class AndL_1 : public Rule {
public:
  AndL_1();
  Sequent apply(std::vector<Sequent> sequents) const override;
};