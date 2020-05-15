#include <set>

#include <gtest/gtest.h>

#include "../utility/Random.hpp"
#include "../traits/Trait1.hpp"
#include "../traits/Trait2.hpp"
#include "../b_plus_tree/BPlusTree.hpp"

const int MIN_VAL = -1000;
const int MAX_VAL = +1000;

const std::string header_path = "../persistency/header_test.bin";
const std::string data_path = "../persistency/data_test.bin";

TEST (Trait_1, int_insert_and_find) {
  using value_t = int;
  using trait_t = Trait1 <value_t>;
  const int order = 33;
  BPlusTree <trait_t, order> tree(header_path, data_path);
  const int n_elements = 6000;
  std::vector <value_t> values(n_elements);
  std::set <value_t> used;
  for (auto& value: values) {
    value = Random::generate_value <value_t> (MIN_VAL, MAX_VAL);
    if (used.count(value)) {
      EXPECT_FALSE(tree.insert(value));
    } else {
      EXPECT_TRUE(tree.insert(value));
    }
    used.insert(value);
  }
  for (auto& value: values) {
    EXPECT_TRUE(tree.find(value));
  }
}

TEST (Trait_1, long_long_insert_and_find) {
  using value_t = long long;
  using trait_t = Trait1 <value_t>;
  const int order = 42;
  BPlusTree <trait_t, order> tree(header_path, data_path);
  const int n_elements = 1000;
  std::vector <value_t> values(n_elements);
  std::set <value_t> used;
  for (auto& value: values) {
    value = Random::generate_value <value_t> (MIN_VAL, MAX_VAL);
    if (used.count(value)) {
      EXPECT_FALSE(tree.insert(value));
    } else {
      EXPECT_TRUE(tree.insert(value));
    }
    used.insert(value);
  }
  for (auto& value: values) {
    EXPECT_TRUE(tree.find(value));
  }
}

TEST (Trait_2, int_insert_and_find) {
  using value_t = int;
  using trait_t = Trait2 <value_t>;
  const int order = 33;
  BPlusTree <trait_t, order> tree(header_path, data_path);
  const int n_elements = 1000;
  std::vector <value_t> values(n_elements);
  std::set <value_t> used;
  for (auto& value: values) {
    value = Random::generate_value <value_t> (MIN_VAL, MAX_VAL);
    if (used.count(value)) {
      EXPECT_FALSE(tree.insert(value));
    } else {
      EXPECT_TRUE(tree.insert(value));
    }
    used.insert(value);
  }
  for (auto& value: values) {
    EXPECT_TRUE(tree.find(value));
  }
}

TEST (print, trait_1) {
  using value_t = int;
  using trait_t = Trait1 <value_t>;
  const int order = 4;
  BPlusTree <trait_t, order> tree(header_path, data_path);
  const int n_elements = 20;
  for (int value = 1; value <= n_elements; value++) {
    tree.insert(value);
  }
  std::cout << tree;
}

TEST (print, trait_2) {
  using value_t = int;
  using trait_t = Trait2 <value_t>;
  const int order = 4;
  BPlusTree <trait_t, order> tree(header_path, data_path);
  const int n_elements = 20;
  for (int value = 1; value <= n_elements; value++) {
    tree.insert(value);
  }
  std::cout << tree;
}

TEST (range_search1, trait_1) {
  using value_t = int;
  using trait_t = Trait2 <value_t>;
  const int order = 4;
  BPlusTree <trait_t, order> tree(header_path, data_path);
  const int n_elements = 20;
  for (int value = 1; value <= n_elements; value++) {
    tree.insert(value);
  }
  int cnt = 0;
  for (auto it = tree.gteq(3); it != tree.end(); ++it) {
    std::cout << *it << ' ';
  }
  std::cout << std::endl;
}

TEST (range_search2, trait_1) {
  using value_t = int;
  using trait_t = Trait2 <value_t>;
  const int order = 4;
  BPlusTree <trait_t, order> tree(header_path, data_path);
  const int n_elements = 20;
  for (int value = 1; value <= n_elements; value++) {
    tree.insert(value);
  }
  int cnt = 0;
  for (auto it = tree.gteq(5); it != tree.gteq(10); ++it) {
    std::cout << *it << ' ';
  }
  std::cout << std::endl;
}

TEST (persistency, trait_1) {
  using value_t = int;
  using trait_t = Trait2 <value_t>;
  const int order = 4;
  BPlusTree <trait_t, order> tree(header_path, data_path, false);
}

TEST (custom_struct, trait_1) {
  struct User {
    int id;
    int edad;
    int n_mascotas;
  };

  class Trait {
  public:
    using value_t = User;
    using key_t = int;
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
        return value.id;
      }
    };

    using print_t = TreePrint;
    using key_g = Key;
  };

  using value_t = User;
  const int order = 10;
  BPlusTree <Trait, order> tree(header_path, data_path);
  const int n_users = 100;
  std::vector <User> users;
  for (int id = 1; id <= n_users; id++) {
    User user;
    user.id = id;
    user.edad = Random::generate_value <int> (1, 100);
    user.n_mascotas = Random::generate_value <int> (1, 10);
    users.push_back(user);
    tree.insert(user);
  }
  for (auto user: users) {
    EXPECT_TRUE(tree.find(user));
  }
  int cnt = 0;
  for (auto it = tree.begin(); it != tree.end(); ++it) {
    User user = *it;
    EXPECT_TRUE(user.id == ++cnt);
  }
  EXPECT_TRUE(cnt == n_users);
}

int main (int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
