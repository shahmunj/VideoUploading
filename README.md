# VideoUploading

A video hosting platform built from scratch — including a **hand-built database engine in C++**, with no SQLite, no ORM, and (eventually) no third-party HTTP library.

The goal of this project isn't just to ship a working app — it's to actually understand what's happening under the hood of the tools most developers use without a second thought: how a database stores and retrieves data, and how an HTTP server actually communicates over a network.

## Why build this from scratch?

Most backend projects reach for SQLite, Postgres, or an ORM, and never think twice about what's happening underneath. This project intentionally skips that shortcut. Every layer — from turning an object into raw bytes, to indexing records for fast lookup, to eventually parsing raw HTTP requests — is being built and understood from first principles.

## Current Status

### ✅ Done
- **Data model** — `Video`, `Media_properties`, and `Timestamps` structs/classes, with safe default values
- **Binary serialization / deserialization** — converts a `Video` object into a compact byte stream (and back), using:
  - Length-prefixed encoding for variable-length fields (strings)
  - Fixed-size encoding for numeric fields and enums, via C++ templates
  - Manual byte-level manipulation with `reinterpret_cast` and pointer arithmetic
- **Test suite** — round-trip correctness tests, edge cases (empty strings, long strings, boundary values), and hand-verified byte-count checks
- **Build system** — CMake, with a separate test executable

### 🚧 In Progress / Planned
- **Pager** — page-based file I/O with in-memory caching, so the database doesn't rewrite the entire file on every write
- **B-Tree** — the indexing structure that makes lookups fast (O(log n)) instead of scanning every record — the same core structure real databases like SQLite use internally
- **C++ API layer** — `insert`, `findById`, `delete`, wrapping the B-Tree
- **HTTP server, built from scratch** — raw TCP socket handling and manual HTTP request/response parsing, no library
- **JSON layer** — for the HTTP API to communicate with clients
- **Video streaming support** — HTTP Range requests for progressive playback and seeking
- **Write-Ahead Logging (WAL)** — crash safety, so a mid-write crash can't corrupt the database
- **Concurrency control** — safe handling of simultaneous reads/writes

## Architecture

```
Video object (C++ struct)
      │  serialize / deserialize
      ▼
Pager + B-Tree (custom storage engine)
      │
      ▼
Your own database file (.dat)
      │
      ▼
C++ HTTP server (built from scratch)
      │
      ▼
API endpoints (POST /videos, GET /videos/:id, streaming, etc.)
```

## Tech Stack

- **Language:** C++ (C++17)
- **Build system:** CMake
- **No external database or HTTP libraries** — storage engine and server are hand-built

## Building & Running

```bash
mkdir build
cd build
cmake ..
make
./main
```

## Running Tests

```bash
cd build
./tests
```

## Project Structure

```
VideoUploading/
├── Database/
│   ├── video.hpp                    # Data model
│   ├── video_serializer.hpp/.cpp    # Binary serialization layer
│   ├── tests/
│   │   └── test_serializer.cpp      # Test suite
│   └── main.cpp
├── Server/                           # (planned) hand-built HTTP/socket server
├── Frontend/
├── CMakeLists.txt
└── README.md
```

## License

See [LICENSE](./LICENSE).
