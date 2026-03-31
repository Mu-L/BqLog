# Game Engine Integration Guide

[← Back to Home](../README.md) | [简体中文](./ENGINE_INTEGRATION_CHS.md)

---

## Unity / Tuanjie Engine

### Integration

- **Unity**
  - Download `unity_package_{version}` from the [Releases page](https://github.com/Tencent/BqLog/releases);
  - Unzip and in Unity Package Manager select "Install from tarball", pointing to the `.tar` file inside to import;
  - Official Unity does not support HarmonyOS yet; if you need HarmonyOS support, you can integrate it yourself as needed.

- **Tuanjie Engine**
  - Download `tuanjie_package_{version}` from the [Releases page](https://github.com/Tencent/BqLog/releases);
  - Unzip and import via Unity Package Manager as tarball similarly;
  - The main difference from Unity is that HarmonyOS related support is already integrated.

### Usage

After importing, use the `bq.log` C# API:

```csharp
string config = @"
    appenders_config.console.type=console
    appenders_config.console.levels=[all]
";
var log = bq.log.create_log("cs_log", config);
log.info("Hello from Unity! value:{}", 42);
```

For more examples, refer to `/demo/csharp` directory in the repository.

---

## Unreal Engine

### Integration

- **Prebuilt**
  - Download `unreal_plugin_prebuilt_{version}` from the [Releases page](https://github.com/Tencent/BqLog/releases);
  - Unzip and according to your engine version, select the corresponding compressed package, unzip to the `Plugins` directory of your game project.

- **Source**
  - Download `unreal_plugin_source_{version}` from the [Releases page](https://github.com/Tencent/BqLog/releases);
  - Unzip and according to your engine version, select the corresponding compressed package, unzip to the `Plugins` directory of your game project, to be recompiled by the engine.

### 1) Support for `FName` / `FString` / `FText`

In Unreal environment, BqLog has built-in adapters:

- Automatically support `FString`, `FName`, `FText` as format string and parameters;
- Compatible with UE4 and UE5.

Example:

```cpp
bq::log log_my = bq::log::create_log("AAA", config);   // config omitted

FString fstring_1 = TEXT("This is a test FString{}");
FString fstring_2 = TEXT("This is also a test FString");
log_my.error(fstring_1, fstring_2);

FText text1 = FText::FromString(TEXT("This is a FText!"));
FName name1 = FName(TEXT("This is a FName"));
log_my.error(fstring_1, text1);
log_my.error(fstring_1, name1);
```

If you wish to customize adaptation behavior, you can also use "Method 2 (Global Function)" to define `bq_log_format_str_size` and `bq_log_format_str_chars` yourself (see [Advanced Usage](./ADVANCED_USAGE.md)).

### 2) Redirect BqLog output to Unreal Log Window

If you have imported the Unreal plugin, BqLog logs will be automatically redirected to Unreal's Output Log.
If the plugin is not used, but BqLog is integrated directly at C++ level, you can use the console callback to forward manually:

```cpp
// You can get Log name and Category name based on different category_idx / log_id,
// forward them to different UE_LOG Categories.
static void on_bq_log(uint64_t log_id,
                      int32_t category_idx,
                      int32_t log_level,
                      const char* content,
                      int32_t length)
{
    switch (log_level)
    {
    case (int32_t)bq::log_level::verbose:
        UE_LOG(LogTemp, VeryVerbose, TEXT("%s"), UTF8_TO_TCHAR(content));
        break;
    case (int32_t)bq::log_level::debug:
        UE_LOG(LogTemp, Verbose, TEXT("%s"), UTF8_TO_TCHAR(content));
        break;
    case (int32_t)bq::log_level::info:
        UE_LOG(LogTemp, Log, TEXT("%s"), UTF8_TO_TCHAR(content));
        break;
    case (int32_t)bq::log_level::warning:
        UE_LOG(LogTemp, Warning, TEXT("%s"), UTF8_TO_TCHAR(content));
        break;
    case (int32_t)bq::log_level::error:
        UE_LOG(LogTemp, Error, TEXT("%s"), UTF8_TO_TCHAR(content));
        break;
    case (int32_t)bq::log_level::fatal:
        UE_LOG(LogTemp, Fatal, TEXT("%s"), UTF8_TO_TCHAR(content));
        break;
    default:
        break;
    }
}

void CallThisOnYourGameStart()
{
    bq::log::register_console_callback(&on_bq_log);
}
```

### 3) Using BqLog in Blueprint

After importing the Unreal plugin, BqLog can be called directly in Blueprint:

1. **Create Log Data Asset**

   - Create Data Asset in Unreal project, type select BqLog:
     - Default Log Type (without Category):
       <img src="img/ue_pick_data_asset_1.png" alt="Default Log Creation" style="width: 455px">
     - If Log Class with Category is generated (see [Advanced Usage — Category](./ADVANCED_USAGE.md)), and `{category}.h` and `{category}_for_UE.h` are added to project:
       <img src="img/ue_pick_data_asset_2.png" alt="Category Log Creation" style="width: 455px">

2. **Configure Log Parameters**

   - Double click to open Data Asset, configure log object name and creation method:
     - `Create New Log`: Create a new Log object at runtime:
       <img src="img/ue_create_log_config_1.png" alt="Config Log Params Create New Log" style="width: 455px">
     - `Get Log By Name`: Only get Log with same name created elsewhere:
       <img src="img/ue_create_log_config_2.png" alt="Config Log Params Get Log By Name" style="width: 455px">

3. **Call Log Node in Blueprint**

   <img src="img/ue_print_log.png" alt="Blueprint Call Log" style="width: 655px">

   - Area 1: Add log parameters;
   - Area 2: Added log parameter nodes, can be deleted via right click menu (Remove ArgX);
   - Area 3: Select log object (i.e. Data Asset just created);
   - Area 4: Displayed only when log object has Category, can select log Category.

4. **Test**

   - Run Blueprint, if configured correctly and there is ConsoleAppender output, similar output can be seen in Log window:

     ```text
     LogBqLog: Display: [Bussiness_Log_Obj] UTC+7 2025-11-27 14:49:19.381[tid-27732 ] [I] [Factory.People.Manager] Test Log Arg0:String Arg, Arg1:TRUE, Arg2:1.000000,0.000000,0.000000|2.999996,0.00[...]
     ```

---

## Next Steps

- [Integration Guide](./INTEGRATION_GUIDE.md) — Integration for all platforms and languages
- [API Reference](./API_REFERENCE.md) — Core APIs for creating logs, writing logs, and more
- [Advanced Usage](./ADVANCED_USAGE.md) — Category, custom types, encryption, and more
