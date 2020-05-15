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

const std::string measures_path = "measures.txt";
const std::string header_path = "../persistency/header_measure.bin";
const std::string data_path = "../persistency/data_measure.bin";
const int N = 1e5;
const int ITERATIONS = 3;

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

Measure simulate (int n_elements) {
  std::vector <TaxiDriver> data;
  for (int i = 0; i < n_elements; i++) {
    data.push_back(generate_fake_taxi_drive());
  }
  std::mt19937 rng;
  rng.seed(std::random_device()());
  std::shuffle(std::begin(data), std::end(data), rng);
  const int order = 1024;
  BPlusTree <Trait, order> tree(header_path, data_path, true);
  Measure measure;
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (auto driver: data) {
      tree.insert(driver);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast <std::chrono::microseconds> (stop - start);
    measure.insert_time = duration.count();
    measure.insert_n_reads = tree.get_n_reads();
    measure.insert_n_writes = tree.get_n_writes();
  }
  {
    int prev_n_reads = tree.get_n_reads();
    int prev_n_writes = tree.get_n_writes();
    auto start = std::chrono::high_resolution_clock::now();
    tree.find(data[0]);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast <std::chrono::microseconds> (stop - start);
    measure.search_time = duration.count();
    measure.search_n_reads = tree.get_n_reads() - prev_n_reads;
    measure.search_n_writes = tree.get_n_writes() - prev_n_writes;
    return measure;
  }
}

int main () {
  std::ofstream file;
  file.open(measures_path, std::ios::out | std::ios::trunc);
  int n_elements = 1;
  while (n_elements <= N) {
    for (int i = 0; i < ITERATIONS; i++) {
      Measure measure = simulate(n_elements);
      file << n_elements << ' ';
      file << measure.insert_time << ' ' << measure.insert_n_reads << ' ' << measure.insert_n_writes << ' ';
      file << measure.search_time << ' ' << measure.search_n_reads << ' ' << measure.search_n_writes;
      file << std::endl;
    }
    int log = std::log10(n_elements);
    n_elements += pow(10, log);
    std::cout << n_elements << std::endl;
  }
  file.close();
  return (0);
}
