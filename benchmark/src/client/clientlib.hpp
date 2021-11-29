#pragma once

#include <vector>
#include <memory>
#include <exception>

#include <grpcpp/grpcpp.h>

#include <send_array.grpc.pb.h>

#include <types_lookup.hpp>

namespace send_array {

template <typename GrpcType>
class ArrayServiceClient {
    private:
        using data_type = typename TypesLookup<GrpcType>::data_type;
        using single_message_type = typename TypesLookup<GrpcType>::single_message_type;
        using repeated_message_type = typename TypesLookup<GrpcType>::repeated_message_type;

        std::unique_ptr<typename GrpcType::Stub> stub_;
    public:
        ArrayServiceClient(
            std::shared_ptr<grpc::Channel> channel
        ): stub_(GrpcType::NewStub(channel)){}

        void PostArray(const std::vector<data_type> & array) {
            repeated_message_type request;
            request.mutable_payload()->Add(array.cbegin(), array.cend());
            Empty response;
            grpc::ClientContext context;
            auto status = stub_->PostArray(&context, request, &response);
            if(!status.ok()) {
                throw std::runtime_error(status.error_message());
            }
        }

        void DeleteArrays() {
            Empty request, response;
            grpc::ClientContext context;
            auto status = stub_->DeleteArrays(&context, request, &response);
            if(!status.ok()) {
                throw std::runtime_error(status.error_message());
            }
        }

        void GetArray(std::vector<data_type>& target) {
            Empty request;
            grpc::ClientContext context;
            repeated_message_type response;
            stub_->GetArray(&context, request, &response);
            for(const auto data: response.payload()) {
                target.push_back(data);
            }
        }

        void GetArrayStreaming(std::vector<data_type>& target) {
            Empty request;
            grpc::ClientContext context;
            std::unique_ptr<grpc::ClientReader<single_message_type>> reader(
                stub_->GetArrayStreaming(&context, request)
            );

            single_message_type message;
            while(reader->Read(&message)) {
                target.emplace_back(message.payload());
            }
        }

        void GetArrayChunked(std::vector<data_type>& target, const int32_t chunk_size) {
            StreamRequest request;
            request.set_chunk_size(chunk_size);
            grpc::ClientContext context;

            std::unique_ptr<grpc::ClientReader<repeated_message_type>> reader(
                stub_->GetArrayChunked(&context, request)
            );

            repeated_message_type chunk;
            while(reader->Read(&chunk)) {
                target.insert(
                    target.cend(),
                    chunk.payload().cbegin(),
                    chunk.payload().cend()
                );
            }
        }

        void GetArrayBinaryChunked(std::vector<data_type>& target, const int32_t chunk_size) {
            auto raw_target = reinterpret_cast<char *> (target.data());
            StreamRequest request;
            request.set_chunk_size(chunk_size);
            grpc::ClientContext context;
            BinaryChunk chunk;
            std::unique_ptr<grpc::ClientReader<BinaryChunk>> reader(
                stub_->GetArrayBinaryChunked(&context, request)
            );
            while(reader->Read(&chunk)) {
                #pragma GCC diagnostic push
                #pragma GCC diagnostic ignored "-Wclass-memaccess"
                memcpy(raw_target, chunk.payload().c_str(), chunk.payload().size());
                #pragma GCC diagnostic pop
                // The chunk_size is equal to 'chunk.payload().size()' except
                // possibly at the last iteration, when it no longer matters.
                raw_target += chunk_size;
            }
        }
};

} // end namespace send_array
