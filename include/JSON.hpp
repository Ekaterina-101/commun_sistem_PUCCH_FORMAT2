#ifndef JSON_HPP
#define JSON_HPP

#include <algorithm>
#include <complex>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using json = nlohmann::json;

bool is_valid_bit_count(int value);
std::complex<double> parse_complex(const std::string &str);
std::string complex_to_string(const std::complex<double> &c);
bool validate_bits(const json &bits_array, std::size_t expected_size);
bool validate_qpsk_symbols(const json &symbols_array);
json vector_int_to_json(const std::vector<int> &vec);
std::vector<int> json_to_vector_int(const json &j);
json vector_complex_to_json(const std::vector<std::complex<double>> &vec);
std::vector<std::complex<double>> json_to_vector_complex(const json &j);

#endif // JSON_HPP