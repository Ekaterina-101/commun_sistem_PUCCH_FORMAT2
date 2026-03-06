#include "decoder.hpp"
#include "coder_RM.hpp"
#include "utils.hpp"

double calculated_metric(std::vector<double> &input_LLR,
                         std::vector<int> &code_bits) {
  if (input_LLR.size() != code_bits.size()) {
    std::cerr << "Error: size mismatch between input LLR and code bits"
              << std::endl;
    COUNTER_ERRORS++;
    return NAN;
  }

  double metric = 0.0;
  for (size_t i = 0; i < code_bits.size(); i++) {
    if (code_bits[i] == 0) {
      metric += input_LLR[i];
    } else {
      metric += (-1.0) * input_LLR[i];
    }
  }
  return metric;
}

std::vector<int> decoder_RM(std::vector<double> &input_LLR, int k) {
  int best_information_bits = 0;
  double max_metric = calculated_metric(input_LLR, CODE_WORDS[0]);
  if (COUNTER_ERRORS > 0) {
    return {};
  }
  for (std::size_t i = 0; i < (1 << k); i++) {

    double metric = calculated_metric(input_LLR, CODE_WORDS[i << (RM_K - k)]);
    if (COUNTER_ERRORS > 0) {
      return {};
    }
    // int a = (i << (RM_K - k));
    // std::cout << metric << std::endl << i << " " << a << std::endl;
    // print_vector(int_to_binary_vector(i << (RM_K - k), 13));
    // print_vector(int_to_binary_vector_head(a, k));
    // print_vector(CODE_WORDS[i << (RM_K - k)]);

    if (max_metric < metric) {
      max_metric = metric;
      best_information_bits = i << (RM_K - k);
    }
  }

  return int_to_binary_vector_head(best_information_bits, k);
}
