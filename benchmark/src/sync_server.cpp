#include <cstdio>
#include <vector>
#include <memory>

#include <grpcpp/grpcpp.h>

#include "send_array.grpc.pb.h"
#include "serverlib.hpp"


void RunServer() {
    std::string server_address("0.0.0.0:50000");
    std::vector<std::unique_ptr<grpc::Service>> services;
    services.push_back(std::make_unique<send_array::ServiceImpl<send_array::SendArrayDouble>>());
    services.push_back(std::make_unique<send_array::ServiceImpl<send_array::SendArrayFloat>>());
    services.push_back(std::make_unique<send_array::ServiceImpl<send_array::SendArrayInt32>>());
    services.push_back(std::make_unique<send_array::ServiceImpl<send_array::SendArrayInt64>>());
    services.push_back(std::make_unique<send_array::ServiceImpl<send_array::SendArraySint32>>());
    services.push_back(std::make_unique<send_array::ServiceImpl<send_array::SendArraySint64>>());
    services.push_back(std::make_unique<send_array::ServiceImpl<send_array::SendArraySfixed32>>());
    services.push_back(std::make_unique<send_array::ServiceImpl<send_array::SendArraySfixed64>>());

    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    for(auto& service: services) {
        builder.RegisterService(service.get());
    }

    // Finally assemble the server.
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main() {
    RunServer();
    return EXIT_SUCCESS;
}
