# BqLog (扁鹊日志) V 2.2.2

[English](./README.md) | **简体中文**

[![license](https://img.shields.io/badge/license-APACHE2.0-brightgreen.svg?style=flat)](LICENSE.txt)
[![Release Version](https://img.shields.io/badge/release-2.2.2-red.svg)](https://github.com/Tencent/BqLog/releases)
[![ChangeLog](https://img.shields.io/badge/📋_更新日志-v2.2.2-orange.svg?style=flat)](CHANGELOG.md)
[![GitHub Stars](https://img.shields.io/github/stars/Tencent/BqLog?style=flat&logo=github)](https://github.com/Tencent/BqLog/stargazers)
[![GitHub Forks](https://img.shields.io/github/forks/Tencent/BqLog?style=flat&logo=github)](https://github.com/Tencent/BqLog/network/members)
[![GitHub Issues](https://img.shields.io/github/issues/Tencent/BqLog?style=flat&logo=github)](https://github.com/Tencent/BqLog/issues)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux%20%7C%20iOS%20%7C%20Android%20%7C%20HarmonyOS%20%7C%20Unix-lightgrey.svg?style=flat)]()
[![Language](https://img.shields.io/badge/language-C%2B%2B%20%7C%20Java%20%7C%20C%23%20%7C%20Kotlin%20%7C%20TypeScript%20%7C%20Python-blue.svg?style=flat)]()

> BqLog 是一个轻量级、高性能的工业级日志系统，已在线上广泛应用于《王者荣耀》等项目。
> **BqLog 2.x 正式发布！新增`纯血鸿蒙`，`Python`与 `Node.js` 支持，***性能较 1.x 实现数倍提升***，并带来非对称混合加密能力。**

---

[![Download](https://img.shields.io/badge/⬇_下载-Release_2.2.2-blue.svg?style=for-the-badge)](https://github.com/Tencent/BqLog/releases/tag/Release_2.2.2)

## 💡 如果您有以下困扰，可以尝试 BqLog

- 如果您的客户端产品（尤其是游戏）希望同时满足以下「不可能三角」：
  - 方便追溯问题（日志应写尽写）
  - 性能足够好（日志要少写）
  - 节约存储空间（日志最好就别写）
- 如果您是后台服务开发者，现有日志库在**高并发场景**下性能不足，导致日志丢失或程序阻塞。
- 如果您的编程语言是 C++、Java、C#、Kotlin、TypeScript、JavaScript、Python 之一，或者同时使用多种语言，希望有一套**统一的跨语言日志解决方案**。

---

## ✨ 特点

- 相比常见开源日志库有显著性能优势（详见 [Benchmark](#-benchmark-结果)），不仅适用于服务器和客户端，也非常适合移动端设备。
- 内存消耗少：在 Benchmark 用例中，10 线程、2,000 万条日志，BqLog 自身内存消耗约为 1 MB。
- 提供高性能、高压缩比的实时压缩日志格式。
- 以接近于0的性能损耗，提供高强度的非对称混合加密日志，保护日志内容安全（可选）。
- 可在游戏引擎（`Unity`、`Unreal` 等）中正常使用，对 Unreal 提供蓝图和常用类型的支持。
- 支持 `utf8`、`utf16`、`utf32` 字符及字符串，支持 bool、float、double、各种长度与类型的整数等常用参数类型。
- 支持 `C++20` 的 `std::format` 规范（不含排序序号与时间格式化）。
- 异步日志支持 Crash 复盘机制，尽量避免日志数据丢失。
- 在 Java、C#、TypeScript 上可以做到「零额外 Heap Alloc」（或极少），不会随着运行不断 new 对象。
- 仅依赖标准 C 语言库与平台 API，可在 Android 的 `ANDROID_STL = none` 模式下编译通过。
- 支持 `C++11` 及之后的标准，可在极其严格的编译选项下工作。
- 编译系统基于 `CMake`，并提供多平台编译脚本，集成简单。
- 支持自定义参数类型。
- 对代码提示非常友好。

---

## 🖥️ 支持的平台和语言

| 平台 | 语言 |
|------|------|
| Windows 64-bit、macOS、Linux（含嵌入式）、iOS、Android、HarmonyOS、Unix（FreeBSD、NetBSD、OpenBSD、Solaris 等） | C++（C++11+）、Java / Kotlin、C#（Unity、.NET）、ArkTS / C++（HarmonyOS）、JavaScript / TypeScript（Node.js）、Python 3.7+、Unreal Engine（UE4 & UE5） |

**硬件架构**：x86、x86_64、ARM32、ARM64
**引入方式**：动态库、静态库、源代码

---

## 🏗️ 架构介绍

![基础结构](docs/img/log_structure.png)

您的程序通过 BqLog 提供的 `BqLog Wrapper`（C++、Java、C#、TypeScript、Python 等）来访问核心引擎。每个 Log 对象可挂载一个或多个 Appender（控制台 / 文本文件 / 压缩文件）。**同一进程内，不同语言的 Wrapper 可以访问同一个 Log 对象。**

| Appender | 输出目标 | 可读 | 性能 | 尺寸 | 加密 |
|----------|---------|------|------|------|------|
| ConsoleAppender | 控制台 | Yes | 低 | - | No |
| TextFileAppender | 文件 | Yes | 低 | 大 | No |
| CompressedFileAppender | 文件 | No | 高 | 小 | Yes |

---

## 🚀 快速上手

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

### TypeScript (鸿蒙 ArkTS)

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

> 各平台完整集成步骤请见 [集成指南](docs/INTEGRATION_GUIDE_CHS.md)。

---

## 📊 Benchmark 结果

测试：1-10 线程，每线程写 2,000,000 条日志。环境：i9-13900K，128 GB，Windows 11。

#### 带 4 个参数的总耗时（毫秒）

|                         | 1 线程 | 2 线程 | 3 线程 | 4 线程 | 5 线程 | 6 线程 | 7 线程 | 8 线程 | 9 线程 | 10 线程 |
|-------------------------|--------|--------|--------|--------|--------|--------|--------|--------|--------|---------|
| BqLog Compress (C++)    | 110    | 125    | 188    | 256    | 318    | 374    | 449    | 511    | 583    | 642     |
| BqLog Text (C++)        | 344    | 699    | 1036   | 1401   | 1889   | 2211   | 2701   | 3121   | 3393   | 3561    |
| BqLog Compress (Java)   | 129    | 141    | 215    | 292    | 359    | 421    | 507    | 568    | 640    | 702     |
| BqLog Text (Java)       | 351    | 702    | 1052   | 1399   | 1942   | 2301   | 2754   | 3229   | 3506   | 3695    |
| Log4j2 Text             | 1065   | 2583   | 4249   | 4843   | 5068   | 6195   | 6424   | 7943   | 8794   | 9254    |

<img src="docs/img/benchmark_4_params.png" alt="4个参数的结果" style="width: 100%;">

#### 不带参数的总耗时（毫秒）

|                         | 1 线程 | 2 线程 | 3 线程 | 4 线程 | 5 线程 | 6 线程 | 7 线程 | 8 线程 | 9 线程 | 10 线程 |
|-------------------------|--------|--------|--------|--------|--------|--------|--------|--------|--------|---------|
| BqLog Compress (C++)    | 97     | 101    | 155    | 228    | 290    | 341    | 415    | 476    | 541    | 601     |
| BqLog Text (C++)        | 153    | 351    | 468    | 699    | 916    | 1098   | 1212   | 1498   | 1733   | 1908    |
| BqLog Compress (Java)   | 109    | 111    | 178    | 240    | 321    | 378    | 449    | 525    | 592    | 670     |
| BqLog Text (Java)       | 167    | 354    | 491    | 718    | 951    | 1139   | 1278   | 1550   | 1802   | 1985    |
| Log4j2 Text             | 3204   | 6489   | 7702   | 8485   | 9640   | 10458  | 11483  | 12853  | 13995  | 14633   |

<img src="docs/img/benchmark_no_param.png" alt="不带参数的结果" style="width: 100%;">

- TextFileAppender 场景下，BqLog 相比 Log4j2 有约 **3 倍** 性能优势
- CompressedFileAppender 场景下，BqLog 相比 Log4j2 有约 **10 倍以上** 性能优势
- 若与 BqLog 1.5 版本相比，2.x 平均性能提升约 **40%**

> 完整 Benchmark 代码和方法论请见 [Benchmark](docs/BENCHMARK_CHS.md)。

---

## 🔄 从 1.x 版本升级到 2.x 版本的变化

1. 增加对鸿蒙系统的支持，包括 ArkTS 和 C++ 两种语言。
2. 增加对 Node.js 的支持（CJS 和 ESM）。
3. 增强跨平台兼容性、稳定性与通用性，支持更多 Unix 系统。
4. utf8编码下性能平均提升约 80%，utf16编码环境（C#，Unreal，Unity）提升超过500%。
5. Android 不再强制要求与 Java 一起使用。
6. 移除 `is_in_sandbox` 配置，改用 `base_dir_type`；对 snapshot 增加过滤配置，支持每次启动新开日志文件。详见 [配置说明](docs/CONFIGURATION_CHS.md)。
7. 支持高性能非对称混合加密，几乎无额外性能损耗，详见 [高级用法 — 加密](docs/ADVANCED_USAGE_CHS.md#6-日志加密和解密)。
8. 提供 Unity、团结引擎、Unreal 引擎插件，方便在游戏引擎中使用；提供 ConsoleAppender 对游戏引擎编辑器日志输出重定向，提供 Unreal 蓝图支持。详见 [高级用法 — Unreal](docs/ADVANCED_USAGE_CHS.md#5-在-unreal-中使用-bqlog)。
9. 仓库不再包含二进制产物，从 2.x 版本起请从 [Releases 页面](https://github.com/Tencent/BqLog/releases)下载对应平台和语言的二进制包。
10. 单条日志长度不再受log.buffer_size限制。
11. 可以精确手动设置时区。
12. `raw_file`类型的appender不再维护，标记为`废弃`，请用`compressed_file`类型替代。
13. 复盘能力增加可靠性，从实验性功能变成正式能力。见[高级用法 — 数据保护](docs/ADVANCED_USAGE_CHS.md#3-程序异常退出的数据保护)。

---

## 📑 文档导航

| 文档 | 说明 |
|------|------|
| [集成指南](docs/INTEGRATION_GUIDE_CHS.md) | 所有平台完整集成步骤 + 各语言 Demo |
| [API 参考](docs/API_REFERENCE_CHS.md) | 核心 API、同步/异步日志、Appender 介绍、构建与工具 |
| [配置说明](docs/CONFIGURATION_CHS.md) | 完整配置参考（appenders、log、snapshot） |
| [高级用法](docs/ADVANCED_USAGE_CHS.md) | 无 Heap Alloc、Category、崩溃恢复、自定义类型、Unreal、加密 |
| [Benchmark](docs/BENCHMARK_CHS.md) | 完整 Benchmark 代码（C++、Java、Log4j）和结果 |

---

## 🤝 如何贡献代码

若您希望贡献代码，请确保您的改动能通过仓库中 GitHub Actions 下的以下工作流：

- `AutoTest`
- `Build`

建议在提交前本地运行对应脚本，确保测试与构建均正常通过。
