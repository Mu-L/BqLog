# @pippocao/bqlog

High-performance, cross-platform logging library for **Node.js** with a native C++ core. Part of the [BqLog](https://github.com/Tencent/BqLog) project.

## Features

- **Blazing fast** — native C++ ring-buffer core, minimal overhead
- **Sync & Async modes** — choose per-log thread model
- **Multiple appenders** — console, text file, compressed file
- **Category logging** — attach categories to log entries, filter with category masks
- **Format parameters** — `log.info("Hello {}, age {}", name, age)`
- **Snapshot** — capture and decode the in-memory log buffer at any time
- **Cross-platform** — Windows, macOS, Linux (x64 & ARM64), FreeBSD, and more
- **ESM + CommonJS** — dual module support, works everywhere

## Installation

```bash
npm install @pippocao/bqlog
```

## Quick Start

### ESM

```typescript
import { bq } from "@pippocao/bqlog";

const config = `
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
`;
const log = bq.log.create_log("my_log", config);

log.info("Hello BqLog! int:{}, float:{}", 123, 3.14);
bq.log.force_flush_all_logs();
```

### CommonJS

```javascript
const { bq } = require("@pippocao/bqlog");

const log = bq.log.create_log("my_log", `
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
`);
log.info("Hello from CJS!");
```

## Appender Types

| Type | Description |
|------|-------------|
| `console` | Output to stdout |
| `text_file` | Plain text log files |
| `compressed_file` | Binary compressed format (use BqLog decoder to read) |

## Configuration Example

```typescript
const config = `
    appenders_config.ConsoleAppender.type=console
    appenders_config.ConsoleAppender.time_zone=localtime
    appenders_config.ConsoleAppender.levels=[all]

    appenders_config.FileAppender.type=text_file
    appenders_config.FileAppender.time_zone=localtime
    appenders_config.FileAppender.levels=[info,warning,error,fatal]
    appenders_config.FileAppender.file_name=logs/app
    appenders_config.FileAppender.max_file_size=10000000
    appenders_config.FileAppender.expire_time_days=7

    log.thread_mode=async
`;
const log = bq.log.create_log("app", config);
```

## Category Logging

Use the [BqLog Category Generator](https://github.com/Tencent/BqLog/tree/main/tools/category_log_generator) tool to generate type-safe category wrappers:

```typescript
import { my_category_log } from "./my_category_log";

const log = my_category_log.create_log("cat_log", config);
log.info(log.cat.ModuleA.SystemA, "categorized message: {}", value);
```

## Log Levels

`verbose` · `debug` · `info` · `warning` · `error` · `fatal`

## Supported Platforms

| OS | Architectures |
|----|--------------|
| Windows | x64, ARM64 |
| macOS | x64, ARM64 (Universal) |
| Linux | x64, ARM64 |
| FreeBSD | x64, ARM64 |

## Documentation

Full documentation and examples for all supported languages (C++, Java, C#, Python, TypeScript, Unreal Engine, Unity, HarmonyOS) available at:

**[github.com/Tencent/BqLog](https://github.com/Tencent/BqLog)**

## License

[Apache-2.0](https://www.apache.org/licenses/LICENSE-2.0)
