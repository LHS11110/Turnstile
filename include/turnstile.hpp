#pragma once

#include "sequent.hpp"
#include <unordered_map>
#include <vector>

template <> struct std::hash<Var> {
  std::size_t operator()(const Var &k) const;
};

class Turnstile {
private:
  static std::vector<std::vector<Sequent>> proof;
  static std::unordered_map<Var, const Prop> theorems;

public:
  static const std::unordered_map<Var, const Prop>::iterator end;
  Turnstile() = default;
};