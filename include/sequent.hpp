#pragma once

#include "lexer.hpp"
#include <map>
#include <memory>
#include <string>

class PropNode;
class Var;

using Prop = std::shared_ptr<const PropNode>;

const Prop replaceVar(const Var &, const Var &, const Prop &);

class PropNode {
private:
  const TokenType nodeType;

public:
  PropNode(TokenType);
  TokenType getNodeType() const;
  virtual bool isEqual(const Prop &) const = 0;
  virtual std::string toString() const = 0;
  virtual const Prop replaceVar(const Var &oldVar, const Var &newVar,
                                const Prop &) const = 0;
  virtual bool checkVar(const Var &) const = 0;
};

class Var : public PropNode {
private:
  const int var;
  static std::map<std::string, int> nameToId;
  static std::map<int, std::string> idToName;
  static int nextId;

private:
  static int registerName(const std::string &name);
  static std::string getName(int id);

public:
  Var(int var);
  Var(const std::string &);
  bool isEqual(const Prop &) const override;
  std::string toString() const override;
  int getVar() const;

  const Var replaceVar(const Var &oldVar, const Var &newVar) const;
  virtual const Prop replaceVar(const Var &oldVar, const Var &newVar,
                                const Prop &) const override;
  virtual bool checkVar(const Var &) const override;
};

class Not : public PropNode {
private:
  const Prop prop;

public:
  Not(Prop prop);
  bool isEqual(const Prop &) const override;
  std::string toString() const override;

  virtual const Prop replaceVar(const Var &oldVar, const Var &newVar,
                                const Prop &) const override;
  virtual bool checkVar(const Var &) const override;
};

class And : public PropNode {
private:
  const Prop left;
  const Prop right;

public:
  And(Prop left, Prop right);
  bool isEqual(const Prop &) const override;
  std::string toString() const override;

  Prop getLeft() const;
  Prop getRight() const;

  virtual const Prop replaceVar(const Var &oldVar, const Var &newVar,
                                const Prop &) const override;
  virtual bool checkVar(const Var &) const override;
};

class Or : public PropNode {
private:
  const Prop left;
  const Prop right;

public:
  Or(Prop left, Prop right);
  bool isEqual(const Prop &) const override;
  std::string toString() const override;

  Prop getLeft() const;
  Prop getRight() const;

  virtual const Prop replaceVar(const Var &oldVar, const Var &newVar,
                                const Prop &) const override;
  virtual bool checkVar(const Var &) const override;
};

class Implies : public PropNode {
private:
  const Prop left;
  const Prop right;

public:
  Implies(Prop left, Prop right);
  bool isEqual(const Prop &) const override;
  std::string toString() const override;

  Prop getLeft() const;
  Prop getRight() const;

  virtual const Prop replaceVar(const Var &oldVar, const Var &newVar,
                                const Prop &) const override;
  virtual bool checkVar(const Var &) const override;
};

class Equiv : public PropNode {
private:
  const Prop left;
  const Prop right;

public:
  Equiv(Prop left, Prop right);
  bool isEqual(const Prop &) const override;
  std::string toString() const override;

  virtual const Prop replaceVar(const Var &oldVar, const Var &newVar,
                                const Prop &) const override;
  virtual bool checkVar(const Var &) const override;
};

class In : public PropNode {
private:
  const Var left;
  const Var right;

public:
  In(Var left, Var right);
  bool isEqual(const Prop &) const override;
  std::string toString() const override;

  virtual const Prop replaceVar(const Var &oldVar, const Var &newVar,
                                const Prop &) const override;
  virtual bool checkVar(const Var &) const override;
};

class Forall : public PropNode {
private:
  const Prop var;
  const Prop prop;

public:
  Forall(Prop var, Prop prop);
  bool isEqual(const Prop &) const override;
  std::string toString() const override;

  virtual const Prop replaceVar(const Var &oldVar, const Var &newVar,
                                const Prop &) const override;
  virtual bool checkVar(const Var &) const override;
};

class Exist : public PropNode {
private:
  const Prop var;
  const Prop prop;

public:
  Exist(Prop var, Prop prop);
  bool isEqual(const Prop &) const override;
  std::string toString() const override;

  virtual const Prop replaceVar(const Var &oldVar, const Var &newVar,
                                const Prop &) const override;
  virtual bool checkVar(const Var &) const override;
};

class Equal : public PropNode {
private:
  const Var left;
  const Var right;

public:
  Equal(Var left, Var right);
  bool isEqual(const Prop &) const override;
  std::string toString() const override;

  virtual const Prop replaceVar(const Var &oldVar, const Var &newVar,
                                const Prop &) const override;
  virtual bool checkVar(const Var &) const override;
};

class Sequent {
private:
  const std::vector<Prop> antecedents;
  const std::vector<Prop> succedents;

public:
  Sequent(const std::vector<Prop> &antecedents,
          const std::vector<Prop> &succedents);
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
  virtual Sequent apply(const std::vector<Sequent> &seq) const = 0;
};

class AndL_1 : public Rule {
private:
  const Prop p;

public:
  AndL_1(Prop p);
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class AndL_2 : public Rule {
private:
  const Prop p;

public:
  AndL_2(Prop p);
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class OrL : public Rule {
public:
  OrL() = default;
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class ImpliesL : public Rule {
public:
  ImpliesL() = default;
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class NotL : public Rule {
public:
  NotL() = default;
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class ForallL : public Rule {
private:
  const Var t;
  const Var x;

public:
  ForallL(Var t, Var x);
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class ExistL : public Rule {
private:
  const Var y;
  const Var x;

public:
  ExistL(Var y, Var x);
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class Identity : public Rule {
private:
  const Prop p;

public:
  Identity(Prop p);
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class Cut : public Rule {
public:
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class WeakeningL : public Rule {
private:
  const Prop p;

public:
  WeakeningL(Prop p);
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class ContractionL : public Rule {
public:
  ContractionL() = default;
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class PermutationL : public Rule {
private:
  const size_t i;

public:
  PermutationL(size_t i);
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class OrR_1 : public Rule {
private:
  const Prop p;

public:
  OrR_1(Prop p);
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class OrR_2 : public Rule {
private:
  const Prop p;

public:
  OrR_2(Prop p);
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class AndR : public Rule {
public:
  AndR() = default;
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class ImpliesR : public Rule {
public:
  ImpliesR() = default;
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class NotR : public Rule {
public:
  NotR() = default;
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class ForallR : public Rule {
private:
  const Var y;
  const Var x;

public:
  ForallR(Var y, Var x);
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class ExistR : public Rule {
private:
  const Var t;
  const Var x;

public:
  ExistR(Var t, Var x);
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class WeakeningR : public Rule {
private:
  const Prop p;

public:
  WeakeningR(Prop p);
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class ContractionR : public Rule {
public:
  ContractionR() = default;
  Sequent apply(const std::vector<Sequent> &seq) const override;
};

class PermutationR : public Rule {
private:
  const size_t i;

public:
  PermutationR(size_t i);
  Sequent apply(const std::vector<Sequent> &seq) const override;
};
