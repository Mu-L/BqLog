# 集成指南

[← 返回首页](../README_CHS.md) | [English](./INTEGRATION_GUIDE.md)

> 以下示例假定您已在 [Releases 页面](https://github.com/Tencent/BqLog/releases) 下载对应版本的二进制包或源码。

---

## C++（动态库 / 静态库 / 源码）

- **动态库**
  下载 `dynamic_lib_{version}` 压缩包：
  - 将 `dynamic_lib/include` 目录添加到头文件搜索路径；
  - 链接 `dynamic_lib/lib` 中对应平台的动态库文件。

- **静态库**
  下载 `static_lib_{version}` 压缩包：
  - 将 `static_lib/include` 目录添加到头文件搜索路径；
  - 链接 `static_lib/lib` 中对应平台的静态库文件。

- **源码集成**
  - 将仓库下的 `/src` 目录加入工程源码编译；
  - 将 `/include` 目录添加到头文件搜索路径。
  - Windows + Visual Studio：请添加编译选项 `/Zc:__cplusplus`。
  - Android 模式下支持 `ANDROID_STL = none`。
  - 如需启用 Java / NAPI（Node.js / HarmonyOS ArkTS）支持，以及系统链接库与部分宏定义，请参考 `/src/CMakeLists.txt`（如果觉得难以理解，建议求助AI提炼）。
  - 当存在 NAPI 环境（Node.js 或 HarmonyOS ArkTS）或需要 Java / C# 调用时，不推荐以「纯 C++ 源码」直接集成，因为需要手动处理初始化和库加载流程；更推荐使用预编译包及对应 wrapper。

---

## C#（Unity / 团结引擎 / .NET）

- **Unity**
  - 下载 `unity_package_{version}`；
  - 解压后在 Unity Package Manager 中选择「从 tarball 安装」，指向其中的 `.tar` 文件导入；
  - 官方 Unity 暂不支持鸿蒙，如需鸿蒙支持可按需自行集成。

- **团结引擎**
  - 下载 `tuanjie_package_{version}`；
  - 解压后同样通过 Unity Package Manager 以 tarball 方式导入；
  - 与 Unity 的主要差异是已集成鸿蒙相关支持。

- **.NET**
  - 下载对应平台的动态库包 `{os}_{arch}_libs_{version}`，引入其中动态库；
  - 将 C# wrapper 源码加入工程 —— 可以直接使用仓库下 `/wrapper/csharp/src`，也可以从 [Releases 页面](https://github.com/Tencent/BqLog/releases) 下载 `c#_wrapper_{version}` 预打包的源码包。

---

## Java / Kotlin（Android / Server）

### Android

- **Maven Central（推荐）**

  在 `app/build.gradle.kts` 中添加：

  ```kotlin
  android {
      buildFeatures {
          prefab = true   // 启用 Prefab，C++ 侧才能通过 find_package 找到头文件和 .so
      }
  }

  dependencies {
      implementation("com.tencent.bqlog:android:2.+")
  }
  ```

  ```java
  import bq.log;

  bq.log myLog = bq.log.create_log("myApp", """
      appenders_config.console.type=console
      appenders_config.console.levels=[all]
  """);
  myLog.info("Hello from Android Java!");
  ```

  **C++（NDK）通过 Prefab 使用** — 在 `CMakeLists.txt` 中添加：

  ```cmake
  find_package(BqLog REQUIRED CONFIG)

  target_link_libraries(your_native_lib
      bqlog::BqLog
      android
      log)
  ```

  C++ 代码中直接包含：

  ```cpp
  #include <bq_log/bq_log.h>
  ```

- **手动引入 AAR**

  从 [Releases 页面](https://github.com/Tencent/BqLog/releases) 下载 `android_libs_{version}`，将其中的 `bqlog-release.aar` 复制到项目的 `libs/` 目录，然后在 `app/build.gradle.kts` 中添加：

  ```kotlin
  android {
      buildFeatures {
          prefab = true
      }
  }

  dependencies {
      implementation(files("libs/bqlog-release.aar"))
  }
  ```

  Java / Kotlin 与 C++ 的使用方式与 Maven 方式完全相同。

### Java（Server / Desktop）

- **Maven Central（推荐）**

  fat JAR 内已打包全平台 native 库（Windows x86_64/arm64、Linux x86_64/arm64/x86、macOS universal、FreeBSD、OpenBSD、NetBSD、DragonFlyBSD、SunOS），运行时自动解压并加载对应平台的库文件，无需额外配置。

  **Maven (`pom.xml`)**：

  ```xml
  <dependency>
      <groupId>com.tencent.bqlog</groupId>
      <artifactId>java</artifactId>
      <version>[2.0,3.0)</version>
  </dependency>
  ```

  **Gradle (`build.gradle.kts`)**：

  ```kotlin
  dependencies {
      implementation("com.tencent.bqlog:java:2.+")
  }
  ```

  ```java
  import bq.log;

  bq.log myLog = bq.log.create_log("myApp", """
      appenders_config.console.type=console
      appenders_config.console.levels=[all]
  """);
  myLog.info("Hello from Java! value: {}", 3.14);
  ```

- **手动引入 JAR**

  从 [Releases 页面](https://github.com/Tencent/BqLog/releases) 下载对应平台的动态库 `{os}_{arch}_libs_{version}`，再下载 `java_wrapper_{version}`，将 JAR 加入 classpath，运行时通过 `-Djava.library.path=/path/to/lib` 指定 native 库目录。

---

## HarmonyOS（ArkTS / C++）

- 下载 `harmony_os_libs_{version}`；
- 引入 `har` 包，或直接引入其中的 `.so` + 仓库下 `/wrapper/typescript/src` 源码（可选）；
- 支持在 ArkTS 侧直接调用，也支持在 Native C++ 侧调用。

---

## Node.js

- 支持 CommonJS 与 ES Module。
- 从 Releases 下载 `nodejs_npm_{version}` 包，解压后找到其中的 `pippocao-bqlog-{version}.tgz`，通过 npm 安装：

```bash
npm install ./pippocao-bqlog-{version}.tgz
```

可参考仓库下 `/demo/nodejs` 目录。

---

## Python

- 支持 Python 3.7+（CPython C Extension，Stable ABI）。
- 从 Releases 下载 `python_wrapper_{version}` 包，解压后找到 `.whl` 文件，通过 pip 安装：

```bash
pip install ./bqlog-{version}-cp37-abi3-{platform}.whl
```

可参考仓库下 `/demo/python` 目录。

---

## Unreal Engine

- **预编译版（Prebuilt）**
  - 从 Releases 下载 `unreal_plugin_prebuilt_{version}`；
  - 解压后根据自己的引擎版本，选择对应压缩包，解压到游戏项目的 `Plugins` 目录下。

- **源码版（Source）**
  - 从 Releases 下载 `unreal_plugin_source_{version}`；
  - 解压后根据自己的引擎版本，选择对应压缩包，解压到游戏项目的 `Plugins` 目录下，由引擎进行二次编译。

---

## 快速上手 Demo

### C++

```cpp
#include <string>
#include <bq_log/bq_log.h>

int main() {
    // 配置：输出到控制台
    std::string config = R"(
        appenders_config.appender_console.type=console
        appenders_config.appender_console.levels=[all]
    )";
    auto log = bq::log::create_log("main_log", config);

    log.info("Hello BqLog 2.0! int:{}, float:{}", 123, 3.14f);
    log.force_flush(); // 强制刷新（通常用于程序退出前）

    return 0;
}
```

更多示例可参考仓库下的 `/demo/cpp` 目录。

### TypeScript — Node.js

```typescript
import { bq } from "@pippocao/bqlog"; // ESM 写法
// const { bq } = require("@pippocao/bqlog"); // CommonJS 写法

const config = `
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
`;
const log = bq.log.create_log("node_log", config);

log.info("Hello from Node.js! params: {}, {}", "text", 123);
bq.log.force_flush_all_logs();
```

更多示例可参考仓库下的 `/demo/nodejs` 目录。

### TypeScript — 鸿蒙 ArkTS

```typescript
import { bq } from "bqlog"; // ohpm 包名

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

更多示例可参考仓库下的 `/demo/python` 目录。

### C#

```csharp
string config = @"
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
";
var log = bq.log.create_log("cs_log", config);
log.info("Hello C#! value:{}", 42);
```

更多示例可参考仓库下的 `/demo/csharp` 目录。

### Java（Android / Server）

```java
String config = """
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
""";
bq.log.Log log = bq.log.Log.createLog("java_log", config);
log.info("Hello Java! value: {}", 3.14);
```

更多示例可参考仓库下的 `/demo/java` 目录。

---

## 接下来

- [API 参考](./API_REFERENCE_CHS.md) — 创建日志、写日志等核心 API
- [配置说明](./CONFIGURATION_CHS.md) — 完整配置参考
- [高级用法](./ADVANCED_USAGE_CHS.md) — Category、加密、Unreal、自定义类型等
