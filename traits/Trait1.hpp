#pragma once

#include <vector>

#include "../b_plus_tree/Macros.hpp"

template <typename T>
class Trait1 {
public:
  using value_t = T;
  using container_t = std::vector <value_t>;

  class SimpleSearch {
  public:
    std::size_t operator () (const container_t& data, std::size_t count, const value_t& value) const {
      std::size_t pos = 0;
      while (pos < count and data[pos] < value) {
        pos++;
      }
      return pos;
    }
  };

  class TreePrint {
  public:
    int operator () () const {
      return TREE_PRINT;
    }
  };

  using functor_t = SimpleSearch;
  using print_t = TreePrint;
};
