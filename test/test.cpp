#include "JSON.hpp"
#include "QPSK.hpp"
#include "channel_AWGN.hpp"
#include "coder_RM.hpp"
#include "decoder.hpp"
#include "generate_codetable.hpp"
#include "global_variables.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <string>

std::vector<std::vector<int>> CODE_WORDS = generate_codewords();
int COUNTER_ERRORS = 0;
double SNR_FOR_SIMULATION = 10.0;

using json = nlohmann::json;

int RunBinary(const std::string &input_json, std::string &output_json) {
  std::string input_file = "/tmp/test_input_XXXXXX.json";
  std::string output_file = "/tmp/test_output_XXXXXX.json";

  {
    std::ofstream f(input_file);
    if (!f)
      return -1;
    f << input_json;
  }

  std::string cmd = "./pucch_codes_modeling " + input_file + " " + output_file;
  int exit_code = std::system(cmd.c_str());

  if (WEXITSTATUS(exit_code) == 0) {
    std::ifstream f(output_file);
    if (f.is_open()) {
      output_json.assign(std::istreambuf_iterator<char>(f),
                         std::istreambuf_iterator<char>());
      f.close();
    }
  }

  std::remove(input_file.c_str());
  std::remove(output_file.c_str());

  return WEXITSTATUS(exit_code);
}

TEST(MainTest, NoArguments) {
  int rc = std::system("./pucch_codes_modeling > /dev/null 2>&1");
  EXPECT_NE(WEXITSTATUS(rc), 0);
}

TEST(MainTest, OneArgument) {
  int rc = std::system("./pucch_codes_modeling input.json > /dev/null 2>&1");
  EXPECT_NE(WEXITSTATUS(rc), 0);
}

TEST(MainTest, NonexistentInputFile) {
  int rc = std::system(
      "./pucch_codes_modeling /nonexistent.json output.json > /dev/null 2>&1");
  EXPECT_EQ(WEXITSTATUS(rc), 1);
}

TEST(MainTest, InvalidJsonSyntax) {
  std::string output;
  int rc = RunBinary("{\"mode\": \"coding\"", output);
  EXPECT_EQ(rc, 3);
}

TEST(MainTest, EmptyFile) {
  std::string output;
  int rc = RunBinary("", output);
  EXPECT_EQ(rc, 3);
}

TEST(MainTest, MissingMode) {
  std::string output;
  int rc = RunBinary(R"({"num_of_pucch_f2_bits": 2})", output);
  EXPECT_EQ(rc, 4);
}

TEST(MainTest, ModeNotString) {
  std::string output;
  int rc = RunBinary(R"({"mode": 123, "num_of_pucch_f2_bits": 2})", output);
  EXPECT_EQ(rc, 4);
}

TEST(MainTest, InvalidModeValue) {
  std::string output;
  int rc =
      RunBinary(R"({"mode": "invalid", "num_of_pucch_f2_bits": 2})", output);
  EXPECT_EQ(rc, 5);
}

TEST(MainTest, MissingBitCount) {
  std::string output;
  int rc = RunBinary(R"({"mode": "coding"})", output);
  EXPECT_EQ(rc, 6);
}

TEST(MainTest, BitCountNotInteger) {
  std::string output;
  int rc =
      RunBinary(R"({"mode": "coding", "num_of_pucch_f2_bits": "2"})", output);
  EXPECT_EQ(rc, 6);
}

TEST(MainTest, InvalidBitCount_3) {
  std::string output;
  int rc =
      RunBinary(R"({"mode": "coding", "num_of_pucch_f2_bits": 3})", output);
  EXPECT_EQ(rc, 7);
}

TEST(MainTest, InvalidBitCount_13) {
  std::string output;
  int rc =
      RunBinary(R"({"mode": "coding", "num_of_pucch_f2_bits": 13})", output);
  EXPECT_EQ(rc, 7);
}

TEST(MainTest, Coding_MissingBits) {
  std::string output;
  int rc =
      RunBinary(R"({"mode": "coding", "num_of_pucch_f2_bits": 2})", output);
  EXPECT_EQ(rc, 8);
}

TEST(MainTest, Coding_WrongBitCount) {
  std::string output;
  int rc = RunBinary(
      R"({"mode": "coding", "num_of_pucch_f2_bits": 2, "pucch_f2_bits": [1,0,0]})",
      output);
  EXPECT_EQ(rc, 9);
}

TEST(MainTest, Coding_InvalidBitValue) {
  std::string output;
  int rc = RunBinary(
      R"({"mode": "coding", "num_of_pucch_f2_bits": 2, "pucch_f2_bits": [1,2]})",
      output);
  EXPECT_EQ(rc, 9);
}

TEST(MainTest, Coding_Valid_n2) {
  std::string output;
  int rc = RunBinary(
      R"({"mode": "coding", "num_of_pucch_f2_bits": 2, "pucch_f2_bits": [1,0]})",
      output);
  EXPECT_EQ(rc, 0);

  json out = json::parse(output);
  EXPECT_EQ(out["mode"], "coding");
  EXPECT_TRUE(out.contains("qpsk_symbols"));
  EXPECT_EQ(out["qpsk_symbols"].size(), 10);
}

TEST(MainTest, Decoding_MissingSymbols) {
  std::string output;
  int rc =
      RunBinary(R"({"mode": "decoding", "num_of_pucch_f2_bits": 2})", output);
  EXPECT_EQ(rc, 10);
}
TEST(MainTest, Decoding_InvalidSymbolFormat) {
  std::string output;
  int rc = RunBinary(
      R"({"mode": "decoding", "num_of_pucch_f2_bits": 2, "qpsk_symbols": ["invalid"]})",
      output);
  EXPECT_EQ(rc, 11);
}

TEST(MainTest, Decoding_Valid) {
  std::string output;
  std::string symbols =
      R"(["0.707+0.707j","-0.707-0.707j","0.707-0.707j","-0.707+0.707j","0.707+0.707j","0.707+0.707j","-0.707-0.707j","0.707-0.707j","-0.707+0.707j","0.707-0.707j"])";
  std::string input =
      R"({"mode": "decoding", "num_of_pucch_f2_bits": 2, "qpsk_symbols": )" +
      symbols + "}\n";

  int rc = RunBinary(
      R"({"mode": "decoding", "num_of_pucch_f2_bits": 2, "qpsk_symbols": )" +
          symbols + "}",
      output);
  EXPECT_EQ(rc, 0);

  json out = json::parse(output);
  EXPECT_EQ(out["mode"], "decoding");
  EXPECT_TRUE(out.contains("pucch_f2_bits"));
  EXPECT_EQ(out["pucch_f2_bits"].size(), 2);
}

TEST(MainTest, Sim_MissingIterations) {
  std::string output;
  int rc = RunBinary(
      R"({"mode": "channel simulation", "num_of_pucch_f2_bits": 2})", output);
  EXPECT_EQ(rc, 12);
}

TEST(MainTest, Sim_IterationsNotInteger) {
  std::string output;
  int rc = RunBinary(
      R"({"mode": "channel simulation", "num_of_pucch_f2_bits": 2, "iterations": "10"})",
      output);
  EXPECT_EQ(rc, 12);
}

TEST(MainTest, Sim_IterationsZero) {
  std::string output;
  int rc = RunBinary(
      R"({"mode": "channel simulation", "num_of_pucch_f2_bits": 2, "iterations": 0})",
      output);
  EXPECT_EQ(rc, 13);
}

TEST(MainTest, Sim_Valid_n2) {
  std::string output;
  int rc = RunBinary(
      R"({"mode": "channel simulation", "num_of_pucch_f2_bits": 2, "iterations": 10})",
      output);
  EXPECT_EQ(rc, 0);

  json out = json::parse(output);
  EXPECT_EQ(out["mode"], "channel simulation");
  EXPECT_TRUE(out.contains("bler"));
  EXPECT_TRUE(out.contains("success"));
  EXPECT_TRUE(out.contains("failed"));
  EXPECT_EQ(out["success"].get<int>() + out["failed"].get<int>(), 10);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}