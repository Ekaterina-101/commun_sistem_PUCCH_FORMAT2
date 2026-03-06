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

std::vector<int> generate_vector(std::size_t size) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, 1);

	std::vector<int> result(size);
	for (std::size_t i = 0; i < size; i++) {
		result[i] = dist(gen);
	}
	return result;
}

#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "JSON.hpp"
#include "QPSK.hpp"
#include "channel_AWGN.hpp"
#include "coder_RM.hpp"
#include "decoder.hpp"
#include "generate_codetable.hpp"
#include "global_variables.hpp"
#include "utils.hpp"

std::vector<std::vector<int>> CODE_WORDS = generate_codewords();
double SNR_FOR_SIMULATION = 10.0;

int failed_trans(int k, double Es_N0, int iterations = 100000) {
	if (iterations <= 0) {
		throw std::invalid_argument(
			"failed_trans: iterations must be positive");
	}
	if (k < 0 || k > RM_K) {
		throw std::invalid_argument("failed_trans: invalid k parameter");
	}
	int errors_word = 0;
	for (int i = 0; i < iterations; i++) {
		std::vector<int> message = generate_vector(k);
		std::vector<int> code = coder_RM(message);
		std::vector<std::complex<double>> modem = QPSKmod(code);
		std::vector<std::complex<double>> modem_noised =
			channel_AWGN(Es_N0, modem);
		std::vector<double> demod = QPSKdemod_soft(modem_noised, Es_N0);
		std::vector<int> message_decode = decoder_RM(demod, message.size());

		if (message != message_decode) {
			errors_word++;
		}
	}

	return errors_word;
}

void BLER(double Es_N0_min = -6.0, double Es_N0_max = 6.0,
		  double Es_N0_step = 0.5, int iterations = 100000,
		  std::string output_file = "data/BLER_data.txt") {
	std::ofstream file(output_file);
	if (!file.is_open()) {
		throw std::runtime_error("BLER: can't open output file '" +
								 output_file + "'");
	}
	std::size_t possible_k[] = {2, 4, 6, 8, 11};
	for (std::size_t i_k = 0; i_k < 5; i_k++) {
		file << "k = " << possible_k[i_k] << std::endl;

		for (double Es_N0 = Es_N0_min; Es_N0 <= Es_N0_max;
			 Es_N0 += Es_N0_step) {
			file << "Es/N0 = " << Es_N0 << " BLER = ";
			file << std::scientific
				 << double(failed_trans(possible_k[i_k], Es_N0, iterations)) /
						iterations
				 << std::endl;
		}
	}
	file.close();
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " input.json or output.json"
				  << std::endl;
		return 1;
	}

	std::ifstream input_file(argv[1]);
	if (!input_file.is_open()) {
		std::cerr << "Error: can't open input file '" << argv[1] << "'"
				  << std::endl;
		return 2;
	}

	std::ofstream output_file(argv[2]);
	if (!output_file.is_open()) {
		std::cerr << "Error: can't open output file '" << argv[2] << "'"
				  << std::endl;
		return 3;
	}

	json input_json;
	try {
		input_json = json::parse(input_file);
	} catch (const json::parse_error &e) {
		std::cerr << "Error: JSON parse error at byte " << e.byte << ": "
				  << e.what() << std::endl;
		return 4;
	}

	if (!input_json.contains("mode") || !input_json["mode"].is_string()) {
		std::cerr << "Error: missing or invalid 'mode' field" << std::endl;
		return 5;
	}

	std::string mode = input_json["mode"].get<std::string>();
	if (mode != "coding" && mode != "decoding" &&
		mode != "channel simulation") {
		std::cerr << "Error: Invalid 'mode' value '" << mode
				  << "' (must be 'coding', 'decoding' or 'channel simulation')"
				  << std::endl;
		return 6;
	}

	if (!input_json.contains("num_of_pucch_f2_bits") ||
		!input_json["num_of_pucch_f2_bits"].is_number_integer()) {
		std::cerr << "Error: Missing or invalid 'num_of_pucch_f2_bits' field"
				  << std::endl;
		return 7;
	}

	int num_bits = input_json["num_of_pucch_f2_bits"].get<int>();
	if (!is_valid_bit_count(num_bits)) {
		std::cerr << "Error: Invalid 'num_of_pucch_f2_bits' value " << num_bits
				  << " must be 2, 4, 6, 8 or 11" << std::endl;
		return 8;
	}

	json output_json;
	output_json["mode"] = mode;

	try {
		if (mode == "coding") {
			if (!input_json.contains("pucch_f2_bits")) {
				std::cerr
					<< "Error: Missing 'pucch_f2_bits' field for 'coding' mode"
					<< std::endl;
				return 9;
			}
			if (!validate_bits(input_json["pucch_f2_bits"], num_bits)) {
				return 10;
			}

			std::vector<int> bits =
				json_to_vector_int(input_json["pucch_f2_bits"]);
			auto code = coder_RM(bits);
			auto QPSK_signal = QPSKmod(code);

			output_json["qpsk_symbols"] = vector_complex_to_json(QPSK_signal);

		} else if (mode == "decoding") {
			if (!input_json.contains("qpsk_symbols")) {
				std::cerr
					<< "Error: Missing 'qpsk_symbols' field for 'decoding' mode"
					<< std::endl;
				return 11;
			}
			if (!validate_qpsk_symbols(input_json["qpsk_symbols"])) {
				return 12;
			}

			std::vector<std::complex<double>> QPSK_signal =
				json_to_vector_complex(input_json["qpsk_symbols"]);
			std::vector<double> signal_LLR =
				QPSKdemod_soft(QPSK_signal, SNR_FOR_SIMULATION);
			std::vector<int> decode_bits = decoder_RM(signal_LLR, num_bits);
			output_json["num_of_pucch_f2_bits"] = num_bits;
			output_json["pucch_f2_bits"] = vector_int_to_json(decode_bits);
		} else if (mode == "channel simulation") {
			if (!input_json.contains("iterations") ||
				!input_json["iterations"].is_number_integer()) {
				std::cerr << "Error: Missing or invalid 'iterations' field for "
							 "'channel "
							 "simulation' mode"
						  << std::endl;
				return 13;
			}
			int iterations = input_json["iterations"].get<int>();
			if (iterations <= 0) {
				std::cerr << "Error: 'iterations' must be positive (got "
						  << iterations << ")" << std::endl;
				return 14;
			}

			int error_trans =
				failed_trans(num_bits, SNR_FOR_SIMULATION, iterations);

			output_json["num_of_pucch_f2_bits"] = num_bits;
			output_json["bler"] = double(error_trans) / iterations;
			output_json["success"] = iterations - error_trans;
			output_json["failed"] = error_trans;
		}

		output_file << output_json.dump(2) << std::endl;
		output_file.close();

	} catch (const nlohmann::json::parse_error &e) {
		std::cerr << "JSON parse error at byte " << e.byte << ": " << e.what()
				  << std::endl;
		return -1;
	} catch (const std::invalid_argument &e) {
		std::cerr << "Invalid argument: " << e.what() << std::endl;
		return -2;
	} catch (const std::runtime_error &e) {
		std::cerr << "Runtime error: " << e.what() << std::endl;
		return -3;
	} catch (const std::exception &e) {
		std::cerr << "Standard exception: " << e.what() << std::endl;
		return -4;
	}

	return 0;
}

// int main() {
//     BLER();
// }
