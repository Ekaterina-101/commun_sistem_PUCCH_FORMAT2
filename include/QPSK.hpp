#ifndef QPSK_HPP
#define QPSK_HPP

#include "utils.hpp"
#include <complex>
#include <vector>

std::vector<std::complex<double>> QPSKmod(std::vector<int> input_bits);
std::vector<double>
QPSKdemod_soft(const std::vector<std::complex<double>> &input_signal,
               double SNR);

#endif // QPSK_HPP