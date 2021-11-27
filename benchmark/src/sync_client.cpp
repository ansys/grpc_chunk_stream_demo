#include <cstdio>

#include <grpcpp/grpcpp.h>

#include "send_array.grpc.pb.h"
#include "types_lookup.hpp"
#include "clientlib.hpp"


int main() {
    grpc::ChannelArguments args;
    args.SetMaxReceiveMessageSize(-1);
    std::string target_str = "localhost:50000";
    auto channel = grpc::CreateCustomChannel(
        target_str,
        grpc::InsecureChannelCredentials(),
        args
    );

    // create client
    send_array::ArrayServiceClient<send_array::ArrayServiceSfixed32> client(channel);

    using vec_type = std::vector<send_array::TypesLookup<send_array::ArrayServiceSfixed32>::data_type>;
    vec_type vec = {1, 2, 3, 4};
    client.PostArray(vec);

    vec_type target;
    target.reserve(vec.size());
    client.GetArray(target);
    for(std::size_t i=0; i < vec.size(); ++i) {
        std::cout << vec[i] << " " << target[i] << std::endl;
    }
    target.clear();
    target.reserve(vec.size());
    client.GetArrayStreaming(target);
    for(std::size_t i=0; i < vec.size(); ++i) {
        std::cout << vec[i] << " " << target[i] << std::endl;
    }
    target.clear();
    target.reserve(vec.size());
    client.GetArrayChunked(target, 4);
    for(std::size_t i=0; i < vec.size(); ++i) {
        std::cout << vec[i] << " " << target[i] << std::endl;
    }
    target.clear();
    target.reserve(vec.size());
    client.GetArrayBinaryChunked(target, 3);
    for(std::size_t i=0; i < vec.size(); ++i) {
        std::cout << vec[i] << " " << target[i] << std::endl;
    }
}
