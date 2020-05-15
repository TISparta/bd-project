#pragma once

#include <vector>
#include "Macros.hpp"

template <typename Trait, int BTREE_ORDER>
class BPlusNode {
public:
  using value_t = typename Trait::value_t;
  using key_t = typename Trait::key_t;
  using key_g = typename Trait::key_g;
  using address = typename Trait::address;
  using node = BPlusNode <Trait, BTREE_ORDER>;

  std::size_t count;
  value_t data[BTREE_ORDER + 1];
  address children[BTREE_ORDER + 2];
  address right;
  bool is_leaf;

  BPlusNode () {
    count = 0;
    std::fill(children, children + BTREE_ORDER + 2, NIL);
    right = NIL;
    is_leaf = true;
  }

  ~BPlusNode () {}

  std::size_t search (const value_t& value) const {
    key_g get_key;
    if (count == 0 or get_key(data[0]) >= get_key(value)) {
      return 0;
    }
    std::size_t low = 0;
    std::size_t high = count - 1;
    while (low != high) {
      std::size_t mid = (low + high + 1) >> 1;
      if (get_key(data[mid]) < get_key(value)) {
        low = mid;
      } else {
        high = mid - 1;
      }
    }
    return low + 1;
  }

  void insert (std::size_t pos, const value_t& value) {
    std::size_t cur = count;
    while (cur > pos) {
      data[cur] = std::move(data[cur - 1]);
      children[cur + 1] = children[cur];
      cur--;
    }
    data[cur] = value;
    children[cur + 1] = children[cur];
    count++;
  }

  inline bool is_overflow () {
    return count > BTREE_ORDER;
  }

  inline bool is_underflow () {
    return count < BTREE_ORDER / 2;
  }

};
