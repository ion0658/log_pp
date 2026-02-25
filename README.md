# log_pp

`log_pp` is a lightweight C++20 logging interface.

Features:

- structured records (`level`, `target`, `source location`, format string, format args)
- optional heterogeneous key-value fields
- runtime level filtering
- compile-time filtering (separate non-Release and Release settings)

## Requirements

- C++20 compiler
- CMake 3.15+

## Quick start

```cpp
#include <format>
#include <iostream>
#include "log.hpp"

struct ConsoleLogger : log_pp::BasicLogger<char> {
    bool enabled(const log_pp::BasicMetadata<char>&) const noexcept override {
        return true;
    }

    void log(const log_pp::BasicRecord<char>& record) override {
        std::cout << std::format("[{}] [{}] {}\n", record.get_level(),
                                 record.get_target(),
                                 std::vformat(record.get_format_string(),
                                              record.get_args()));
    }

    void flush() override { std::cout.flush(); }
};

int main() {
    static ConsoleLogger logger;
    log_pp::set_logger(logger);  // first call wins
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_INFO("hello {}", "world");
    LOG_PP_INFO({"api"}, "request {}", 42);
    LOG_PP_INFO({{"request_id", 42}, {"ok", true}}, "completed");
}
```

See also:

- `examples/simple_logger/main.cpp`
- `examples/local_logger/main.cpp`
- `examples/target_filter/main.cpp`
- `examples/comptime_filter/main.cpp`

## Build

```bash
cmake -S . -B build
cmake --build build
```

### Build with examples

```bash
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build --target simple_logger
```

### Build and run tests

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## API overview

- `log_pp::BasicLogger<CharT>`: logger interface (`enabled`, `log`, `flush`).
- `log_pp::ILogger`: alias of `log_pp::BasicLogger<char>`.
- `log_pp::set_logger(...)`: sets global logger once (first successful call wins).
- `log_pp::logger<CharT>()`: gets current global logger.
- `log_pp::set_max_level(...)` / `log_pp::max_level()`: runtime filter (`Trace` by default).
- `LOG_PP_TRACE/DEBUG/INFO/WARN/ERROR(...)`: macros that capture `std::source_location`.

`LOG_PP_*` supports both explicit logger and global logger forms.

Examples (explicit logger):

```cpp
LOG_PP_INFO(my_logger, {"target"}, {{"k", 1}}, "value={}", 1);
LOG_PP_INFO(my_logger, {"target"}, "value={}", 1);
LOG_PP_INFO(my_logger, {{"k", 1}}, "value={}", 1);
LOG_PP_INFO(my_logger, "value={}", 1);
```

Examples (global logger):

```cpp
LOG_PP_INFO({"target"}, {{"k", 1}}, "value={}", 1);
LOG_PP_INFO({"target"}, "value={}", 1);
LOG_PP_INFO({{"k", 1}}, "value={}", 1);
LOG_PP_INFO("value={}", 1);
```

## Compile-time filter options

Set at most one option in each group:

- non-Release group
  - `LOG_PP_LEVEL_FILTER_TRACE`
  - `LOG_PP_LEVEL_FILTER_DEBUG`
  - `LOG_PP_LEVEL_FILTER_INFO`
  - `LOG_PP_LEVEL_FILTER_WARN`
  - `LOG_PP_LEVEL_FILTER_ERROR`
- Release group
  - `LOG_PP_RELEASE_LEVEL_FILTER_TRACE`
  - `LOG_PP_RELEASE_LEVEL_FILTER_DEBUG`
  - `LOG_PP_RELEASE_LEVEL_FILTER_INFO`
  - `LOG_PP_RELEASE_LEVEL_FILTER_WARN`
  - `LOG_PP_RELEASE_LEVEL_FILTER_ERROR`

If multiple options in the same group are `ON`, CMake configuration fails.

Example:

```bash
cmake -S . -B build \
  -DLOG_PP_LEVEL_FILTER_DEBUG=ON \
  -DLOG_PP_RELEASE_LEVEL_FILTER_ERROR=ON
```

This keeps `DEBUG/INFO/WARN/ERROR` in non-Release and `ERROR` in Release.

You can also define the filter macros manually before including `log.hpp`.

## Key-value logging

`LOG_PP_*` accepts heterogeneous kv entries:

```cpp
LOG_PP_INFO({{"request_id", 42}, {"user", "alice"}, {"ratio", 1.25}},
            "processed {} items", 10);

LOG_PP_INFO({"api"},
            {{"status", "ok"}, {"elapsed_ms", 12.3}},
            "done: {}", true);

LOG_PP_INFO({{"id", 255, "0x{:04X}"}, {"ratio", 1.236, "{:.2f}"}},
            "custom formatted kv");
```

Supported entry forms:

- `{ "key", value }`
- `{ "key", value, "{fmt}" }`

## License

MIT License. See `LICENSE`.

