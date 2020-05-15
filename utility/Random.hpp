#pragma once

#include <random>
#include <type_traits>

class Random {
public:
  template <typename T>
  static T generate_value (const T min_val = 0, const T max_val = 0) {
    Random random;
    if (std::is_same <T, int>::value) {
      return random.generate_integer <T> (min_val, max_val);
    }
    if (std::is_same <T, long long>::value) {
      return random.generate_integer <T> (min_val, max_val);
    }
    if (std::is_same <T, float>::value) {
      return random.generate_real <T> (min_val, max_val);
    }
    if (std::is_same <T, double>::value) {
      return random.generate_real <T> (min_val, max_val);
    }
    if (std::is_same <T, char>::value) {
      return random.generate_char();
    }
    throw "Data type not supported";
  }

private:

  template <typename int_type>
  int_type generate_integer (const int_type min_val, const int_type max_val) {
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution <> distribution(min_val, max_val);
    return distribution(rng);
  }

  template <typename float_type>
  float_type generate_real (const float_type min_val, const float_type max_val) {
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution <> distribution(min_val, max_val);
    return distribution(rng);
  }

  char generate_char () {
    std::mt19937 rng;
    rng.seed(std::random_device()());
    int increment = generate_integer <int> (0, 52) % 26;
    return 'a' + increment;
  }
};
