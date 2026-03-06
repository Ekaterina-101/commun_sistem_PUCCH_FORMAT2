#include "utils.hpp"

std::vector<int> int_to_binary_vector_head(int i, int length) {
	if (i < 0) {
		throw std::invalid_argument(
			"int_to_binary_vector_head: negative input number");
	}
	if (length < 0 || length > RM_K) {
		throw std::invalid_argument(
			"int_to_binary_vector_head: invalid length parameter");
	}

	std::vector<int> binary_vector(length, 0);

	for (std::size_t j = 0; j < std::size_t(length); j++) {
		binary_vector[j] = (i >> (RM_K - 1 - j)) & 1;
	}

	return binary_vector;
}

std::vector<int> int_to_binary_vector(int i, int length) {
	if (length < 0) {
		throw std::invalid_argument("int_to_binary_vector: negative length");
	}
	std::vector<int> binary_vector(length);

	for (std::size_t j = 0; j < std::size_t(length); j++) {
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
