cmake_minimum_required(VERSION 3.8)
project(punjiCC)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp muaHandler.cpp muaHandler.h Log.cpp Log.h honeypotHandler.cpp honeypotHandler.h SocketIO.cpp SocketIO.h)
add_executable(punjiCC ${SOURCE_FILES} )

find_package(Threads)
target_link_libraries(punjiCC ${CMAKE_THREAD_LIBS_INIT} proxygenhttpserver folly glog curl)