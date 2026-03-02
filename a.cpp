#include <vector>
#include <random>
#include <cmath>
#include <complex>

#include <vector>
#include <cmath>
#include <complex>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>


void print_vector(const std::vector<int>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i < vec.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    std::cout << std::endl;
}
void print_double_vector(const std::vector<double>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i < vec.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    std::cout << std::endl;
}
void print_complex_vector_compact(const std::vector<std::complex<double>>& vec) {
    std::cout << "[";
    for (std::size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i].real() << (vec[i].imag() >= 0 ? "+" : "") 
                  << vec[i].imag() << "i";
        if (i < vec.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << std::endl;
}

std::vector<std::complex<double>> AWGN_Q(double SNR, std::vector<std::complex<double>>& input_bits) {
    double disp = 0.5 * std::pow(10.0, -SNR/10.0);

    std::random_device rd;
    std::mt19937 gen(rd());
    double sigma = std::sqrt(disp/2.0);
    std::normal_distribution<double> dist(0.0, 1.0);
    std::vector<std::complex<double>> noised_bits(input_bits.size());

    for (std::size_t i = 0; i < input_bits.size(); i++) {
        noised_bits[i].real(input_bits[i].real() + sigma * dist(gen));
        noised_bits[i].imag(input_bits[i].imag() + sigma * dist(gen));
    }

    return noised_bits;
}

double norm = 1.0 / std::sqrt(2.0);

std::vector<std::complex<double>> QPSKmod(const std::vector<int>& input_bits) {
    std::vector<std::complex<double>> modulated_signal(input_bits.size() / 2);

    for (std::size_t i = 0, j = 0; i < input_bits.size(); i += 2, j++) {
        modulated_signal[j].real((2 * input_bits[i + 1] - 1) / std::sqrt(2.0));
        modulated_signal[j].imag((2 * input_bits[i] - 1) / std::sqrt(2.0));
    }

    return modulated_signal;
}




std::vector<double> QPSKdemod_soft(const std::vector<std::complex<double>>& input_signal, double SNR) {
    std::vector<double> output_LLR(input_signal.size() * 2);
    double disp = 0.5 * std::pow(10.0, -SNR/10.0);


    for (size_t i = 0, j = 0; j < input_signal.size(); i += 2, j++) {
        double d00 = std::norm(input_signal[j] - std::complex<double>(-1.0 / std::sqrt(2.0), -1.0 / std::sqrt(2.0)));
        double d10 = std::norm(input_signal[j] - std::complex<double>(-1.0 / std::sqrt(2.0), 1.0  / std::sqrt(2.0)));
        double d01 = std::norm(input_signal[j] - std::complex<double>(1.0 / std::sqrt(2.0), -1.0 / std::sqrt(2.0)));
        double d11 = std::norm(input_signal[j] - std::complex<double>(1.0 / std::sqrt(2.0), 1.0 / std::sqrt(2.0)));


        double sum0_first_bit = std::exp(d00/(-2.0 * disp)) + std::exp(d01/(-2.0 * disp));
        double sum1_first_bit = std::exp(d10/(-2.0 * disp)) + std::exp(d11/(-2.0 * disp));
        output_LLR[i] = std::log(sum0_first_bit/sum1_first_bit);

        double sum0_second_bit = std::exp(d00/(-2.0 * disp)) + std::exp(d10/(-2.0 * disp));
        double sum1_second_bit = std::exp(d01/(-2.0 * disp)) + std::exp(d11/(-2.0 * disp));
        output_LLR[i + 1] = std::log(sum0_second_bit/sum1_second_bit);

    }

    return output_LLR;
}


std::vector<int> coder_RM(std::vector<int>& input_bits) {
    std::vector<std::vector<int>> G = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0},
        {0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0},
        {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1},
        {0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0},
        {0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0},
        {0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}
    };

    if (G.size() != input_bits.size()) {
        //error
    }
    std::vector<int> code_bits(G[0].size());

    for (std::size_t i = 0; i <  input_bits.size(); i++) {
        if (input_bits[i] == 0) {
            continue;
        }

       for (std::size_t j = 0; j < G[0].size(); j++) {
            code_bits[j] ^= G[i][j];
        } 
    }

    return code_bits;
}

double calculated_metric(std::vector<double>& input_LLR, std::vector<int>& code_bits) {

    if (input_LLR.size() != code_bits.size()) {
        //error
        std::cout << "erorr" << std::endl;
    }

    double metric = 0.0;
    for (size_t i = 0; i < input_LLR.size(); i++) {
        if (code_bits[i] == 0) {
            metric += input_LLR[i];
        } else {
            metric += (-1.0) * input_LLR[i];
        }
    }
    return metric;
    
}

std::vector<int> decoder_RM(std::vector<double>& input_LLR, int k) {
    std::vector<std::vector<int>> code_word((1 << k), std::vector<int>(20));
    std::vector<std::vector<int>> information_word((1 << k), std::vector<int>(k));

    for (std::size_t i = 0; i < (1 << k); i++) {
        for (std::size_t j = 0; j < k; j++) {
            information_word[i][j] = (i >> (k - j - 1)) & 1;
        }
        
        code_word[i] = coder_RM(information_word[i]);
    }

    int best_information_bits = 0;
    double max_metric = calculated_metric(input_LLR, code_word[0]);
    for (std::size_t i = 0; i < (1 << k); i++) {

        double metric = calculated_metric(input_LLR, code_word[i]);

        if (max_metric < metric) {
            max_metric = metric;
            best_information_bits = i;
        }

    }


    return information_word[best_information_bits];
}


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

int main() {
    // double SNR = -10.0;
    // // std::vector<int> m = {1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1};
    // std::vector<int> m = {1, 0};
    // print_vector(m);
    // std::vector<int> c = coder_RM(m);
    // print_vector(c);
    // std::vector<std::complex<double>> c_modem = QPSKmod(c);
    // print_complex_vector_compact(c_modem);
    // std::vector<std::complex<double>> c_modem_awgn = AWGN_Q(SNR, c_modem, double(m.size())/20.0);
    // print_complex_vector_compact(c_modem_awgn);
    // std::vector<double> c_demod = QPSKdemod_soft(c_modem_awgn, SNR, m.size()/20.0);
    // print_double_vector(c_demod);
    // std::cout << c_demod.size() << std::endl;
    // std::vector<int> m_decode = decoder_RM(c_demod, m.size());
    // print_vector(m_decode);

    std::ofstream file("BLER_data.txt");
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл!" << std::endl;
    }

    std::size_t possible_k[] = {2, 4, 6, 8, 11};
    double SNR_min = 0.0;
    double SNR_max = 2.0;
    double SNR_step = 0.5;
    std::size_t iterations = 1000; 
    for (std::size_t i_k = 0; i_k < 5; i_k++) {
        file << "k = " << possible_k[i_k] << std::endl;
        
        for (double SNR = SNR_min; SNR <= SNR_max; SNR += SNR_step) {
            int errors_word = 0;
            for (int i = 0; i < iterations; i++) {
                std::vector<int> message = generate_vector(possible_k[i_k]);
                std::vector<int> code = coder_RM(message);
                std::vector<std::complex<double>> modem = QPSKmod(code);
                std::vector<std::complex<double>> modem_noised = AWGN_Q(SNR, modem);
                std::vector<double> demod = QPSKdemod_soft(modem_noised, SNR);
                std::vector<int> message_decode = decoder_RM(demod, message.size());

                if (message != message_decode) {
                    errors_word++;
                }
            }
            file << "SNR = " << SNR << " BLER = ";
            file << std::scientific << (double(errors_word) / iterations) << std::endl;
        }
        
        
    }
    file.close();
}