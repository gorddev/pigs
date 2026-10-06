#pragma once

// creates a random number from a given state.
inline uint64_t fast_rand(uint64_t& state) {
    state += 0xa0761d6478bd642fULL;
    __uint128_t val = (__uint128_t)state * 0xe7037ed1a0b428dbULL;
    return (val >> 64) ^ val;
}

// fills a c-style array with random numbers.
inline void fill_random_range(std::span<uint8_t> arr, uint8_t min, uint8_t max) {
    uint64_t state = 123456789ULL;
    uint32_t range = (max - min) + 1;

    size_t i = 0;

    for (; i <= arr.size() - 8; i += 8) {
        uint64_t rand64 = fast_rand(state);
        arr[i + 0] = min + (uint8_t)(((uint32_t)(uint8_t)(rand64 >> 0)  * range) >> 8);
        arr[i + 1] = min + (uint8_t)(((uint32_t)(uint8_t)(rand64 >> 8)  * range) >> 8);
        arr[i + 2] = min + (uint8_t)(((uint32_t)(uint8_t)(rand64 >> 16) * range) >> 8);
        arr[i + 3] = min + (uint8_t)(((uint32_t)(uint8_t)(rand64 >> 24) * range) >> 8);
        arr[i + 4] = min + (uint8_t)(((uint32_t)(uint8_t)(rand64 >> 32) * range) >> 8);
        arr[i + 5] = min + (uint8_t)(((uint32_t)(uint8_t)(rand64 >> 40) * range) >> 8);
        arr[i + 6] = min + (uint8_t)(((uint32_t)(uint8_t)(rand64 >> 48) * range) >> 8);
        arr[i + 7] = min + (uint8_t)(((uint32_t)(uint8_t)(rand64 >> 56) * range) >> 8);
    }
    if (i < arr.size()) {
        uint64_t rand64 = fast_rand(state);
        for (int j = 0; i < arr.size(); ++i, ++j) {
            arr[i] = min + (uint8_t)(((uint32_t)(uint8_t)(rand64 >> (j * 8)) * range) >> 8);
        }
    }
}
