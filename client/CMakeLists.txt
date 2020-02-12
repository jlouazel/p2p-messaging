cmake_minimum_required(VERSION 3.5)

project(client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(client main.cpp Client.cpp PeerServer.cpp ../common/IServer.cpp ../common/Tools.cpp)
target_link_libraries(client pthread)