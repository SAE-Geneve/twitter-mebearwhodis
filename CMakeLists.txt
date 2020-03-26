cmake_minimum_required(VERSION 3.14)

if(WIN32)
    add_compile_options("/std:c++latest")
    add_compile_options("/D _WIN32_WINNT=0x0601")
endif(WIN32)
set(CMAKE_CXX_STANDARD 17)

project(GrpcTwitter)

enable_testing()

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Gtest CONFIG REQUIRED)

add_library(GrpcLib
  STATIC
    ${PROJECT_SOURCE_DIR}/GrpcLib/Tweet.pb.h
    ${PROJECT_SOURCE_DIR}/GrpcLib/Tweet.pb.cc
    ${PROJECT_SOURCE_DIR}/GrpcLib/Tweet.grpc.pb.h
    ${PROJECT_SOURCE_DIR}/GrpcLib/Tweet.grpc.pb.cc
    ${PROJECT_SOURCE_DIR}/GrpcLib/Tweet_mock.grpc.pb.h
    ${PROJECT_SOURCE_DIR}/GrpcLib/Tweet.proto
)

target_link_libraries(GrpcLib
  PRIVATE
    protobuf::libprotoc
    protobuf::libprotobuf 
    protobuf::libprotobuf-lite
    gRPC::gpr 
    gRPC::grpc 
    gRPC::grpc++
)

add_library(TweetLib
  STATIC
    ${PROJECT_SOURCE_DIR}/TweetLib/Client.h
    ${PROJECT_SOURCE_DIR}/TweetLib/Client.cpp
    ${PROJECT_SOURCE_DIR}/TweetLib/Server.h
    ${PROJECT_SOURCE_DIR}/TweetLib/Server.cpp
    ${PROJECT_SOURCE_DIR}/TweetLib/Storage.h
    ${PROJECT_SOURCE_DIR}/TweetLib/Storage.cpp
)

target_link_libraries(TweetLib
  PRIVATE
    protobuf::libprotoc
    protobuf::libprotobuf 
    protobuf::libprotobuf-lite
    gRPC::gpr 
    gRPC::grpc 
    gRPC::grpc++
)

add_executable(TweetTest
    ${PROJECT_SOURCE_DIR}/TweetTest/main.cpp
    ${PROJECT_SOURCE_DIR}/TweetTest/StorageTest.h
    ${PROJECT_SOURCE_DIR}/TweetTest/StorageTest.cpp
)

target_link_libraries(TweetTest
  PUBLIC
    GrpcLib
    TweetLib
  PRIVATE
    GTest::gtest
    GTest::gtest_main
    GTest::gmock 
    GTest::gmock_main
    protobuf::libprotoc
    protobuf::libprotobuf 
    protobuf::libprotobuf-lite
    gRPC::gpr 
    gRPC::grpc 
    gRPC::grpc++
)

add_executable(TweetServer
    ${PROJECT_SOURCE_DIR}/TweetServer/main.cpp
)

target_link_libraries(TweetServer
  PUBLIC
    GrpcLib
    TweetLib
  PRIVATE
    protobuf::libprotoc
    protobuf::libprotobuf 
    protobuf::libprotobuf-lite
    gRPC::gpr 
    gRPC::grpc 
    gRPC::grpc++
)

add_executable(TweetClient
    ${PROJECT_SOURCE_DIR}/TweetClient/main.cpp
    ${PROJECT_SOURCE_DIR}/TweetClient/Command.h
    ${PROJECT_SOURCE_DIR}/TweetClient/Command.cpp
)

target_link_libraries(TweetClient
  PUBLIC
    GrpcLib
    TweetLib
  PRIVATE
    protobuf::libprotoc
    protobuf::libprotobuf 
    protobuf::libprotobuf-lite
    gRPC::gpr 
    gRPC::grpc 
    gRPC::grpc++
)
