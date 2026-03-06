#ifndef AWGN_HPP
#define AWGN_HPP

#include <cmath>
#include <complex>
#include <random>
#include <vector>

#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

std::vector<std::complex<double>>
channel_AWGN(double Es_N0, std::vector<std::complex<double>> &input_bits);

#endif // AWGN_HPP