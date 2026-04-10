#include "turnstile.hpp"
#include <functional>

std::vector<std::vector<Sequent>> Turnstile::proof = {};
std::unordered_map<Var, const Prop> Turnstile::theorems = {};
const std::unordered_map<Var, const Prop>::iterator Turnstile::end =
    theorems.end();

std::size_t std::hash<Var>::operator()(const Var &k) const {
  return std::hash<int>()(k.getVar());
}

int main() { return 0; }
