#pragma once

#include "sequent.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

template <> struct std::hash<const Var> {
  std::size_t operator()(const Var &k) const;
};

class Turnstile {
private:
  std::shared_ptr<const Head> head;
  bool isProvable;
  static std::vector<std::vector<Sequent>> proof;
  static std::unordered_map<const Var, const Sequent> theorems;

public:
  Turnstile(const std::shared_ptr<const Head> &head);

  void interpret(const std::string &);
  static bool isTheoremName(const Var &var);
  static const Sequent getTheorem(const Var &var);
  static void addTheorem(const Var &var, const Sequent &sequent);
};