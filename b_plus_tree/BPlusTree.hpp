#pragma once

#include <vector>
#include <memory>

#include "BPlusNode.hpp"
#include "Iterator.hpp"
#include "Macros.hpp"
#include "Manager.hpp"

template <typename Trait, int BTREE_ORDER>
class BPlusTree {
public:
  using value_t = typename Trait::value_t;
  using key_t = typename Trait::key_t;
  using key_g = typename Trait::key_g;
  using print_t = typename Trait::print_t;
  using address = typename Trait::address;
  using node = BPlusNode <Trait, BTREE_ORDER>;
  using iterator_t = Iterator <Trait, BTREE_ORDER>;

  struct Header {
    address root_address;
    int n_blocks;
    value_t head;
  } header;

  BPlusTree (std::string header_path, std::string tree_path, bool trunc = true):
    manager_header(std::make_shared <Manager> (header_path, trunc)),
    manager_data(std::make_shared <Manager> (tree_path, trunc)) {
    if (manager_header->is_empty()) {
      header.n_blocks = 1;
      header.root_address = 0;
      manager_header->write <Header> (0, header);
      manager_data->write <node> (0, node());
      valid_head = false;
    } else {
      manager_header->read <Header> (0, header);
      head = header.head;
      valid_head = true;
    }
  }

  bool insert (const value_t& value) {
    if (find(value)) {
      return false;
    }
    int state = insert(header.root_address, value);
    if (state == state::BT_OVERFLOW) {
      split(header.root_address, -1, true);
    }
    if ((not valid_head) or (get_key(head) > get_key(value))) {
      head = value;
    }
    valid_head = true;
    return true;
  }

  bool find (const value_t& value) const {
    iterator_t it = gteq(value);
    if (it != end()) {
      return not it.fail() and (get_key(*it) == get_key(value));
    }
    return false;
  }

  iterator_t gteq (const value_t value) const {
    iterator_t it = gteq(header.root_address, value);
    if (it != end()) {
      if (it.fail()) {
        return end();
      }
      return it;
    }
    return end();
  };

  friend std::ostream& operator << (std::ostream& out, BPlusTree tree) {
    tree.print_tree(out);
    tree.print_inorder(out);
    return out;
  }

  iterator_t begin () const {
    if (not valid_head) {
      return iterator_t(NIL, 0, manager_data);
    }
    return gteq(header.root_address, head);
  }

  iterator_t end () const {
    return iterator_t(NIL, 0, manager_data);
  }

  int get_n_reads () const {
    return manager_data->get_n_reads() + manager_header->get_n_reads();
  }

  int get_n_writes () const {
    return manager_data->get_n_writes() + manager_header->get_n_writes();
  }

private:
  bool valid_head;
  value_t head;
  print_t print;
  key_g get_key;
  std::shared_ptr <Manager> manager_header;
  std::shared_ptr <Manager> manager_data;

  enum state {
    BT_OVERFLOW,
    BT_UNDERFLOW,
    NORMAL
  };

  int insert (address node_address, const value_t& value) {
    node pnode;
    manager_data->read <node> (node_address, pnode);
    std::size_t pos = pnode.search(value);
    if (pnode.children[pos] != NIL) {
      int state = insert(pnode.children[pos], value);
      if (state == state::BT_OVERFLOW) {
        split(node_address, pos);
      }
    } else {
      pnode.insert(pos, value);
      manager_data->write <node> (node_address, pnode);
    }
    return pnode.is_overflow() ? state::BT_OVERFLOW : state::NORMAL;
  }

  void split (address node_address, std::size_t pos, bool is_root = false) {
    node pnode;
    manager_data->read <node> (node_address, pnode);
    node poverflow;
    if (is_root) {
      poverflow = pnode;
    } else {
      manager_data->read <node> (pnode.children[pos], poverflow);
    }
    value_t vsplit = poverflow.data[BTREE_ORDER / 2];
    // left child
    node child_1 = is_root ? node() : poverflow;
    address child_1_address = is_root ? header.n_blocks * sizeof(node) : pnode.children[pos];
    if (is_root) {
      header.n_blocks += 1;
    }
    std::move(std::begin(poverflow.data), 
              std::begin(poverflow.data) + BTREE_ORDER / 2, 
              std::begin(child_1.data));
    std::move(std::begin(poverflow.children), 
              std::begin(poverflow.children) + BTREE_ORDER / 2 + 1,
              std::begin(child_1.children));
    child_1.count = BTREE_ORDER / 2;
    child_1.is_leaf = poverflow.is_leaf;
    // right child
    node child_2 = node();
    address child_2_address = header.n_blocks * sizeof(node);
    header.n_blocks += 1;
    std::move(std::begin(poverflow.data) + BTREE_ORDER / 2 + (not poverflow.is_leaf), 
              std::end(poverflow.data), 
              std::begin(child_2.data));
    std::move(std::begin(poverflow.children) + BTREE_ORDER / 2 + 1, 
              std::end(poverflow.children), 
              std::begin(child_2.children));
    child_2.count = BTREE_ORDER - BTREE_ORDER / 2 + (poverflow.is_leaf);
    child_2.is_leaf = poverflow.is_leaf;
    // update node
    pnode.is_leaf = false;
    child_2.right = poverflow.right;
    child_1.right = child_2_address;
    if (is_root) {
      pnode.data[0] = vsplit;
      pnode.children[0] = child_1_address;
      pnode.children[1] = child_2_address;
      pnode.count = 1;
    } else {
      pnode.insert(pos, vsplit);
      pnode.children[pos] = child_1_address;
      pnode.children[pos + 1] = child_2_address;
    }
    manager_data->write <node> (node_address, pnode);
    manager_data->write <node> (child_1_address, child_1);
    manager_data->write <node> (child_2_address, child_2);
    manager_header->write <Header> (0, header);
  }

  iterator_t gteq (address node_address, const value_t& value) const {
    if (node_address == NIL) {
      return iterator_t(node_address, 0, manager_data);
    }
    node pnode;
    manager_data->read <node> (node_address, pnode);
    std::size_t pos = pnode.search(value);
    if (pnode.is_leaf) {
      return iterator_t(node_address, pos, manager_data);
    }
    if (pos < pnode.count and get_key(pnode.data[pos]) == get_key(value)) {
      pos++;
    }
    return gteq(pnode.children[pos], value);
  }

  void print_tree (std::ostream& out) const {
    if (print() == TREE_PRINT) {
      print_tree(header.root_address, 0, out);
    }
  }

  void print_tree (address node_address, int level, std::ostream& out) const {
    node pnode;
    manager_data->read <node> (node_address, pnode);
    int pos = 0;
    for (pos = pnode.count - 1; pos >= 0; pos--) {
      if (pnode.children[pos + 1] != NIL) {
        print_tree(pnode.children[pos + 1], level + 1, out);
      }
      out << std::string(level * 2, ' ') << pnode.data[pos] << '\n';
    }
    if (pnode.children[pos + 1] != NIL) {
      print_tree(pnode.children[pos + 1], level + 1, out);
    }
  }

  void print_inorder (std::ostream& out) const {
    if (print() == INORDER_PRINT) {
      print_inorder(header.root_address, out);
      out << '\n';  
    }
  }

  void print_inorder (address node_address, std::ostream& out) const {
    node pnode;
    manager_data->read <node> (node_address, pnode);
    std::size_t pos = 0;
    for (pos = 0; pos < pnode.count; pos++) {
      if (pnode.children[pos] != NIL) {
        print_inorder(pnode.children[pos], out);
      }
      out << pnode.data[pos] << ' ';
    }
    if (pnode.children[pos] != NIL) {
      print_inorder(pnode.children[pos], out);
    }
  }

};
