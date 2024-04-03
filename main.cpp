#include <chrono>
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <vector>

// Modify as you like
#define SIZE_256 3000000
#define NUMBER_OF_SUM_RUN 100

// Do not touch
#define SIZE_32 SIZE_256 * (256 / 32)

struct RegularStruct {
  uint32_t x;
  uint32_t y;
  uint32_t z;
};

struct StructOfArrays32Bits {
  std::vector<uint32_t> x_vec;
  std::vector<uint32_t> y_vec;
  std::vector<uint32_t> z_vec;
};

struct StructOfArrays256Bits {
  std::vector<__m256i> x_vec;
  std::vector<__m256i> y_vec;
  std::vector<__m256i> z_vec;
};

uint64_t sum_aos(const std::vector<struct RegularStruct> &threeDimensions) {
  uint64_t sum = 0;
  for (uint64_t i = 0; i < SIZE_32; i++) {
    sum += threeDimensions[i].x;
  }
  return sum;
}

uint64_t sum_soa_32(const StructOfArrays32Bits &structOfArrays32Bits) {
  uint64_t sum = 0;
  for (uint64_t i = 0; i < SIZE_32; i++) {
    sum += structOfArrays32Bits.x_vec[i];
  }
  return sum;
}

uint64_t sum_simd(const StructOfArrays256Bits &structOfArrays256Bits) {
  uint64_t zeroes[4] = {0, 0, 0, 0};

  // In loadu the u stands for unaligned because pointers by defaults
  // are not aligned
  __m256i sum = _mm256_loadu_si256((__m256i *)zeroes);

  for (uint64_t i = 0; i < SIZE_256; i++) {
    sum = _mm256_add_epi32(sum, structOfArrays256Bits.x_vec[i]);
  }

  uint32_t final_sum[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  // There are probably better ways to do it like lateral sum
  _mm256_storeu_si256((__m256i *)final_sum, sum);

  uint64_t acc = 0;
  for (uint8_t i = 0; i < 8; i++)
    acc += final_sum[i];

  return acc;
}

const std::vector<struct RegularStruct> initArrayOfStruct() {
  std::vector<struct RegularStruct> regularStruct;
  regularStruct.reserve(SIZE_32);

  for (uint64_t i = 0; i < SIZE_32; i++) {
    regularStruct[i].x = (2);
    regularStruct[i].y = (2);
    regularStruct[i].z = (3);
  }

  return regularStruct;
}

const struct StructOfArrays32Bits initStructOfArrays32Bits() {
  StructOfArrays32Bits structOfArrays32Bits;

  structOfArrays32Bits.x_vec.reserve(SIZE_32);
  structOfArrays32Bits.y_vec.reserve(SIZE_32);
  structOfArrays32Bits.z_vec.reserve(SIZE_32);

  for (uint64_t i = 0; i < SIZE_32; i++) {
    structOfArrays32Bits.x_vec[i] = (2);
    structOfArrays32Bits.y_vec[i] = (2);
    structOfArrays32Bits.z_vec[i] = (3);
  }

  return structOfArrays32Bits;
}

const struct StructOfArrays256Bits initStructOfArrays256Bits() {
  StructOfArrays256Bits structOfArrays256Bits;
  structOfArrays256Bits.x_vec.reserve(SIZE_256);
  structOfArrays256Bits.y_vec.reserve(SIZE_256);
  structOfArrays256Bits.z_vec.reserve(SIZE_256);

  uint32_t x_array[256 / 32];
  for (uint64_t i = 0; i < 256 / 32; i++)
    x_array[i] = 2;

  uint32_t y_array[256 / 32];
  for (uint64_t i = 0; i < 256 / 32; i++)
    y_array[i] = 2;

  uint32_t z_array[256 / 32];
  for (uint64_t i = 0; i < 256 / 32; i++)
    z_array[i] = 2;

  for (uint64_t i = 0; i < SIZE_256; i++) {
    structOfArrays256Bits.x_vec[i] = _mm256_loadu_si256((__m256i const *)x_array);
    structOfArrays256Bits.y_vec[i] = _mm256_loadu_si256((__m256i const *)y_array);
    structOfArrays256Bits.z_vec[i] = _mm256_loadu_si256((__m256i const *)z_array);
  }

  return structOfArrays256Bits;
}

int main(int argc, char *argv[]) {
  using namespace std::chrono;

  const auto begin = high_resolution_clock::now();

  // Array of struct, no SIMD
  const std::vector<struct RegularStruct> arrayOfStruct = initArrayOfStruct();

  const auto init_aos = high_resolution_clock::now();

  uint64_t x_aos = 0;
  for (uint64_t i = 0; i < NUMBER_OF_SUM_RUN; i++)
    x_aos += sum_aos(arrayOfStruct);

  const auto sum_aos = high_resolution_clock::now();

  // Struct of arrays, no SIMD
  const StructOfArrays32Bits structOfArrays32Bits = initStructOfArrays32Bits();

  const auto ini_soa_32 = high_resolution_clock::now();

  uint64_t x_soa_32 = 0;
  for (uint64_t i = 0; i < NUMBER_OF_SUM_RUN; i++)
    x_soa_32 += sum_soa_32(structOfArrays32Bits);

  const auto sum_soa_32 = high_resolution_clock::now();

  // Struct of arrays, no SIMD
  const StructOfArrays256Bits structOfArrays256Bits = initStructOfArrays256Bits();

  const auto init_soa_256 = high_resolution_clock::now();

  uint64_t x_soa_256 = 0;
  for (int i = 0; i < NUMBER_OF_SUM_RUN; i++)
    x_soa_256 += sum_simd(structOfArrays256Bits);

  const auto sum_soa_256 = high_resolution_clock::now();

  std::cout << "Array of structures " << std::endl
            << "Initialization="
            << duration_cast<milliseconds>(init_aos - begin).count()
            << "ms sum="
            << duration_cast<milliseconds>(sum_aos - init_aos).count() << "ms"
            << std::endl << std::endl;

  std::cout << "Structure of arrays without SIMD" << std::endl
            << "Initialization="
            << duration_cast<milliseconds>(ini_soa_32 - sum_aos).count()
            << "ms sum="
            << duration_cast<milliseconds>(sum_soa_32 - ini_soa_32).count() << "ms"
            << std::endl << std::endl;

  std::cout << "Structure of arrays with SIMD" << std::endl
            << "Initialization="
            << duration_cast<milliseconds>(init_soa_256 - sum_aos).count()
            << "ms sum="
            << duration_cast<milliseconds>(sum_soa_256 - init_soa_256).count() << "ms"
            << std::endl << std::endl;

  if (x_aos != x_soa_256 || x_soa_32 != x_soa_256) {
    std::cout << "Test failed. Results are different x_aos=" << x_aos << " x_soa_32=" << x_soa_32 << " x_soa_256=" << x_soa_256 << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Test succeeded (all values are the same). Result is " << x_aos << "." << std::endl;

  return 0;
}
