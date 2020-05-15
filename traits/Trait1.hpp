#pragma once

#include <vector>

#include "../b_plus_tree/Macros.hpp"

template <typename T>
class Trait1 {
public:
  using value_t = T;
  using key_t = T;
  using address = int;

  class TreePrint {
  public:
    int operator () () const {
      return TREE_PRINT;
    }
  };

  class Key {
  public:
    key_t operator () (value_t value) const {
      return value;
    }
  };

  using print_t = TreePrint;
  using key_g = Key;
};
