# HTTP Server (C++20)

A lightweight HTTP/1.1 server written in C++20 for macOS/BSD. Built from scratch to demonstrate low-level networking concepts: non-blocking I/O with **kqueue**, a custom **thread pool**, and RAII resource management throughout.

## Architecture

The project contains two server implementations that can be studied side-by-side:

| Component | Description |
|-----------|-------------|
| `EventLoop` | Single-threaded, non-blocking I/O using **kqueue** |
| `Server` | Multi-threaded using the custom `ThreadPool` |
| `ThreadPool` | Fixed-size worker thread pool with a task queue |
| `Socket` | RAII wrapper around a TCP listen socket |
| `ConnectionGuard` | RAII wrapper that closes a client fd on scope exit |
| `HttpRequest` | HTTP/1.1 request parser |
| `HttpResponse` | HTTP/1.1 response builder |
| `Logger` | Thread-safe console logger |

### Event-driven flow (`EventLoop`)

```
EventLoop (kqueue)
 ├── listen fd becomes readable → accept_new_connections()
 │    └── set non-blocking, register EVFILT_READ for client fd
 └── client fd becomes readable → handle_readable()
      ├── recv() until EAGAIN
      ├── wait for full "\r\n\r\n" header terminator
      ├── parse HttpRequest, build HttpResponse
      └── send response, close connection
```

## Requirements

- macOS (uses `kqueue` / `sys/event.h`)
- CMake ≥ 4.0
- C++20 compiler (Clang recommended)

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

For a debug build with AddressSanitizer and UBSan:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Run

```bash
./build/http_server
# Listening on port 8080
```

Test with curl:

```bash
curl -i http://localhost:8080/
```

## Project Structure

```
.
├── main.cpp              # Entry point — starts EventLoop on port 8080
├── EventLoop.{h,cpp}     # kqueue-based non-blocking event loop
├── Server.{h,cpp}        # Thread-pool-based server
├── ThreadPool.{h,cpp}    # Fixed-size thread pool
├── Socket.{h,cpp}        # TCP listen socket (RAII)
├── ConnectionGuard.{h}   # Client fd lifetime guard (RAII)
├── HttpRequest.{h,cpp}   # HTTP/1.1 request parser
├── HttpResponse.{h,cpp}  # HTTP/1.1 response serializer
├── Logger.{h}            # Thread-safe logger
├── utils.{h,cpp}         # Helpers (e.g. set_nonblocking)
└── CMakeLists.txt
```
