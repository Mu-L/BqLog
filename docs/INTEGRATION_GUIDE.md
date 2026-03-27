# Integration Guide

[← Back to Home](../README.md) | [简体中文](./INTEGRATION_GUIDE_CHS.md)

> The examples below assume you have already downloaded the corresponding binary package or source code from the [Releases page](https://github.com/Tencent/BqLog/releases).

---

## C++ (dynamic / static / source)

- **Dynamic library**
  Download `dynamic_lib_{version}` archive:
  - Add `dynamic_lib/include` directory to your header search path;
  - Link against the dynamic library file in `dynamic_lib/lib` for your platform.

- **Static library**
  Download `static_lib_{version}` archive:
  - Add `static_lib/include` directory to your header search path;
  - Link against the static library file in `static_lib/lib` for your platform.

- **Source integration**
  - Add the `/src` directory under the repo into your project sources compilation;
  - Add `/include` directory to your header search path.
  - Windows + Visual Studio: please add compile option `/Zc:__cplusplus`.
  - Android supports `ANDROID_STL = none`.
  - If you need to enable Java / NAPI (Node.js / HarmonyOS ArkTS) support, as well as system link libraries and some macro definitions, please refer to `/src/CMakeLists.txt`(If the file implies complexity, consider using AI to interpret it.).
  - When NAPI environment (Node.js or HarmonyOS ArkTS) is present, or when Java / C# needs to call, it is not recommended to integrate directly as "pure C++ source", because initialization and library loading processes need to be handled manually; using prebuilt packages and corresponding wrappers is more recommended.

---

## C# (Unity / Tuanjie Engine / .NET)

- **Unity**
  - Download `unity_package_{version}`;
  - Unzip and in Unity Package Manager select "Install from tarball", pointing to the `.tar` file inside to import;
  - Official Unity does not support HarmonyOS yet; if you need HarmonyOS support, you can integrate it yourself as needed.

- **Tuanjie Engine**
  - Download `tuanjie_package_{version}`;
  - Unzip and import via Unity Package Manager as tarball similarly;
  - The main difference from Unity is that HarmonyOS related support is already integrated.

- **.NET**
  - Download the dynamic library package `{os}_{arch}_libs_{version}` for the corresponding platform, and import the dynamic libraries within;
  - Add the C# wrapper source to your project — either clone the repository and include `/wrapper/csharp/src`, or download `c#_wrapper_{version}` from the [Releases page](https://github.com/Tencent/BqLog/releases) which contains the same source files pre-packaged.

---

## Java / Kotlin (Android / Server)

- **Android**
  - Download `android_libs_{version}`;
  - You can directly import the `.aar` package within(The AAR follows standard AGP packaging conventions, with native headers and prebuilt libraries exported via Prefab.), or manually import `/src` + `/wrapper/java/src` source code under the repository.

- **Server**
  - Download the dynamic library `{os}_{arch}_libs_{version}` for the corresponding platform and import it;
  - Then download `java_wrapper_{version}`, import the jar package or directly add `/wrapper/java/src` source code under the repository.

---

## HarmonyOS (ArkTS / C++)

- Download `harmony_os_libs_{version}`;
- Import the `har` package, or directly import `.so` + `/wrapper/typescript/src` source code under the repository (optional);
- Supports direct calling from ArkTS side, also supports calling from Native C++ side.

---

## Node.js

- Supports CommonJS and ES Modules.
- From Releases download `nodejs_npm_{version}` package, unzip to find `pippocao-bqlog-{version}.tgz` inside, install via npm:

```bash
npm install ./pippocao-bqlog-{version}.tgz
```

Refer to `/demo/nodejs` directory under the repository.

---

## Python

- Supports Python 3.7+ (CPython C Extension, Stable ABI).
- From Releases download `python_wrapper_{version}` package, unzip to find `.whl` file inside, install via pip:

```bash
pip install ./bqlog-{version}-cp37-abi3-{platform}.whl
```

Refer to `/demo/python` directory under the repository.

---

## Unreal Engine

- **Prebuilt**
  - Download `unreal_plugin_prebuilt_{version}` from Releases;
  - Unzip and according to your engine version, select the corresponding compressed package, unzip to the `Plugins` directory of your game project.

- **Source**
  - Download `unreal_plugin_source_{version}` from Releases;
  - Unzip and according to your engine version, select the corresponding compressed package, unzip to the `Plugins` directory of your game project, to be recompiled by the engine.

---

## Quick Start Demos

### C++

```cpp
#include <string>
#include <bq_log/bq_log.h>

int main() {
    // Config: output to console
    std::string config = R"(
        appenders_config.appender_console.type=console
        appenders_config.appender_console.levels=[all]
    )";
    auto log = bq::log::create_log("main_log", config);

    log.info("Hello BqLog 2.0! int:{}, float:{}", 123, 3.14f);
    log.force_flush(); // Force flush (usually used before program exit)

    return 0;
}
```

For more examples, refer to `/demo/cpp` directory.

### TypeScript — Node.js

```typescript
import { bq } from "@pippocao/bqlog"; // ESM style
// const { bq } = require("@pippocao/bqlog"); // CommonJS style

const config = `
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
`;
const log = bq.log.create_log("node_log", config);

log.info("Hello from Node.js! params: {}, {}", "text", 123);
bq.log.force_flush_all_logs();
```

For more examples, refer to `/demo/nodejs` directory.

### TypeScript — ArkTS on HarmonyOS

```typescript
import { bq } from "bqlog"; // ohpm package name

const config = `
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
`;
const log = bq.log.create_log("ohos_log", config);

log.info("Hello from HarmonyOS! params: {}, {}", "text", 123);
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

For more examples, refer to `/demo/python` directory.

### C#

```csharp
string config = @"
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
";
var log = bq.log.create_log("cs_log", config);
log.info("Hello C#! value:{}", 42);
```

For more examples, refer to `/demo/csharp` directory.

### Java (Android / Server)

```java
String config = """
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
""";
bq.log.Log log = bq.log.Log.createLog("java_log", config);
log.info("Hello Java! value: {}", 3.14);
```

For more examples, refer to `/demo/java` directory.

---

## Next Steps

- [API Reference](./API_REFERENCE.md) — Core APIs for creating logs, writing logs, and more
- [Configuration](./CONFIGURATION.md) — Full configuration reference
- [Advanced Usage](./ADVANCED_USAGE.md) — Category, encryption, Unreal, custom types, and more
