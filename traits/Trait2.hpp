#pragma once

#include <vector>
#include <iostream>

#include "../b_plus_tree/Macros.hpp"

template <typename T>
class Trait2 {
public:
  using value_t = T;
  using address = long;
  using key_t = T;

  class InOrderPrint {
  public:
    int operator () () const {
      return INORDER_PRINT;
    }
  };

  class Key {
  public:
    key_t operator () (value_t value) const {
      return value;
    }
  };

  using print_t = InOrderPrint;
  using key_g = Key;
};
