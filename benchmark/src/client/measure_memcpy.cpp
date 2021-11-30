#include <cstdint>

#include <random>
#include <vector>
#include <chrono>
#include <iostream>

int main() {
    std::random_device rnd_device;
    std::mt19937 mersenne_engine {rnd_device()};
    std::uniform_int_distribution<int32_t> dist;

    std::size_t size_pow = 28;
    std::size_t size = std::size_t(1) << size_pow;
    uint16_t repetitions = 15;

    std::vector<std::vector<int32_t>> source_vectors;
    for(std::size_t i = 0; i < repetitions; ++i) {
        source_vectors.emplace_back();
        for(std::size_t j = 0; j < size; ++j) {
            source_vectors[i].push_back(dist(mersenne_engine));
        }
    }

    std::vector<std::vector<int32_t>> target_vectors;
    for(std::size_t i = 0; i < repetitions; ++i) {
        target_vectors.emplace_back();
        target_vectors[i].reserve(1<<25);
    }
    auto start = std::chrono::high_resolution_clock::now();
    for(std::size_t i = 0; i < repetitions; ++i) {
        for(std::size_t j = 0; j < size; ++j) {
            target_vectors[i].push_back(source_vectors[i][j]);
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();

    for(std::size_t i = 0; i < repetitions; ++i) {
        for(std::size_t j = 0; j < size; ++j) {
            if(source_vectors[i][j] != target_vectors[i][j]) {
                std::cout << "fail!";
            }
        }
    }
    int64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

    std::cout << "duration: " << duration << std::endl;
}
