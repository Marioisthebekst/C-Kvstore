# C-KVStore ⚡

A lightweight, high-performance in-memory key-value store implemented in C (C99). It features a dynamic hash table with separate chaining for collision resolution, TTL-based key expiration, an interactive shell (REPL), and file-backed persistence.

---

## 🚀 Features

- **Hash Table Architecture:** O(1) average-time complexity for core operations (`SET`, `GET`, `DELETE`) with linked-list collision resolution.
- **Dynamic Resizing:** Automated rehashing and capacity scaling (grow and shrink) to maintain an optimal load factor.
- **Key Expiration (TTL):** Set keys with a relative time-to-live or an absolute Unix timestamp; expired keys are lazily evicted on access.
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
./kv_store.exe
```

### Running the Test Suite

```
make test
./test_runner.exe
```

Or compiling manually with GCC:

```
gcc -Wall -Wextra -std=c99 -Iinclude -Itests src/db.c src/repl.c tests/test.c tests/testMain.c -o test_runner.exe
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
- [ ] Automated CI pipeline via GitHub Actions
- [ ] Lightweight socket-based TCP server interface
- [ ] LRU/LFU memory eviction policies