#include "coder_RM.hpp"

std::vector<int> coder_RM(const std::vector<int> &input_bits) {
	if (input_bits.size() > RM_K) {
		throw std::runtime_error(
			"coder_RM: input bits size more max size code Reed-Muller");
	}
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
