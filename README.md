<p align="center">
  <img src="banner.jpg" alt="BqLog Banner" width="100%">
</p>

# BqLog (BianQue Log) V 2.2.8

**English** | [简体中文](./README_CHS.md)

[![license](https://img.shields.io/badge/license-APACHE2.0-brightgreen.svg?style=flat)](LICENSE.txt)
[![Release Version](https://img.shields.io/badge/release-2.2.8-red.svg)](https://github.com/Tencent/BqLog/releases)
[![ChangeLog](https://img.shields.io/badge/📋_ChangeLog-v2.2.8-orange.svg?style=flat)](CHANGELOG.md)
[![GitHub Stars](https://img.shields.io/github/stars/Tencent/BqLog?style=flat&logo=github)](https://github.com/Tencent/BqLog/stargazers)
[![GitHub Forks](https://img.shields.io/github/forks/Tencent/BqLog?style=flat&logo=github)](https://github.com/Tencent/BqLog/network/members)
[![GitHub Issues](https://img.shields.io/github/issues/Tencent/BqLog?style=flat&logo=github)](https://github.com/Tencent/BqLog/issues)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux%20%7C%20iOS%20%7C%20Android%20%7C%20HarmonyOS%20%7C%20Unix-lightgrey.svg?style=flat)]()
[![Language](https://img.shields.io/badge/language-C%2B%2B%20%7C%20Java%20%7C%20C%23%20%7C%20Kotlin%20%7C%20TypeScript%20%7C%20Python-blue.svg?style=flat)]()

> BqLog is a lightweight, high-performance, industrial-grade logging system that has been widely used in online projects such as "Honor of Kings".
> **BqLog 2.x is officially released! With native `HarmonyOS NEXT`, `Python` and `Node.js` support, ***multiple times faster than 1.x***, and asymmetric hybrid encryption.**

---

[![Download](https://img.shields.io/badge/⬇_Download-Release_2.2.8-blue.svg?style=for-the-badge)](https://github.com/Tencent/BqLog/releases/tag/Release_2.2.8)

## 📋 What's New in v2.2.8

- **Code quality** — Refactored all SFINAE usages: moved `enable_if` from return types to template parameters for improved readability and consistency.
- **Bug fix** — Fixed crash when stack trace is enabled in C++. ([#62](https://github.com/Tencent/BqLog/issues/62))

> Full changelog → [CHANGELOG.md](CHANGELOG.md)

---

## 💡 If you have the following pain points, try BqLog

- If your client product (especially games) wants to satisfy this "impossible triangle" at the same time:
  - Easy troubleshooting (log as much as possible)
  - Good performance (log as little as possible)
  - Save storage space (better not log at all)
- If you are a backend service developer and your current logging library cannot handle **high-concurrency scenarios**, causing log loss or application stalls.
- If your programming language is one of C++, Java, C#, Kotlin, TypeScript, JavaScript, Python, or you use multiple languages at the same time and want a **unified cross-language logging solution**.

---

## ✨ Highlights

- Significant performance advantage over common open-source logging libraries (see [Benchmark](#-benchmark-results)); suitable for server, client, and mobile.
- Low memory usage: in the Benchmark case (10 threads, 20,000,000 log entries), BqLog itself uses about 1 MB of memory.
- Provides a high-performance, high-compression real-time compressed log format.
- Supports strong hybrid encryption (asymmetric + symmetric) for log content protection with nearly zero performance overhead (optional).
- Works well inside game engines (`Unity`, `Unreal`, etc.), with UE Blueprint and builtin data type support.
- Supports `utf8`, `utf16`, `utf32` characters and strings, as well as bool, float, double, and integer types of various sizes.
- Supports `C++20` `std::format` style format strings (without positional index and time formatting).
- Asynchronous logging supports crash recovery and tries to avoid data loss.
- On Java, C#, TypeScript wrappers, it can achieve "zero extra heap alloc" (or very close), avoiding continuous object allocations.
- Depends only on the standard C library and platform APIs; can be compiled with Android `ANDROID_STL = none`.
- Supports `C++11` and later standards and works under very strict compiler options.
- Build system is based on `CMake` and provides multi-platform scripts, easy to integrate.
- Supports custom parameter types.
- Very friendly for code completion and IDE hints.

---

## 🖥️ Supported platforms & languages

| Platforms | Languages |
|-----------|-----------|
| Windows 64-bit, macOS, Linux (incl. embedded), iOS, Android, HarmonyOS, Unix (FreeBSD, NetBSD, OpenBSD, Solaris, etc.) | C++ (C++11+), Java / Kotlin, C# (Unity, .NET), ArkTS / C++ (HarmonyOS), JavaScript / TypeScript (Node.js), Python 3.7+, Unreal Engine (UE4 & UE5) |

**Hardware architectures**: x86, x86_64, ARM32, ARM64
**Integration methods**: Dynamic library, Static library, Source code

---

## 🏗️ Architecture

![Structure](docs/img/log_structure.png)

Your program accesses the core engine through `BqLog Wrapper` (C++, Java, C#, TypeScript, Python, etc.). Each Log object can mount one or more Appenders (Console / Text File / Compressed File). **Within the same process, Wrappers of different languages can access the same Log object.**

| Appender | Output | Readable | Performance | Size | Encryption |
|----------|--------|----------|-------------|------|------------|
| ConsoleAppender | Console | Yes | Low | - | No |
| TextFileAppender | File | Yes | Low | Large | No |
| CompressedFileAppender | File | No | High | Small | Yes |

---

## 🚀 Quick Start

> Before calling any API, you need to integrate BqLog into your project first:
> - **Standard environments** (C++, Java, C#, Python, Node.js, etc.) → [Integration Guide](docs/INTEGRATION_GUIDE.md)
> - **Game engines** (Unity, Tuanjie Engine, Unreal Engine) → [Game Engine Integration Guide](docs/ENGINE_INTEGRATION.md)

### C++

```cpp
#include <string>
#include <bq_log/bq_log.h>

int main() {
    std::string config = R"(
        appenders_config.appender_console.type=console
        appenders_config.appender_console.levels=[all]
    )";
    auto log = bq::log::create_log("main_log", config);
    log.info("Hello BqLog 2.0! int:{}, float:{}", 123, 3.14f);
    log.force_flush();
    return 0;
}
```

### Java

```java
String config = """
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
""";
bq.log.Log log = bq.log.Log.createLog("java_log", config);
log.info("Hello Java! value: {}", 3.14);
```

### C#

```csharp
string config = @"
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
";
var log = bq.log.create_log("cs_log", config);
log.info("Hello C#! value:{}", 42);
```

### TypeScript (Node.js)

```typescript
import { bq } from "@pippocao/bqlog";
const config = `
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
`;
const log = bq.log.create_log("node_log", config);
log.info("Hello from Node.js! params: {}, {}", "text", 123);
bq.log.force_flush_all_logs();
```

### Python

```python
from bq.log import log
config = """
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
"""
my_log = log.create_log("python_log", config)
my_log.info("Hello from Python! params: {}, {}", "text", 123)
log.force_flush_all_logs()
```

### TypeScript (HarmonyOS ArkTS)

```typescript
import { bq } from "bqlog";
const config = `
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
`;
const log = bq.log.create_log("ohos_log", config);
log.info("Hello from HarmonyOS! params: {}, {}", "text", 123);
bq.log.force_flush_all_logs();
```

> For full integration steps for all platforms, see [Integration Guide](docs/INTEGRATION_GUIDE.md) and [Game Engine Integration Guide](docs/ENGINE_INTEGRATION.md).

---

## 📊 Benchmark results

Test: 1–10 threads, each writing 2,000,000 log entries. Environment: i9-13900K, 128 GB, Windows 11.

#### Total Time Cost with 4 parameters (ms)

|                         | 1 Thread | 2 Threads | 3 Threads | 4 Threads | 5 Threads | 6 Threads | 7 Threads | 8 Threads | 9 Threads | 10 Threads |
|-------------------------|--------|--------|--------|--------|--------|--------|--------|--------|--------|---------|
| BqLog Compress (C++)    | 110    | 125    | 188    | 256    | 318    | 374    | 449    | 511    | 583    | 642     |
| BqLog Text (C++)        | 344    | 699    | 1036   | 1401   | 1889   | 2211   | 2701   | 3121   | 3393   | 3561    |
| BqLog Compress (Java)   | 129    | 141    | 215    | 292    | 359    | 421    | 507    | 568    | 640    | 702     |
| BqLog Text (Java)       | 351    | 702    | 1052   | 1399   | 1942   | 2301   | 2754   | 3229   | 3506   | 3695    |
| Log4j2 Text             | 1065   | 2583   | 4249   | 4843   | 5068   | 6195   | 6424   | 7943   | 8794   | 9254    |

<img src="docs/img/benchmark_4_params.png" alt="Results of 4 params" style="width: 100%;">

#### Total Time Cost without parameters (ms)

|                         | 1 Thread | 2 Threads | 3 Threads | 4 Threads | 5 Threads | 6 Threads | 7 Threads | 8 Threads | 9 Threads | 10 Threads |
|-------------------------|--------|--------|--------|--------|--------|--------|--------|--------|--------|---------|
| BqLog Compress (C++)    | 97     | 101    | 155    | 228    | 290    | 341    | 415    | 476    | 541    | 601     |
| BqLog Text (C++)        | 153    | 351    | 468    | 699    | 916    | 1098   | 1212   | 1498   | 1733   | 1908    |
| BqLog Compress (Java)   | 109    | 111    | 178    | 240    | 321    | 378    | 449    | 525    | 592    | 670     |
| BqLog Text (Java)       | 167    | 354    | 491    | 718    | 951    | 1139   | 1278   | 1550   | 1802   | 1985    |
| Log4j2 Text             | 3204   | 6489   | 7702   | 8485   | 9640   | 10458  | 11483  | 12853  | 13995  | 14633   |

<img src="docs/img/benchmark_no_param.png" alt="Results of no param" style="width: 100%;">

- TextFileAppender: BqLog has about **3x** performance advantage over Log4j2
- CompressedFileAppender: BqLog has about **10x+** performance advantage over Log4j2
- Compared with BqLog 1.5, 2.x average performance improved by about **40%**

> For full benchmark code and methodology, see [Benchmark](docs/BENCHMARK.md).

---

## 🔄 Changes from 1.x to 2.x

1. Added HarmonyOS support, including ArkTS and C++.
2. Added Node.js support (CJS and ESM).
3. Improved cross-platform compatibility, stability and generality; supports more Unix systems.
4. Average performance improved by ~80% for UTF-8, and by >500% for UTF-16 environments (C#, Unreal, Unity).
5. Android no longer must be used together with Java.
6. Removed the `is_in_sandbox` config and replaced it with `base_dir_type`; added filters for snapshots and support for opening a new log file on each startup. See [Configuration](docs/CONFIGURATION.md).
7. Added high-performance hybrid asymmetric encryption, ***almost zero overhead***; see [Advanced Usage — Encryption](docs/ADVANCED_USAGE.md#6-log-encryption-and-decryption).
8. Provides Unity, Tuanjie Engine, and Unreal Engine plugins, making it easy to use in game engines; provides ConsoleAppender redirection to game-engine editors and Blueprint support for Unreal. See [Game Engine Integration Guide](docs/ENGINE_INTEGRATION.md).
9. The repository no longer ships binaries. From 2.x on, please download platform- and language-specific packages from the [Releases page](https://github.com/Tencent/BqLog/releases).
10. The size of a single log entry is not limited by `log.buffer_size` anymore;
11. The timezone can be specified manually.
12. The `raw_file` appender is deprecated and no longer maintained in 2.x; please use the `compressed_file` appender instead.
13. The Recovery feature's reliability has been improved and it has been promoted from experimental (beta) to stable (release). see [Advanced Usage — Data Protection](docs/ADVANCED_USAGE.md#3-data-protection-on-abnormal-exit).

---

## 📑 Documentation

| Document | Description |
|----------|-------------|
| [Integration Guide](docs/INTEGRATION_GUIDE.md) | Full integration steps for all platforms + all language demos |
| [Game Engine Integration](docs/ENGINE_INTEGRATION.md) | Unity, Tuanjie Engine, Unreal Engine plugins and Blueprint usage |
| [API Reference](docs/API_REFERENCE.md) | Core APIs, sync/async logging, Appender overview, build & tools |
| [Configuration](docs/CONFIGURATION.md) | Full configuration reference (appenders, log, snapshot) |
| [Advanced Usage](docs/ADVANCED_USAGE.md) | No Heap Alloc, Category, crash recovery, custom types, encryption |
| [Benchmark](docs/BENCHMARK.md) | Full benchmark code (C++, Java, Log4j) and results |

---

## 🤝 How to contribute

If you want to contribute code, please make sure your changes can pass the following workflows under GitHub Actions in the repository:

- `AutoTest`
- `Build`

It is recommended to run corresponding scripts locally before submitting to ensure both testing and building pass normally.
