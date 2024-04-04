#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <immintrin.h>
#include <iostream>
#include <vector>

// Modify as you like (MUST be a multiple of 32)
#define NUMBER_OF_ELEMENTS ((256 / 8) * 120)
#define NUMBER_OF_SUM_RUN 1000000

// Do not touch
#define SIZE_256 (NUMBER_OF_ELEMENTS / (256 / 8))

struct RegularStruct {
  uint8_t x;
  uint8_t y;
  uint8_t z;
};

struct StructOfArrays8Bits {
  std::vector<uint8_t> x_vec;
  std::vector<uint8_t> y_vec;
  std::vector<uint8_t> z_vec;
};

struct StructOfArrays256Bits {
  __m256i *x_vec;
  __m256i *y_vec;
  __m256i *z_vec;
};

uint64_t sum_aos(const std::vector<struct RegularStruct> &threeDimensions) {
  uint64_t sum = 0;
  for (uint64_t i = 0; i < NUMBER_OF_ELEMENTS; i++) {
    sum += threeDimensions[i].x;
  }
  return sum;
}

uint64_t sum_soa_8(const StructOfArrays8Bits &structOfArrays8Bits) {
  uint64_t sum = 0;
  for (uint64_t i = 0; i < NUMBER_OF_ELEMENTS; i++) {
    sum += structOfArrays8Bits.x_vec[i];
  }
  return sum;
}

uint64_t sum_simd(const StructOfArrays256Bits *structOfArrays256Bits) {
  __m256i sum =
      _mm256_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  for (uint64_t i = 0; i < SIZE_256; i++) {
    sum = _mm256_add_epi8(sum, structOfArrays256Bits->x_vec[i]);
  }

  uint64_t acc = 0;

  for (uint8_t i = 0; i < 256 / 8; i++)
    acc += ((uint8_t *)&sum)[i];

  return acc;
}

const std::vector<struct RegularStruct> initArrayOfStruct() {
  std::vector<struct RegularStruct> regularStruct;
  regularStruct.reserve(NUMBER_OF_ELEMENTS);

  for (uint64_t i = 0; i < NUMBER_OF_ELEMENTS; i++) {
    regularStruct[i].x = 2;
    regularStruct[i].y = 2;
    regularStruct[i].z = 3;
  }

  return regularStruct;
}

const struct StructOfArrays8Bits initStructOfArrays8Bits() {
  StructOfArrays8Bits structOfArrays8Bits;

  structOfArrays8Bits.x_vec.reserve(NUMBER_OF_ELEMENTS);
  structOfArrays8Bits.y_vec.reserve(NUMBER_OF_ELEMENTS);
  structOfArrays8Bits.z_vec.reserve(NUMBER_OF_ELEMENTS);

  for (uint64_t i = 0; i < NUMBER_OF_ELEMENTS; i++) {
    structOfArrays8Bits.x_vec[i] = 2;
    structOfArrays8Bits.y_vec[i] = 2;
    structOfArrays8Bits.z_vec[i] = 3;
  }

  return structOfArrays8Bits;
}

const struct StructOfArrays256Bits *initStructOfArrays256Bits() {
  StructOfArrays256Bits *structOfArrays256Bits =
      (struct StructOfArrays256Bits *)malloc(sizeof(StructOfArrays256Bits));

  structOfArrays256Bits->x_vec = (__m256i *)malloc(sizeof(__m256i) * SIZE_256);
  structOfArrays256Bits->y_vec = (__m256i *)malloc(sizeof(__m256i) * SIZE_256);
  structOfArrays256Bits->z_vec = (__m256i *)malloc(sizeof(__m256i) * SIZE_256);

  for (uint64_t i = 0; i < SIZE_256; i++) {
    structOfArrays256Bits->x_vec[i] =
        _mm256_setr_epi8(2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2);
    structOfArrays256Bits->y_vec[i] =
        _mm256_setr_epi8(2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2);
    structOfArrays256Bits->z_vec[i] =
        _mm256_setr_epi8(3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3);
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
  const StructOfArrays8Bits structOfArrays8Bits = initStructOfArrays8Bits();

  const auto init_soa_8 = high_resolution_clock::now();

  uint64_t x_soa_8 = 0;
  for (uint64_t i = 0; i < NUMBER_OF_SUM_RUN; i++)
    x_soa_8 += sum_soa_8(structOfArrays8Bits);

  const auto sum_soa_8 = high_resolution_clock::now();

  // Struct of arrays, no SIMD
  const StructOfArrays256Bits *structOfArrays256Bits =
      initStructOfArrays256Bits();

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
            << std::endl
            << std::endl;

  std::cout << "Structure of arrays without SIMD" << std::endl
            << "Initialization="
            << duration_cast<milliseconds>(init_soa_8 - sum_aos).count()
            << "ms sum="
            << duration_cast<milliseconds>(sum_soa_8 - init_soa_8).count()
            << "ms" << std::endl
            << std::endl;

  std::cout << "Structure of arrays with SIMD" << std::endl
            << "Initialization="
            << duration_cast<milliseconds>(init_soa_256 - sum_soa_8).count()
            << "ms sum="
            << duration_cast<milliseconds>(sum_soa_256 - init_soa_256).count()
            << "ms" << std::endl
            << std::endl;

  if (x_aos != x_soa_256 || x_soa_8 != x_soa_256) {
    std::cout << "Test failed. Results are different x_aos=" << x_aos
              << " x_soa_32=" << x_soa_8 << " x_soa_256=" << x_soa_256
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Test succeeded (all values are the same). Result is " << x_aos
            << "." << std::endl;

  return 0;
}
