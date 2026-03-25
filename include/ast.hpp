#pragma once

#include "lexer.hpp"
#include "sequent.hpp"
#include <memory>
#include <string>
#include <vector>

// Theorem Block
struct TheoremNode {
  std::string name;
  std::shared_ptr<Sequent> proposition;
  std::vector<Token> proofTokens; // Raw tokens between `:=` and `qed`

  std::string toString() const;
};
