# C-KVStore ⚡
[![C/C++ CI](https://github.com/Marioisthebekst/C-Kvstore/actions/workflows/ci.yml/badge.svg)](https://github.com/Marioisthebekst/C-Kvstore/actions/workflows/ci.yml)

A lightweight, high-performance in-memory key-value store implemented in C (C99). It features a dynamic hash table with separate chaining for collision resolution, TTL-based key expiration, an interactive shell (REPL), and file-backed persistence.

---

## 🚀 Features

- **Hash Table Architecture:** O(1) average-time complexity for core operations (`SET`, `GET`, `DELETE`) with linked-list collision resolution.
- **Dynamic Resizing:** Automated rehashing and capacity scaling (grow and shrink) to maintain an optimal load factor.
- **Key Expiration (TTL):** Set keys with a relative time-to-live or an absolute Unix timestamp; expired keys are lazily evicted on access and periodically swept from the LRU tail during REPL activity.
- **LRU Memory Eviction:** The store can be created with a maximum item capacity; once exceeded, the least recently used key is automatically evicted to make room, tracked via an O(1) doubly linked list touched on every read and write.
- **Atomic Counters:** `INCR` / `DECR` for integer-valued keys, Redis-style.
- **Interactive REPL:** Built-in command interpreter supporting quoted strings for values containing spaces.
- **Disk Persistence:** Safe file serialization and deserialization via `SAVE` and `LOAD` commands, TTL included.
- **Memory Safety:** Strict allocation checks and complete deallocation routines, verified leak-free under AddressSanitizer.

---

## 💻 REPL Commands

| Command | Syntax | Description | Example |
| :--- | :--- | :--- | :--- |
| `SET` | `SET <key> <value>` | Stores or updates a key. Multi-word strings must be wrapped in quotes. | `SET name "Alice Doe"` |
| `GET` | `GET <key>` | Retrieves the value for the given key. Returns `NULL` if not found. | `GET name` |
| `DELETE` | `DELETE <key>` | Removes the key-value pair from the store. | `DELETE name` |
| `SETEX` | `SETEX <key> <value> <seconds>` | Sets a key with a relative TTL (in seconds). | `SETEX session abc123 30` |
| `SETAT` | `SETAT <key> <value> <timestamp>` | Sets a key with an absolute Unix timestamp expiry. | `SETAT name "Alice Doe" 1735689600` |
| `EXIST` | `EXIST <key>` | Checks if a key exists and has not expired. | `EXIST name` |
| `INCR` | `INCR <key>` | Increments the integer value of a key by 1 (creates it at 0 if missing). | `INCR score` |
| `DECR` | `DECR <key>` | Decrements the integer value of a key by 1. | `DECR score` |
| `COUNT` | `COUNT` | Shows the total number of items currently in the table. | `COUNT` |
| `KEYS` | `KEYS` | Lists all valid (non-expired) keys in the database. | `KEYS` |
| `SAVE` | `SAVE <filename>` | Serializes all stored data (including TTLs) into a file on disk. | `SAVE dump.txt` |
| `LOAD` | `LOAD <filename>` | Restores data from an existing save file. | `LOAD dump.txt` |
| `EXIT` | `EXIT` | Frees all heap memory and exits the REPL. | `EXIT` |

### Example Session

```
Key-Value Store initialized. Type commands (e.g., SET, GET, KEYS, EXIT):
> SET name "Alice Doe"
Key Set Successfully!
> SETEX session abc123 30
Key Set with TTL Successfully!
> INCR visits
(integer) 1
> GET name
Alice Doe
> KEYS
Key: name
Key: session
Key: visits
> SAVE dump.txt
Table saved successfully!
> EXIT
```

---

## 🧠 LRU Eviction

The store is created with `createTable(size, capacity)`: `size` is the initial hash table capacity, and `capacity` is the maximum number of live keys the store will hold. Once the store is full, inserting a new key evicts the least recently used one first — both `GET` and `SET` count as a "use" and move a key back to the front of the eviction order. Pass `capacity <= 0` for an unbounded store (the default used throughout the test suite). The running key-value store (`src/main.c`) currently starts with `createTable(5, 6)` — a max of 6 live keys — as a demonstration; adjust this call to change the limit.

---

## 📂 Project Structure

```
c-kvstore/
├── src/
│   ├── db.c        - Core hash table engine: insert, get, delete, resize/shrink, TTL logic
│   ├── repl.c       - REPL parsing, quote handling, command dispatch
│   └── main.c       - Entry point for the interactive store
├── include/
│   ├── db.h
│   └── repl.h
├── tests/
│   ├── test.c        - Unit and integration test suite
│   ├── test.h
│   └── testMain.c    - Test runner entry point (kept separate from src/main.c)
├── Makefile
├── .gitignore
└── README.md
```

---

## 🛠️ Building and Running

### Prerequisites

- GCC compiler supporting C99 or later
- GNU Make (recommended)

### Running the Key-Value Store

```
make all
./kv_store
```

### Running the Test Suite

```
make test
```

`make test` builds and runs the suite in one step (matching what CI does). Or compiling manually with GCC:

```
gcc -Wall -Wextra -std=c99 -Iinclude -Itests src/db.c src/repl.c tests/test.c tests/testMain.c -o test_runner
./test_runner
```

### Cleaning Build Artifacts

```
make clean
```

---

## 🗺️ Roadmap

- [x] KEYS command to enumerate all stored keys
- [x] Time-To-Live (TTL) key expiration (SETEX / SETAT)
- [x] INCR / DECR atomic counters
- [x] Automated CI pipeline via GitHub Actions
- [x] LRU memory eviction policy
- [ ] Lightweight socket-based TCP server interface
- [ ] LFU memory eviction policy