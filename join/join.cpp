#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <ctime>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <random>

#include "../b_plus_tree/BPlusTree.hpp"
#include "../utility/Random.hpp"

const std::string header_path1 = "../persistency/header_join1.bin";
const std::string data_path1 = "../persistency/data_join1.bin";
const std::string header_path2 = "../persistency/header_join2.bin";
const std::string data_path2 = "../persistency/data_join2.bin";
const std::string header_path3 = "../persistency/header_join3.bin";
const std::string data_path3 = "../persistency/data_join3.bin";

struct TaxiDriver {
  int dni;
  int edad;
  char nombre[20];
  char empresa[20];
  char modelo_carro[10];
  char placa[10];
  float ingresos;
};

struct Trait {
  using value_t = TaxiDriver;
  using key_t = int;
  using address = int;

  struct TreePrint {
    int operator () () const {
      return TREE_PRINT;
    }
  };

  struct Key {
    key_t operator () (value_t value) const {
      return value.dni;
    }
  };

  using print_t = TreePrint;
  using key_g = Key;
};

std::string generate_random_string (int len) {
  assert(len >= 0);
  std::string ret;
  for (int i = 0; i < len; i++) {
    ret += Random::generate_value <char> ();
  }
  return ret;
}

int prev_id;

TaxiDriver generate_fake_taxi_drive () {
  TaxiDriver driver;
  driver.dni = ++prev_id;
  driver.edad = Random::generate_value <int> (1, 100);
  strcpy(driver.nombre, generate_random_string(6).c_str());
  strcpy(driver.empresa, generate_random_string(10).c_str());
  strcpy(driver.modelo_carro, generate_random_string(7).c_str());
  strcpy(driver.placa, generate_random_string(8).c_str());
  driver.ingresos = Random::generate_value <float> (100, 5000);
  return driver;
}

void init () {
  prev_id = 0;
}

struct Measure {
  long long insert_time;
  int insert_n_reads;
  int insert_n_writes;
  long long search_time;
  int search_n_reads;
  int search_n_writes;
};

int main () {
  const int n_elements1 = 1e3;
  const int n_elements2 = 1e4;
  const int order = 1024;
  BPlusTree <Trait, order> tree1(header_path1, data_path1, true);
  init();
  for (int i = 0; i < n_elements1; i++) {
    tree1.insert(generate_fake_taxi_drive());
  }
  BPlusTree <Trait, order> tree2(header_path2, data_path2, true);
  init();
  for (int i = 0; i < n_elements2; i++) {
    tree2.insert(generate_fake_taxi_drive());
  }
  // join tree1 and tree2 in tree3
  BPlusTree <Trait, order> tree_join(header_path3, data_path3, true);
  for (auto it = tree1.begin(); it != tree1.end(); ++it) {
    tree_join.insert(*it);
  }
  for (auto it = tree2.begin(); it != tree2.end(); ++it) {
    tree_join.insert(*it);
  }
  return (0);
}
