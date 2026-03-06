#include "utils.hpp"

std::vector<int> int_to_binary_vector_head(int i, int length) {
  std::vector<int> binary_vector(length, 0);

  if (i < 0) {
    std::cerr << "Error: invaid number" << std::endl;
    COUNTER_ERRORS++;
    return binary_vector;
  }

  // for (int temp = i; temp != 0; temp >>= 1) {
  //     msb_pos++;
  // }

  for (std::size_t j = 0; j < length && j <= RM_K - 1; j++) {
    binary_vector[j] = (i >> (RM_K - 1 - j)) & 1;
  }

  return binary_vector;
}

std::vector<int> int_to_binary_vector(int i, int length) {
  std::vector<int> binary_vector(length);

  for (std::size_t j = 0; j < length; j++) {
    binary_vector[j] = (i >> (length - j - 1)) & 1;
  }

  return binary_vector;
}

int binary_vector_to_int(const std::vector<int> &binary_vector) {
  int result = 0;
  for (int bit : binary_vector) {
    result = (result << 1) | bit;
  }
  return result;
}

void print_vector(const std::vector<int> &vec) {
  for (size_t i = 0; i < vec.size(); ++i) {
    std::cout << vec[i];
    if (i < vec.size() - 1) {
      std::cout << " ";
    }
  }
  std::cout << std::endl;
  std::cout << std::endl;
}
void print_double_vector(const std::vector<double> &vec) {
  for (size_t i = 0; i < vec.size(); ++i) {
    std::cout << vec[i];
    if (i < vec.size() - 1) {
      std::cout << " ";
    }
  }
  std::cout << std::endl;
  std::cout << std::endl;
}
void print_complex_vector_compact(
    const std::vector<std::complex<double>> &vec) {
  std::cout << "[";
  for (std::size_t i = 0; i < vec.size(); ++i) {
    std::cout << vec[i].real() << (vec[i].imag() >= 0 ? "+" : "")
              << vec[i].imag() << "i";
    if (i < vec.size() - 1)
      std::cout << ", ";
  }
  std::cout << "]" << std::endl;
  std::cout << std::endl;
}
