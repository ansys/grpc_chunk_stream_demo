#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <grpcpp/grpcpp.h>

#include "send_array.grpc.pb.h"

#include "types_lookup.hpp"


namespace send_array {

template<typename GrpcType>
class ServiceImpl final: public GrpcType::Service {
    private:
        using data_type = typename TypesLookup<GrpcType>::data_type;
        using repeated_type = typename TypesLookup<GrpcType>::repeated_type;

        std::vector<std::vector<data_type>> data__;
        std::size_t data_index__;

    public:
        ServiceImpl(): data__(), data_index__(0) {}

        grpc::Status PopulateArray(
            grpc::ServerContext* context,
            const repeated_type* request,
            Empty *response
        ) override {
            data__.emplace_back(request->payload().cbegin(), request->payload().cend());
            return grpc::Status::OK;
        }

        grpc::Status ClearArrays(
            grpc::ServerContext* context,
            const Empty* request,
            Empty* response
        ) override {
            data__.clear();
            data_index__ = 0;
            return grpc::Status::OK;
        }

        grpc::Status DownloadArray(
            grpc::ServerContext* context,
            const Empty* request,
            repeated_type* response
        ) override {
            const auto & source_vec = data__[data_index__];
            response->mutable_payload()->Add(
                source_vec.cbegin(),
                source_vec.cend()
            );
            ++data_index__;
            return grpc::Status::OK;
        }

        grpc::Status DownloadArrayChunked(
            grpc::ServerContext* context,
            const StreamRequest* request,
            grpc::ServerWriter<repeated_type>* writer
        ) override {
            auto chunk_size = request->chunk_size();
            repeated_type chunk;
            const auto & source_vec = data__[data_index__];
            for(
                auto it=source_vec.cbegin();
                it < source_vec.cend();
                std::advance(it, chunk_size)
            ) {
                chunk.mutable_payload()->Clear();
                chunk.mutable_payload()->Add(it, std::min(it + chunk_size, source_vec.cend()));
                writer->Write(chunk);
            }
            ++data_index__;
            return grpc::Status::OK;
        }

        grpc::Status DownloadArrayBinaryChunked(
            grpc::ServerContext* context,
            const StreamRequest* request,
            grpc::ServerWriter<BinaryChunk>* writer
        ) override {
            auto chunk_size = request->chunk_size();

            const auto & source_vec = data__[data_index__];
            auto start = reinterpret_cast<const std::string*>(&source_vec.front());
            const auto end = reinterpret_cast<const std::string* const>(&source_vec.back());

            BinaryChunk chunk;
            // Add chunks that can be transmitted fully
            while(start <= end - chunk_size) {
                chunk.set_payload(start, chunk_size);
                writer-> Write(chunk);
                start += chunk_size;
            }
            // Add last partial chunk
            chunk.set_payload(start, end - start);
            writer-> Write(chunk);

            ++data_index__;
            return grpc::Status::OK;
        }
};

} // end namespace send_array
