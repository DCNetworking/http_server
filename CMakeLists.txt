cmake_minimum_required(VERSION 4.0)
project(http_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -fno-omit-frame-pointer")

add_executable(http_server
        main.cpp
        Socket.cpp
        Server.cpp
        HttpRequest.cpp
        HttpResponse.cpp
        Logger.cpp
        Logger.h
        ConnectionGuard.cpp
        ConnectionGuard.h
        ThreadPool.cpp
        ThreadPool.h
        utils.cpp
        utils.h
        EventLoop.cpp
        EventLoop.h
)
