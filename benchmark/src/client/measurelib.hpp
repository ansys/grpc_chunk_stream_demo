#include <cstdio>
#include <cstdint>
#include <vector>
#include <chrono>
#include <functional>

#include <grpcpp/grpcpp.h>

#include <types_lookup.hpp>
#include "send_array.grpc.pb.h"
#include "clientlib.hpp"

namespace send_array {

template<typename GrpcType>
int64_t measure_runtime(
    ArrayServiceClient<GrpcType>& client,
    std::function<void(std::vector<typename TypesLookup<GrpcType>::data_type> &)> array_getter,
    std::size_t vec_size,
    std::size_t num_repetitions,
    std::size_t num_preheat,
    std::function<void(std::vector<typename TypesLookup<GrpcType>::data_type> &, std::size_t)> array_filler
) {
    using vector_type = typename TypesLookup<GrpcType>::vector_type;

    auto num_vectors = num_repetitions + num_preheat;

    // Fill source vectors and send them to the server
    std::vector<vector_type> source_vectors(num_repetitions + num_preheat);
    client.DeleteArrays();
    for(auto &vec: source_vectors) {
        array_filler(vec, vec_size);
        client.PostArray(vec);
    }

    // Allocate target vectors
    std::vector<vector_type> target_vectors(num_vectors);
    for(auto &vec: target_vectors) {
        vec.reserve(vec_size);
    }

    // Preheat the connection
    for(std::size_t i = 0; i < num_preheat; ++i) {
        array_getter(target_vectors[i]);
    }

    // Run the measurement
    auto start = std::chrono::high_resolution_clock::now();
    for(std::size_t i = num_preheat; i < num_vectors; ++i) {
        array_getter(target_vectors[i]);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    int64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

    // Check correctness
    for(std::size_t i = 0; i < num_vectors; ++i) {
        for(std::size_t j = 0; j < vec_size; ++j) {
            if(source_vectors[i][j] != target_vectors[i][j]) {
                std::cout << "(i, j)=(" << i << ", " << j << ")" << std::endl;
                std::cout <<source_vectors[i][j] << "!=" << target_vectors[i][j] << std::endl;
                throw std::runtime_error("Results do not match the source vector.");
            }
        }
    }
    client.DeleteArrays();
    return duration;
}

struct Measurement {
    int64_t runtime;
    std::size_t num_repetitions;
    std::size_t vec_size;
    std::size_t chunk_size;
    std::size_t type_size;
    std::string type_id;
    std::string method_id;
};

void print_measurement(Measurement m) {
    std::cout <<
    m.runtime << ", " <<
    m.num_repetitions << ", " <<
    m.vec_size << ", " <<
    m.chunk_size << ", " <<
    m.type_size << ", " <<
    m.type_id << ", " <<
    m.method_id <<
    std::endl;
}
void print_measurement_header() {
    std::cout <<
    "runtime" << ", " <<
    "num_repetitions" << ", " <<
    "vec_size" << ", " <<
    "chunk_size" << ", " <<
    "type_size" << ", " <<
    "type_id" << ", " <<
    "method_id" <<
    std::endl;
}

template<typename GrpcType>
void run_all_measurements(
    ArrayServiceClient<GrpcType>& client,
    std::size_t num_measurements,
    std::size_t num_repetitions_per_measurement,
    int64_t max_time_microseconds,
    std::size_t max_vec_size
) {

    using vector_type = typename TypesLookup<GrpcType>::vector_type;

    std::map<std::string, std::function<void(vector_type&)>> func_without_chunking = {
        {
            "GetArray",
            [&client](auto & target_vec){client.GetArray(target_vec);}
        },
        {
            "GetArrayStreaming",
            [&client](auto & target_vec){client.GetArrayStreaming(target_vec);},
        }
    };
    std::map<std::string, std::function<void(vector_type&, std::size_t)>> func_with_chunking = {
        {
            "GetArrayChunked",
            [&client](auto & target_vec, std::size_t chunk_size){client.GetArrayChunked(target_vec, chunk_size);}
        },
        {
            "GetArrayBinaryChunked",
            [&client](auto & target_vec, std::size_t chunk_size){client.GetArrayBinaryChunked(target_vec, chunk_size);},
        }
    };


    for(auto pair: func_without_chunking) {
        auto method_id = pair.first;
        auto type_id = TypesLookup<GrpcType>::type_id;
        using data_type = typename TypesLookup<GrpcType>::data_type;
        bool cancel_measurements = false;
        for(std::size_t vec_size = 1; vec_size <= max_vec_size; vec_size <<=1 ) {
            if(cancel_measurements) break;
            for(std::size_t count = 0; count < num_measurements; ++count) {
                auto runtime = measure_runtime(
                    client,
                    pair.second,
                    vec_size,
                    num_repetitions_per_measurement,
                    2,
                    [](auto & vec, std::size_t vec_size){
                        vec.resize(vec_size);
                        std::fill(vec.begin(), vec.end(), 1);
                    }
                );
                print_measurement({
                    runtime,
                    num_repetitions_per_measurement,
                    vec_size,
                    0,
                    sizeof(data_type),
                    std::string(type_id),
                    method_id
                });
                if(runtime > max_time_microseconds) cancel_measurements = true;
            }
        }
    }
    for(auto pair: func_with_chunking) {
        auto method_id = pair.first;
        auto type_id = TypesLookup<GrpcType>::type_id;
        using data_type = typename TypesLookup<GrpcType>::data_type;


        bool cancel_measurements = false;
        // We use 'chunk_size' in the outer loop, because runtimes should (up to measurement
        // error) increase monotonically when the 'vec_size' increases. This makes it more
        // suited to define a breaking condition, even if we need to enforce 'chunk_size <= vec_size'
        // in a counterintuitive way
        for(std::size_t chunk_size = 1 << 8; chunk_size <= max_vec_size; chunk_size <<= 1) {
            for(std::size_t vec_size = chunk_size; vec_size <= max_vec_size; vec_size <<= 1) {
                if(cancel_measurements) break;
                auto array_getter = [pair, chunk_size](auto & vec){pair.second(vec, chunk_size);};
                for(std::size_t count = 0; count < num_measurements; ++count) {
                    auto runtime = measure_runtime(
                        client,
                        array_getter,
                        vec_size,
                        num_repetitions_per_measurement,
                        2,
                        [](auto & vec, std::size_t vec_size){
                            vec.resize(vec_size);
                            std::fill(vec.begin(), vec.end(), 1);
                        }
                    );
                    print_measurement({
                        runtime,
                        num_repetitions_per_measurement,
                        vec_size,
                        chunk_size,
                        sizeof(data_type),
                        std::string(type_id),
                        method_id
                    });
                    if(runtime > max_time_microseconds) cancel_measurements = true;
                }
            }
        }
    }
}

} // end of namespace send_array
