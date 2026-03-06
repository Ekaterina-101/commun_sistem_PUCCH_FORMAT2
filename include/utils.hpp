#ifndef UTILS_HPP
#define UTILS_HPP

#include "global_variables.hpp"
#include <complex>
#include <iostream>
#include <vector>

std::vector<int> int_to_binary_vector(int i, int length);
int binary_vector_to_int(const std::vector<int> &binary_vector);
std::vector<int> int_to_binary_vector_head(int i, int length);
void print_vector(const std::vector<int> &vec);
void print_double_vector(const std::vector<double> &vec);
void print_complex_vector_compact(const std::vector<std::complex<double>> &vec);

#endif // UTILS_HPP