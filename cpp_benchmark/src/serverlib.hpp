#pragma once

#include <cstdint>
#include <vector>
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
        typename std::vector<std::vector<data_type>>::iterator data_iterator__;

    public:
        ServiceImpl(): data__(), data_iterator__(data__.begin()) {}

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
            data_iterator__ = data__.begin();
            return grpc::Status::OK;
        }

        grpc::Status DownloadArray(
            grpc::ServerContext* context,
            const Empty* request,
            repeated_type* response
        ) override {
            response->mutable_payload()->Add(
                data_iterator__->cbegin(),
                data_iterator__->cend()
            );
            ++data_iterator__;
            return grpc::Status::OK;
        }

        grpc::Status DownloadArrayChunked(
            grpc::ServerContext* context,
            const StreamRequest* request,
            grpc::ServerWriter<repeated_type>* writer
        ) override {
            auto chunk_size = request->chunk_size();
            repeated_type chunk;
            for(
                auto it=data_iterator__->cbegin();
                it < data_iterator__->cend();
                std::advance(it, chunk_size)
            ) {
                chunk.mutable_payload()->Clear();
                chunk.mutable_payload()->Add(it, std::min(it + chunk_size, data_iterator__->cend()));
                writer->Write(chunk);
            }
            ++data_iterator__;
            return grpc::Status::OK;
        }

        grpc::Status DownloadArrayBinaryChunked(
            grpc::ServerContext* context,
            const StreamRequest* request,
            grpc::ServerWriter<BinaryChunk>* writer
        ) override {
            auto chunk_size = request->chunk_size();

            auto start = reinterpret_cast<const char*>(&data_iterator__->front());
            const auto end = reinterpret_cast<const char* const>(&data_iterator__->back());

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

            ++data_iterator__;
            return grpc::Status::OK;
        }
};

} // end namespace send_array
