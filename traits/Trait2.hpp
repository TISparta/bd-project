#pragma once

#include <vector>
#include <iostream>

#include "../b_plus_tree/Macros.hpp"

template <typename T>
class Trait2 {
public:
  using value_t = T;
  using container_t = std::vector <value_t>;

  class BinarySearch {
  public:
    std::size_t operator () (const container_t& data, std::size_t count, const value_t& value) const {
      if (count == 0 or data[0] >= value) {
        return 0;
      }
      std::size_t low = 0;
      std::size_t high = count - 1;
      while (low != high) {
        std::size_t mid = (low + high + 1) >> 1;
        if (data[mid] < value) {
          low = mid;
        } else {
          high = mid - 1;
        }
      }
      return low + 1;
    }
  };

  class InOrderPrint {
  public:
    int operator () () const {
      return INORDER_PRINT;
    }
  };

  using functor_t = BinarySearch;
  using print_t = InOrderPrint;
};
