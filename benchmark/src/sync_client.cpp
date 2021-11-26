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
    send_array::SendArrayClient<send_array::SendArraySfixed32> client(channel);

    client.PopulateArray({1, 2, 3});
    client.PopulateArray({4, 5, 6, 7});
    client.ClearArrays();
    client.PopulateArray({4, 5, 6, 7});
}
