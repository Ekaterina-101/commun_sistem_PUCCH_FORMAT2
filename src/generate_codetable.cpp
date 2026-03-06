#include "generate_codetable.hpp"

std::vector<std::vector<int>> generate_codewords() {
	std::vector<std::vector<int>> code_word((1 << RM_K),
											std::vector<int>(RM_N));

	for (std::size_t i = 0; i < (1 << RM_K); i++) {
		std::vector<int> information_word = int_to_binary_vector(i, RM_K);
		std::vector<int> code = coder_RM(information_word);
		code_word[i] = code;
	}

	return code_word;
}
