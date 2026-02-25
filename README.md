# log_pp

`log_pp` is a lightweight C++20 logging interface with:

- structured records (`level`, `target`, `source location`, message args)
- optional heterogeneous key-value fields
- runtime level filtering
- compile-time macro filtering (separate non-Release and Release settings)

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
				std::cout << std::format("[{}] {}\n",
										record.get_level(),
										std::vformat(record.get_format_string(), record.get_args())
                             );
		}

		void flush() override { std::cout.flush(); }
};

int main() {
		static ConsoleLogger logger;
		log_pp::set_logger(logger);
		log_pp::set_max_level(log_pp::LevelFilter::Trace);

		LOG_PP_INFO("hello {}", "world");
		LOG_PP_INFO({"api"}, "request {}", 42);
		LOG_PP_INFO({{"request_id", 42}, {"ok", true}}, "completed");
}
```

See also: `examples/simple_logger/simple_logger.cpp`.

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run tests

```bash
ctest --test-dir build --output-on-failure
```

## Build example

```bash
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build --target simple_logger
```

## API overview

- `log_pp::BasicLogger<CharT>`: implement this interface.
- `log_pp::set_logger(...)`: set global logger once (first call wins).
- `log_pp::set_max_level(...)`: runtime maximum level filter.
- `LOG_PP_TRACE/DEBUG/INFO/WARN/ERROR(...)`: logging macros with automatic
	`std::source_location` capture.

## Compile-time log filtering

You can filter log macros at compile time for non-Release and Release builds
independently.

Set at most one option in each group:

- non-Release group:
	- `LOG_PP_LEVEL_FILTER_TRACE`
	- `LOG_PP_LEVEL_FILTER_DEBUG`
	- `LOG_PP_LEVEL_FILTER_INFO`
	- `LOG_PP_LEVEL_FILTER_WARN`
	- `LOG_PP_LEVEL_FILTER_ERROR`
- Release group:
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

With this example, non-Release keeps `DEBUG/INFO/WARN/ERROR`, while Release
keeps `ERROR` only.

You can also define filter macros manually per translation unit before
including `log.hpp`.

## Key-value logging

`LOG_PP_*` macros accept optional heterogeneous key-value pairs:

```cpp
LOG_PP_INFO({{"request_id", 42}, {"user", "alice"}, {"ratio", 1.25}},
						"processed {} items",
						10);

LOG_PP_INFO({"api"},
						{{"status", "ok"}, {"elapsed_ms", 12.3}},
						"done: {}",
						true);

LOG_PP_INFO({{"id", 255, "0x{:04X}"}, {"ratio", 1.236, "{:.2f}"}},
						"custom formatted kv");
```

Supported kv entry forms:

- `{ "key", value }`
- `{ "key", value, "{fmt}" }`

## License

This project is licensed under the MIT License. See `LICENSE`.

