#include <chrono>
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <vector>

#define SIZE_256 3000000
#define SIZE_32 SIZE_256 * (256 / 32)

#define NUMBER_OF_SUM_RUN 100

struct RegularStruct {
  uint32_t x;
  uint32_t y;
  uint32_t z;
};

struct StructOfArrays {
  std::vector<__m256i> x_vec;
  std::vector<__m256i> y_vec;
  std::vector<__m256i> z_vec;
};

uint64_t sum(const std::vector<struct RegularStruct> &threeDimensions) {
  uint64_t sum = 0;
  for (uint64_t i = 0; i < SIZE_32; i++) {
    sum += threeDimensions[i].x;
  }
  return sum;
}

uint64_t sum_2(const StructOfArrays &structOfArrays) {
  uint64_t zeroes[4] = {0, 0, 0, 0};

  // In loadu the u stands for unaligned because pointers by defaults
  // are not aligned
  __m256i sum = _mm256_loadu_si256((__m256i *)zeroes);

  for (uint64_t i = 0; i < SIZE_256; i++) {
    sum = _mm256_add_epi32(sum, structOfArrays.x_vec[i]);
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

const struct StructOfArrays initStructOfArrays() {
  StructOfArrays structOfArrays;
  structOfArrays.x_vec.reserve(SIZE_256);
  structOfArrays.y_vec.reserve(SIZE_256);
  structOfArrays.z_vec.reserve(SIZE_256);

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
    structOfArrays.x_vec[i] = _mm256_loadu_si256((__m256i const *)x_array);
    structOfArrays.x_vec[i] = _mm256_loadu_si256((__m256i const *)y_array);
    structOfArrays.x_vec[i] = _mm256_loadu_si256((__m256i const *)z_array);
  }

  return structOfArrays;
}

int main(int argc, char *argv[]) {
  using namespace std::chrono;

  const auto begin = high_resolution_clock::now();

  const std::vector<struct RegularStruct> arrayOfStruct = initArrayOfStruct();

  const auto reserve_1 = high_resolution_clock::now();

  uint64_t x1 = 0;
  for (uint64_t i = 0; i < NUMBER_OF_SUM_RUN; i++)
    x1 += sum(arrayOfStruct);

  const auto sum_1 = high_resolution_clock::now();

  const StructOfArrays structOfArrays = initStructOfArrays();

  const auto reserve_2 = high_resolution_clock::now();

  uint64_t x2 = 0;
  for (int i = 0; i < NUMBER_OF_SUM_RUN; i++)
    x2 += sum_2(structOfArrays);

  const auto sum_2 = high_resolution_clock::now();

  std::cout << "reserve_1="
            << duration_cast<milliseconds>(reserve_1 - begin).count()
            << "ms sum_1="
            << duration_cast<milliseconds>(sum_1 - reserve_1).count() << "ms"
            << std::endl;

  std::cout << "reserve_2="
            << duration_cast<milliseconds>(reserve_2 - sum_1).count()
            << "ms sum_2="
            << duration_cast<milliseconds>(sum_2 - reserve_2).count() << "ms"
            << std::endl;

  if (x1 != x2)
    std::cout << " x1 != x2, x1=" << x1 << " x2=" << x2 << std::endl;

  return 0;
}
