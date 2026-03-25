#pragma once

#include "lexer.hpp"
#include <map>
#include <memory>
#include <string>

class PropTree;

using Prop = std::shared_ptr<const PropTree>;

class PropTree {
private:
  const TokenType nodeType;

public:
  PropTree(TokenType);
  TokenType getNodeType() const;
  virtual bool isEqual(const Prop) const = 0;
  virtual std::string toString() const = 0;
};

class Var : public PropTree {
private:
  const int var;
  static std::map<std::string, int> nameToId;
  static std::map<int, std::string> idToName;
  static int nextId;

public:
  Var(int var);
  bool isEqual(const Prop) const override;
  std::string toString() const override;
  int getVar() const;

  static int registerName(const std::string &name);
  static std::string getName(int id);
};

class Not : public PropTree {
private:
  const Prop prop;

public:
  Not(Prop prop);
  bool isEqual(const Prop) const override;
  std::string toString() const override;
};

class And : public PropTree {
private:
  const Prop left;
  const Prop right;

public:
  And(Prop left, Prop right);
  bool isEqual(const Prop) const override;
  std::string toString() const override;

  Prop getLeft() const;
  Prop getRight() const;
};

class Or : public PropTree {
private:
  const Prop left;
  const Prop right;

public:
  Or(Prop left, Prop right);
  bool isEqual(const Prop) const override;
  std::string toString() const override;

  Prop getLeft() const;
  Prop getRight() const;
};

class Implies : public PropTree {
private:
  const Prop left;
  const Prop right;

public:
  Implies(Prop left, Prop right);
  bool isEqual(const Prop) const override;
  std::string toString() const override;

  Prop getLeft() const;
  Prop getRight() const;
};

class Equiv : public PropTree {
private:
  const Prop left;
  const Prop right;

public:
  Equiv(Prop left, Prop right);
  bool isEqual(const Prop) const override;
  std::string toString() const override;
};

class In : public PropTree {
private:
  const Var left;
  const Var right;

public:
  In(Var left, Var right);
  bool isEqual(const Prop) const override;
  std::string toString() const override;
};

class Forall : public PropTree {
private:
  const Prop var;
  const Prop prop;

public:
  Forall(Prop var, Prop prop);
  bool isEqual(const Prop) const override;
  std::string toString() const override;
};

class Exist : public PropTree {
private:
  const Prop var;
  const Prop prop;

public:
  Exist(Prop var, Prop prop);
  bool isEqual(const Prop) const override;
  std::string toString() const override;
};

class Equal : public PropTree {
private:
  const Var left;
  const Var right;

public:
  Equal(Var left, Var right);
  bool isEqual(const Prop) const override;
  std::string toString() const override;
};

class Sequent {
private:
  const std::vector<Prop> antecedents;
  const std::vector<Prop> succedents;

public:
  Sequent(std::vector<Prop> antecedents, std::vector<Prop> succedents);
  std::string toString() const;

  const std::vector<Prop> &getAntecedents() const;
  const std::vector<Prop> &getSuccedents() const;
  Sequent addAntecedent(Prop p) const;
  Sequent addSuccedent(Prop p) const;
  Sequent removeAntecedent(size_t index) const;
  Sequent removeSuccedent(size_t index) const;
};

class Rule {
public:
  virtual ~Rule() = default;
  virtual Sequent apply(const Sequent &seq) const = 0;
};

class AndL_1 : public Rule {
private:
  const Prop p;

public:
  AndL_1(Prop p);
  Sequent apply(const Sequent &seq) const override;
};