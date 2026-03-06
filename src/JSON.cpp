#include "JSON.hpp"
#include "utils.hpp"

const std::vector<int> valid_bit_counts = {2, 4, 6, 8, 11};

bool is_valid_bit_count(int value) {
  return value == 2 || value == 4 || value == 6 || value == 8 || value == 11;
}

// bool parse_complex(const std::string& str, std::complex<double>& result) {
//     std::regex complex_pattern(R"(^([+-]?\d*\.?\d+)([+-]\d*\.?\d+)j$)");
//     std::smatch matches;

//     if (!std::regex_match(str, matches, complex_pattern) || matches.size() !=
//     3) {
//         return false;
//     }

//     try {
//         double real = std::stod(matches[1].str());
//         double imag = std::stod(matches[2].str());
//         result = std::complex<double>(real, imag);
//         return true;
//     } catch (...) {
//         return false;
//     }
// }

bool validate_bits(const json &bits_array, int expected_size) {
  // if (!bits_array.is_array()) {
  //     std::cerr << "Error: 'pucch_f2_bits' must be an array" << std::endl;
  //     return false;
  // }

  if (bits_array.size() != expected_size) {
    std::cerr << "Error: mismatch num_of_pucch_f2_bits " << expected_size
              << " and size " << bits_array.size() << std::endl;
    return false;
  }

  for (std::size_t i = 0; i < bits_array.size(); ++i) {
    int bit = bits_array[i].get<int>();
    if (bit != 0 && bit != 1) {
      std::cerr << "Error: bit at index " << i << " has invalid value " << bit
                << " (must be 0 or 1)" << std::endl;
      return false;
    }
  }
  return true;
}

std::complex<double> parse_complex(const std::string &str) {
  if (str.empty() || str.back() != 'j') {
    return std::complex<double>(NAN, NAN);
  }

  std::size_t sign_pos = str.find_last_of("+-", str.size() - 2);
  if (sign_pos <= 0) {
    return std::complex<double>(NAN, NAN);
  }

  std::string real_part = str.substr(0, sign_pos);
  std::string imag_part = str.substr(sign_pos, str.size() - sign_pos - 1);

  return std::complex<double>(std::stod(real_part), std::stod(imag_part));
}

bool validate_qpsk_symbols(const json &symbols_array) {
  if (!symbols_array.is_array()) {
    std::cerr << "Error: 'qpsk_symbols' must be an array" << std::endl;
    return false;
  }

  for (std::size_t i = 0; i < symbols_array.size(); ++i) {
    if (!symbols_array[i].is_string()) {
      std::cerr << "Error: QPSK symbol at index " << i << " is not a string"
                << std::endl;
      return false;
    }

    std::complex<double> c = parse_complex(symbols_array[i].get<std::string>());
    if (std::isnan(c.real()) || std::isnan(c.imag())) {
      std::cerr << "Error: invalid QPSK symbol format at index " << i << ": '"
                << symbols_array[i].get<std::string>() << "'" << std::endl;
      return false;
    }
  }
  return true;
}

std::vector<int> json_to_vector_int(const json &j) {
  std::vector<int> result;
  result.reserve(j.size());

  for (const auto &bit : j) {
    result.push_back(bit.get<int>());
  }

  return result;
}

json vector_int_to_json(const std::vector<int> &vec) {
  json result = json::array();

  for (const auto &bit : vec) {
    result.push_back(bit);
  }

  return result;
}

std::vector<std::complex<double>> json_to_vector_complex(const json &j) {
  std::vector<std::complex<double>> result;
  result.reserve(j.size());

  for (const auto &signal : j) {
    result.push_back(parse_complex(signal));
  }

  return result;
}

std::string complex_to_string(const std::complex<double> &c) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(3) << c.real();

  if (c.imag() >= 0) {
    oss << "+";
  }
  oss << std::fixed << std::setprecision(3) << c.imag() << "j";

  return oss.str();
}

json vector_complex_to_json(const std::vector<std::complex<double>> &vec) {
  json result = json::array();

  for (const auto &signal : vec) {
    result.push_back(complex_to_string(signal));
  }

  return result;
}