#include "coder_RM.hpp"

std::vector<int> coder_RM(const std::vector<int> &input_bits) {
  // if (CODE_WORDS.size() == (1 << RM_K)) {
  //     COUNTER_ERRORS++;
  //     return CODE_WORDS[binary_vector_to_int(input_bits)];
  // }

  std::vector<int> code_bits(RM_N);

  for (std::size_t i = 0; i < input_bits.size(); i++) {
    if (input_bits[i] == 0) {
      continue;
    }

    for (std::size_t j = 0; j < RM_N; j++) {
      code_bits[j] ^= G[i][j];
    }
  }
  return code_bits;
}
