#include "decoder.hpp"

double calculated_metric(std::vector<double> &input_LLR,
						 std::vector<int> &code_bits) {
	if (input_LLR.size() != code_bits.size()) {
		throw std::runtime_error(
			"metric: size mismatch between input LLR and code bits");
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
	if (k < 0 || k > RM_K) {
		throw std::invalid_argument("decoder_RM: invalid k parameter");
	}
	if (input_LLR.empty()) {
		throw std::invalid_argument("decoder_RM: empty input LLR vector");
	}
	int best_information_bits = 0;
	double max_metric = calculated_metric(input_LLR, CODE_WORDS[0]);
	for (std::size_t i = 0; i < std::size_t(1 << k); i++) {

		double metric =
			calculated_metric(input_LLR, CODE_WORDS[i << (RM_K - k)]);

		if (max_metric < metric) {
			max_metric = metric;
			best_information_bits = i << (RM_K - k);
		}
	}

	return int_to_binary_vector_head(best_information_bits, k);
}
