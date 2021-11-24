add_library(send_array STATIC send_array.pb.cc send_array.grpc.pb.cc)
target_include_directories(send_array PUBLIC .)
