#pragma once

#include <vector>

#include "BPlusNode.hpp"
#include "Iterator.hpp"
#include "Macros.hpp"

template <typename Trait, int BTREE_ORDER>
class BPlusTree {
public:
  using value_t = typename Trait::value_t;
  using container_t = typename Trait::container_t;
  using functor_t = typename Trait::functor_t;
  using print_t = typename Trait::print_t;
  using node = BPlusNode <Trait, BTREE_ORDER>;
  using iterator_t = Iterator <Trait, BTREE_ORDER>;

  BPlusTree () {
    root = new node();
  }

  bool insert (const value_t& value) {
    if (find(value)) {
      return false;
    }
    int state = insert(root, value);
    if (state == node::BT_OVERFLOW) {
      split(root, value, true);
    }
    return true;
  }

  bool find (const value_t& value) const {
    iterator_t it = gteq(root, value);
    if (it != end()) {
      return not it.fail() and (*it == value);
    }
    return false;
  }

  iterator_t gteq (const value_t value) const {
    iterator_t it = gteq(root, value);
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

  iterator_t end () const {
    return iterator_t(nullptr, 0);
  }

private:
  node* root;
  functor_t search;
  print_t print;

  int insert (node* pnode, const value_t& value) {
    std::size_t pos = search(pnode->data, pnode->count, value);
    if (pnode->children[pos]) {
      int state = insert(pnode->children[pos], value);
      if (state == node::BT_OVERFLOW) {
        split(pnode, pos);
      }
    } else {
      pnode->insert(pos, value);
    }
    return pnode->is_overflow() ? node::BT_OVERFLOW : node::NORMAL;
  }

  void split (node* pnode, std::size_t pos, bool is_root = false) {
    node* poverflow = is_root ? pnode : pnode->children[pos];
    value_t vsplit = poverflow->data[BTREE_ORDER / 2];
    // left child
    node* child_1 = is_root ? new node() : poverflow;
    std::move(std::begin(poverflow->data), 
              std::begin(poverflow->data) + BTREE_ORDER / 2, 
              std::begin(child_1->data));
    std::move(std::begin(poverflow->children), 
              std::begin(poverflow->children) + BTREE_ORDER / 2 + 1,
              std::begin(child_1->children));
    child_1->count = BTREE_ORDER / 2;
    child_1->is_leaf = poverflow->is_leaf;
    // right child
    node* child_2 = new node();
    std::move(std::begin(poverflow->data) + BTREE_ORDER / 2 + (not poverflow->is_leaf), 
              std::end(poverflow->data), 
              std::begin(child_2->data));
    std::move(std::begin(poverflow->children) + BTREE_ORDER / 2 + 1, 
              std::end(poverflow->children), 
              std::begin(child_2->children));
    child_2->count = BTREE_ORDER - BTREE_ORDER / 2 + (poverflow->is_leaf);
    child_2->is_leaf = poverflow->is_leaf;
    // update node
    pnode->is_leaf = false;
    child_2->right = poverflow->right;
    child_1->right = child_2;
    if (is_root) {
      pnode->data[0] = vsplit;
      pnode->children[0] = child_1;
      pnode->children[1] = child_2;
      pnode->count = 1;
    } else {
      pnode->insert(pos, vsplit);
      pnode->children[pos] = child_1;
      pnode->children[pos + 1] = child_2;
      // pnode->childe[pos]->right = pnode->children[pos + 1];
    }
  }

  iterator_t gteq (node* pnode, const value_t& value) const {
    if (not pnode) {
      return iterator_t(nullptr, 0);
    }
    std::size_t pos = search(pnode->data, pnode->count, value);
    if (pnode->is_leaf) {
      return iterator_t(pnode, pos);
    }
    if (pos < pnode->count and pnode->data[pos] == value) {
      pos++;
    }
    return gteq(pnode->children[pos], value);
  }

  void print_tree (std::ostream& out) const {
    if (print() == TREE_PRINT) {
      print_tree(root, 0, out);
    }
  }

  void print_tree (node* pnode, int level, std::ostream& out) const {
    int pos = 0;
    for (pos = pnode->count - 1; pos >= 0; pos--) {
      if (pnode->children[pos + 1]) {
        print_tree(pnode->children[pos + 1], level + 1, out);
      }
      out << std::string(level * 2, ' ') << pnode->data[pos] << '\n';
    }
    if (pnode->children[pos + 1]) {
      print_tree(pnode->children[pos + 1], level + 1, out);
    }
  }

  void print_inorder (std::ostream& out) const {
    if (print() == INORDER_PRINT) {
      print_inorder(root, out);
      out << '\n';  
    }
  }

  void print_inorder (node* pnode, std::ostream& out) const {
    std::size_t pos = 0;
    for (pos = 0; pos < pnode->count; pos++) {
      if (pnode->children[pos]) {
        print_inorder(pnode->children[pos], out);
      }
      out << pnode->data[pos] << ' ';
    }
    if (pnode->children[pos]) {
      print_inorder(pnode->children[pos], out);
    }
  }

};
