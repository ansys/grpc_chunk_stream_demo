#pragma once

#include <cstdint>

#include "send_array.pb.h"
#include "send_array.grpc.pb.h"

namespace send_array {

template <typename GrpcType> struct TypesLookup;

template<>
struct TypesLookup<SendArrayDouble>{
    using data_type = double;
    using repeated_type = RepeatedDouble;
};

template<>
struct TypesLookup<SendArrayFloat>{
    using data_type = float;
    using repeated_type = RepeatedFloat;
};

template<>
struct TypesLookup<SendArrayInt32>{
    using data_type = int32_t;
    using repeated_type = RepeatedInt32;
};

template<>
struct TypesLookup<SendArrayInt64>{
    using data_type = int64_t;
    using repeated_type = RepeatedInt64;
};

template<>
struct TypesLookup<SendArraySint32>{
    using data_type = int32_t;
    using repeated_type = RepeatedSint32;
};

template<>
struct TypesLookup<SendArraySint64>{
    using data_type = int64_t;
    using repeated_type = RepeatedSint64;
};

template<>
struct TypesLookup<SendArraySfixed32>{
    using data_type = int32_t;
    using repeated_type = RepeatedSfixed32;
};

template<>
struct TypesLookup<SendArraySfixed64>{
    using data_type = int64_t;
    using repeated_type = RepeatedSfixed64;
};

} // end namespace send_array
