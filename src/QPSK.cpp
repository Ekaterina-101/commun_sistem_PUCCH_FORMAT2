#include "QPSK.hpp"

std::vector<std::complex<double>> QPSKmod(std::vector<int> input_bits) {
	std::vector<std::complex<double>> modulated_signal(input_bits.size() / 2);

	for (std::size_t i = 0, j = 0; i < input_bits.size(); i += 2, j++) {
		modulated_signal[j].real((2 * input_bits[i + 1] - 1) / std::sqrt(2.0));
		modulated_signal[j].imag((2 * input_bits[i] - 1) / std::sqrt(2.0));
	}

	return modulated_signal;
}

std::vector<double>
QPSKdemod_soft(const std::vector<std::complex<double>> &input_signal,
			   double Es_N0) {
	std::vector<double> output_LLR(input_signal.size() * 2);
	double disp = 0.5 * std::pow(10.0, -Es_N0 / 10.0);

	for (size_t i = 0, j = 0; j < input_signal.size(); i += 2, j++) {
		double d00 = std::norm(
			input_signal[j] -
			std::complex<double>(-1.0 / std::sqrt(2.0), -1.0 / std::sqrt(2.0)));
		double d10 = std::norm(
			input_signal[j] -
			std::complex<double>(-1.0 / std::sqrt(2.0), 1.0 / std::sqrt(2.0)));
		double d01 = std::norm(
			input_signal[j] -
			std::complex<double>(1.0 / std::sqrt(2.0), -1.0 / std::sqrt(2.0)));
		double d11 = std::norm(
			input_signal[j] -
			std::complex<double>(1.0 / std::sqrt(2.0), 1.0 / std::sqrt(2.0)));

		double sum0_first_bit =
			std::exp(d00 / (-2.0 * disp)) + std::exp(d01 / (-2.0 * disp));
		double sum1_first_bit =
			std::exp(d10 / (-2.0 * disp)) + std::exp(d11 / (-2.0 * disp));
		output_LLR[i] = std::log(sum0_first_bit / sum1_first_bit);

		double sum0_second_bit =
			std::exp(d00 / (-2.0 * disp)) + std::exp(d10 / (-2.0 * disp));
		double sum1_second_bit =
			std::exp(d01 / (-2.0 * disp)) + std::exp(d11 / (-2.0 * disp));
		output_LLR[i + 1] = std::log(sum0_second_bit / sum1_second_bit);
	}

	return output_LLR;
}