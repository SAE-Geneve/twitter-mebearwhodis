cmake_minimum_required(VERSION 3.14)

if(WIN32)
    add_compile_options("/std:c++latest")
    add_compile_options("/D _WIN32_WINNT=0x0603")
endif(WIN32)
set(CMAKE_CXX_STANDARD 17)

project(GrpcTwitter)

enable_testing()

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Gtest CONFIG REQUIRED)

add_library(GrpcLib
  STATIC
    GrpcLib/Tweet.pb.h
    GrpcLib/Tweet.pb.cc
    GrpcLib/Tweet.grpc.pb.h
    GrpcLib/Tweet.grpc.pb.cc
    GrpcLib/Tweet.proto
)

target_link_libraries(GrpcLib
  PUBLIC
    protobuf::libprotoc
    protobuf::libprotobuf 
    protobuf::libprotobuf-lite
    gRPC::gpr 
    gRPC::grpc 
    gRPC::grpc++
)

add_library(TweetLib
  STATIC
    TweetLib/Client.h
    TweetLib/Client.cpp
    TweetLib/Server.h
    TweetLib/Server.cpp
    TweetLib/Storage.h
    TweetLib/Storage.cpp
)

target_link_libraries(TweetLib
  PUBLIC
    GrpcLib
)

add_executable(TweetTest
    TweetTest/main.cpp
    TweetTest/StorageTest.h
    TweetTest/StorageTest.cpp
)

target_link_libraries(TweetTest
  PUBLIC
    GrpcLib
    TweetLib
    GTest::gtest
    GTest::gtest_main
)

add_test(NAME TweetTest COMMAND TweetTest)

add_executable(TweetServer
    TweetServer/main.cpp
)

target_link_libraries(TweetServer
  PUBLIC
    GrpcLib
    TweetLib
)

add_executable(TweetClient
    TweetClient/main.cpp
    TweetClient/Command.h
    TweetClient/Command.cpp
)

target_link_libraries(TweetClient
  PUBLIC
    GrpcLib
    TweetLib
)
