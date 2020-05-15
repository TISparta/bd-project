#include "BPlusNode.hpp"

template <typename Trait, int BTREE_ORDER>
class Iterator {
public:
  using value_t = typename Trait::value_t;
  using node = BPlusNode <Trait, BTREE_ORDER>;
  using iterator_t = Iterator <Trait, BTREE_ORDER>;

  Iterator () {}

  Iterator (node* current_node, std::size_t pos):
    current_node(current_node), pos(pos) {}

  bool fail () const {
    return (pos >= current_node->count);
  }

  value_t operator * () const {
    return current_node->data[pos];
  }

  bool operator != (iterator_t other) const {
    if (current_node != other.current_node) {
      return true;
    }
    return (pos != other.pos);
  }
  
  iterator_t operator = (iterator_t other) {
    current_node = other.current_node;
    pos = other.pos;
  }

  iterator_t operator ++ () {
    if (pos + 1 < current_node->count) {
      pos += 1;
    } else {
      current_node = current_node->right;
      pos = 0;
    }
    return (*this);
  }

protected:
  node* current_node = nullptr;
  std::size_t pos;
};
