#include <memory>

#include "BPlusNode.hpp"
#include "Manager.hpp"

template <typename Trait, int BTREE_ORDER>
class Iterator {
public:
  using value_t = typename Trait::value_t;
  using address = typename Trait::address;
  using node = BPlusNode <Trait, BTREE_ORDER>;
  using iterator_t = Iterator <Trait, BTREE_ORDER>;

  Iterator () {}

  Iterator (address current_address, std::size_t pos, std::shared_ptr <Manager> manager):
    current_address(current_address), pos(pos), manager(manager) {
    if (current_address != NIL) {
      manager->read <node> (current_address, pnode);
    }
  }

  bool fail () const {
    return (pos >= pnode.count);
  }

  value_t operator * () const {
    return pnode.data[pos];
  }

  bool operator != (iterator_t other) const {
    return not (current_address == other.current_address and pos == other.pos);
  }
  
  iterator_t operator = (iterator_t other) {
    current_address = other.current_address;
    pos = other.pos;
    pnode = other.pnode;
    manager = other.manager;
  }

  iterator_t operator ++ () {
    if (pos + 1 < pnode.count) {
      pos += 1;
    } else {
      current_address = pnode.right;
      manager->read <node> (current_address, pnode);
      pos = 0;
    }
    return (*this);
  }

protected:
  address current_address = NIL;
  std::size_t pos;
  node pnode;
  std::shared_ptr <Manager> manager;
};
