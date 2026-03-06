#ifndef DECODER_HPP
#define DECODER_HPP

#include "global_variables.hpp"
#include <complex>
#include <iostream>
#include <vector>

std::vector<int> decoder_RM(std::vector<double> &input_LLR, int k);

#endif // DECODER_HPP