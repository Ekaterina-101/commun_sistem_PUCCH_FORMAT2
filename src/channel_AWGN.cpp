#include "channel_AWGN.hpp"

std::vector<std::complex<double>>
channel_AWGN(double Es_N0, std::vector<std::complex<double>> &input_bits) {
	if (input_bits.empty()) {
		throw std::invalid_argument("channel_AWGN: nput bits vector is empty");
	}

	double disp = 0.5 * std::pow(10.0, -Es_N0 / 10.0);
	double sigma = std::sqrt(disp / 2.0);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<double> dist(0.0, 1.0);
	std::vector<std::complex<double>> noised_bits(input_bits.size());

	for (std::size_t i = 0; i < input_bits.size(); i++) {
		noised_bits[i].real(input_bits[i].real() + sigma * dist(gen));
		noised_bits[i].imag(input_bits[i].imag() + sigma * dist(gen));
	}

	return noised_bits;
}