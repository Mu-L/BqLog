# Benchmark

[← Back to Home](../README.md) | [简体中文](./BENCHMARK_CHS.md)

### 1. Benchmark description

Test Environment:

- **CPU**: 13th Gen Intel(R) Core(TM) i9-13900K @ 3.00 GHz
- **Memory**: 128 GB
- **OS**: Windows 11

Test Cases:

- Use 1~10 threads to write logs simultaneously;
- Each thread writes 2,000,000 log entries:
  - One with 4 parameters formatted log;
  - One with no parameter plain text log;
- Wait for all threads to finish, then call `force_flush_all_logs()`, count total time from start of writing to all logs flushed to disk.

Comparison Objects:

- BqLog 2.x (C++ / Java, TextFileAppender and CompressedFileAppender)
- Log4j2 (Only TextFileAppender, **gzip compression not used when rolling**, because its compression is done on existing text files during rolling, performance model is different, cannot be compared fairly)

### 2. BqLog C++ Benchmark code

```cpp
#if defined(WIN32)
#include <windows.h>
#endif
#include "bq_log/bq_log.h"
#include <stdio.h>
#include <thread>
#include <chrono>
#include <string>
#include <iostream>
#include <vector>

void test_compress_multi_param(int32_t thread_count)
{
    std::cout << "============================================================" << std::endl;
    std::cout << "=========Begin Compressed File Log Test 1, 4 params=========" << std::endl;
    bq::log log_obj = bq::log::get_log_by_name("compress");
    std::vector<std::thread*> threads(thread_count);

    uint64_t start_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

    std::cout << "Now Begin, each thread will write 2000000 log entries, please wait the result..." << std::endl;

    for (int32_t idx = 0; idx < thread_count; ++idx)
    {
        threads[idx] = new std::thread([idx, &log_obj]() {
            for (int i = 0; i < 2000000; ++i)
            {
                log_obj.info("idx:{}, num:{}, This test, {}, {}",
                    idx, i, 2.4232f, true);
            }
        });
    }

    for (int32_t idx = 0; idx < thread_count; ++idx)
    {
        threads[idx]->join();
        delete threads[idx];
    }

    bq::log::force_flush_all_logs();

    uint64_t flush_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

    std::cout << "Time Cost:" << (flush_time - start_time) << std::endl;
    std::cout << "============================================================" << std::endl << std::endl;
}

void test_text_multi_param(int32_t thread_count)
{
    std::cout << "============================================================" << std::endl;
    std::cout << "============Begin Text File Log Test 2, 4 params============" << std::endl;
    bq::log log_obj = bq::log::get_log_by_name("text");
    std::vector<std::thread*> threads(thread_count);

    uint64_t start_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

    std::cout << "Now Begin, each thread will write 2000000 log entries, please wait the result..." << std::endl;

    for (int32_t idx = 0; idx < thread_count; ++idx)
    {
        threads[idx] = new std::thread([idx, &log_obj]() {
            for (int i = 0; i < 2000000; ++i)
            {
                log_obj.info("idx:{}, num:{}, This test, {}, {}",
                    idx, i, 2.4232f, true);
            }
        });
    }

    for (int32_t idx = 0; idx < thread_count; ++idx)
    {
        threads[idx]->join();
        delete threads[idx];
    }

    bq::log::force_flush_all_logs();

    uint64_t flush_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

    std::cout << "Time Cost:" << (flush_time - start_time) << std::endl;
    std::cout << "============================================================" << std::endl << std::endl;
}

void test_compress_no_param(int32_t thread_count)
{
    std::cout << "============================================================" << std::endl;
    std::cout << "=========Begin Compressed File Log Test 3, no param=========" << std::endl;
    bq::log log_obj = bq::log::get_log_by_name("compress");
    std::vector<std::thread*> threads(thread_count);

    uint64_t start_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

    std::cout << "Now Begin, each thread will write 2000000 log entries, please wait the result..." << std::endl;

    for (int32_t idx = 0; idx < thread_count; ++idx)
    {
        threads[idx] = new std::thread([&log_obj]() {
            for (int i = 0; i < 2000000; ++i)
            {
                log_obj.info("Empty Log, No Param");
            }
        });
    }

    for (int32_t idx = 0; idx < thread_count; ++idx)
    {
        threads[idx]->join();
        delete threads[idx];
    }

    bq::log::force_flush_all_logs();

    uint64_t flush_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

    std::cout << "Time Cost:" << (flush_time - start_time) << std::endl;
    std::cout << "============================================================" << std::endl << std::endl;
}

void test_text_no_param(int32_t thread_count)
{
    std::cout << "============================================================" << std::endl;
    std::cout << "============Begin Text File Log Test 4, no param============" << std::endl;
    bq::log log_obj = bq::log::get_log_by_name("text");
    std::vector<std::thread*> threads(thread_count);

    uint64_t start_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

    std::cout << "Now Begin, each thread will write 2000000 log entries, please wait the result..." << std::endl;

    for (int32_t idx = 0; idx < thread_count; ++idx)
    {
        threads[idx] = new std::thread([&log_obj]() {
            for (int i = 0; i < 2000000; ++i)
            {
                log_obj.info("Empty Log, No Param");
            }
        });
    }

    for (int32_t idx = 0; idx < thread_count; ++idx)
    {
        threads[idx]->join();
        delete threads[idx];
    }

    bq::log::force_flush_all_logs();

    uint64_t flush_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

    std::cout << "Time Cost:" << (flush_time - start_time) << std::endl;
    std::cout << "============================================================" << std::endl << std::endl;
}

int main()
{
#ifdef BQ_WIN
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    bq::log compressed_log = bq::log::create_log("compress", R"(
        appenders_config.appender_3.type=compressed_file
        appenders_config.appender_3.levels=[all]
        appenders_config.appender_3.file_name=benchmark_output/compress_
        appenders_config.appender_3.capacity_limit=1
    )");

    bq::log text_log = bq::log::create_log("text", R"(
        appenders_config.appender_3.type=text_file
        appenders_config.appender_3.levels=[all]
        appenders_config.appender_3.file_name=benchmark_output/text_
        appenders_config.appender_3.capacity_limit=1
    )");

    std::cout << "Please input the number of threads which will write log simultaneously:" << std::endl;
    int32_t thread_count = 0;
    std::cin >> thread_count;

    // Trigger capacity_limit with one log to make sure old log files is deleted
    compressed_log.verbose("use this log to trigger capacity_limit make sure old log files is deleted");
    text_log.verbose("use this log to trigger capacity_limit make sure old log files is deleted");
    bq::log::force_flush_all_logs();

    test_compress_multi_param(thread_count);
    test_text_multi_param(thread_count);
    test_compress_no_param(thread_count);
    test_text_no_param(thread_count);

    return 0;
}
```

### 3. BqLog Java Benchmark code

```java
import java.util.Scanner;

/**
 * Please ensure that the dynamic library corresponding to the current platform is in java.library.path before running,
 * or configure Native Library Location in IDE pointing to dynamic library directory under (ProjectRoot)/dist.
 * Otherwise UnsatisfiedLinkError may be encountered.
 */
public class benchmark_main {

    static abstract class benchmark_thread implements Runnable {
        protected int idx;
        public benchmark_thread(int idx) {
            this.idx = idx;
        }
    }

    private static void test_compress_multi_param(int thread_count) throws Exception {
        System.out.println("============================================================");
        System.out.println("=========Begin Compressed File Log Test 1, 4 params=========");
        bq.log log_obj = bq.log.get_log_by_name("compress");
        Thread[] threads = new Thread[thread_count];

        long start_time = System.currentTimeMillis();
        System.out.println("Now Begin, each thread will write 2000000 log entries, please wait the result...");

        for (int idx = 0; idx < thread_count; ++idx) {
            Runnable r = new benchmark_thread(idx) {
                @Override
                public void run() {
                    for (int i = 0; i < 2000000; ++i) {
                        log_obj.info("idx:{}, num:{}, This test, {}, {}",
                            bq.utils.param.no_boxing(idx),
                            bq.utils.param.no_boxing(i),
                            bq.utils.param.no_boxing(2.4232f),
                            bq.utils.param.no_boxing(true));
                    }
                }
            };
            threads[idx] = new Thread(r);
            threads[idx].start();
        }

        for (int idx = 0; idx < thread_count; ++idx) {
            threads[idx].join();
        }

        bq.log.force_flush_all_logs();

        long flush_time = System.currentTimeMillis();
        System.out.println("Time Cost:" + (flush_time - start_time));
        System.out.println("============================================================");
        System.out.println();
    }

    private static void test_text_multi_param(int thread_count) throws Exception {
        System.out.println("============================================================");
        System.out.println("============Begin Text File Log Test 2, 4 params============");
        bq.log log_obj = bq.log.get_log_by_name("text");
        Thread[] threads = new Thread[thread_count];

        long start_time = System.currentTimeMillis();
        System.out.println("Now Begin, each thread will write 2000000 log entries, please wait the result...");

        for (int idx = 0; idx < thread_count; ++idx) {
            Runnable r = new benchmark_thread(idx) {
                @Override
                public void run() {
                    for (int i = 0; i < 2000000; ++i) {
                        log_obj.info("idx:{}, num:{}, This test, {}, {}",
                            bq.utils.param.no_boxing(idx),
                            bq.utils.param.no_boxing(i),
                            bq.utils.param.no_boxing(2.4232f),
                            bq.utils.param.no_boxing(true));
                    }
                }
            };
            threads[idx] = new Thread(r);
            threads[idx].start();
        }

        for (int idx = 0; idx < thread_count; ++idx) {
            threads[idx].join();
        }

        bq.log.force_flush_all_logs();

        long flush_time = System.currentTimeMillis();
        System.out.println("Time Cost:" + (flush_time - start_time));
        System.out.println("============================================================");
        System.out.println();
    }

    private static void test_compress_no_param(int thread_count) throws Exception {
        System.out.println("============================================================");
        System.out.println("=========Begin Compressed File Log Test 3, no param=========");
        bq.log log_obj = bq.log.get_log_by_name("compress");
        Thread[] threads = new Thread[thread_count];

        long start_time = System.currentTimeMillis();
        System.out.println("Now Begin, each thread will write 2000000 log entries, please wait the result...");

        for (int idx = 0; idx < thread_count; ++idx) {
            Runnable r = new benchmark_thread(idx) {
                @Override
                public void run() {
                    for (int i = 0; i < 2000000; ++i) {
                        log_obj.info("Empty Log, No Param");
                    }
                }
            };
            threads[idx] = new Thread(r);
            threads[idx].start();
        }

        for (int idx = 0; idx < thread_count; ++idx) {
            threads[idx].join();
        }

        bq.log.force_flush_all_logs();

        long flush_time = System.currentTimeMillis();
        System.out.println("Time Cost:" + (flush_time - start_time));
        System.out.println("============================================================");
        System.out.println();
    }

    private static void test_text_no_param(int thread_count) throws Exception {
        System.out.println("============================================================");
        System.out.println("============Begin Text File Log Test 4, no param============");
        bq.log log_obj = bq.log.get_log_by_name("text");
        Thread[] threads = new Thread[thread_count];

        long start_time = System.currentTimeMillis();
        System.out.println("Now Begin, each thread will write 2000000 log entries, please wait the result...");

        for (int idx = 0; idx < thread_count; ++idx) {
            Runnable r = new benchmark_thread(idx) {
                @Override
                public void run() {
                    for (int i = 0; i < 2000000; ++i) {
                        log_obj.info("Empty Log, No Param");
                    }
                }
            };
            threads[idx] = new Thread(r);
            threads[idx].start();
        }

        for (int idx = 0; idx < thread_count; ++idx) {
            threads[idx].join();
        }

        bq.log.force_flush_all_logs();

        long flush_time = System.currentTimeMillis();
        System.out.println("Time Cost:" + (flush_time - start_time));
        System.out.println("============================================================");
        System.out.println();
    }

    public static void main(String[] args) throws Exception {
        bq.log compressed_log =  bq.log.create_log("compress", """
            appenders_config.appender_3.type=compressed_file
            appenders_config.appender_3.levels=[all]
            appenders_config.appender_3.file_name=benchmark_output/compress_
            appenders_config.appender_3.capacity_limit=1
        """);

        bq.log text_log =  bq.log.create_log("text", """
            appenders_config.appender_3.type=text_file
            appenders_config.appender_3.levels=[all]
            appenders_config.appender_3.file_name=benchmark_output/text_
            appenders_config.appender_3.capacity_limit=1
        """);

        System.out.println("Please input the number of threads which will write log simultaneously:");
        int thread_count = 0;

        try (Scanner scanner = new Scanner(System.in)) {
            thread_count = scanner.nextInt();
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        compressed_log.verbose("use this log to trigger capacity_limit make sure old log files is deleted");
        text_log.verbose("use this log to trigger capacity_limit make sure old log files is deleted");
        bq.log.force_flush_all_logs();

        test_compress_multi_param(thread_count);
        test_text_multi_param(thread_count);
        test_compress_no_param(thread_count);
        test_text_no_param(thread_count);
    }

}
```

### 4. Log4j Benchmark code

Log4j2 part only tests text output format, because its gzip compression is "re-gzip compression on existing text files during rolling", which is completely different from BqLog's real-time compression mode performance model, and cannot be directly benchmarked.

**Dependencies:**

```xml
<!-- pom.xml -->
<dependency>
  <groupId>org.apache.logging.log4j</groupId>
  <artifactId>log4j-api</artifactId>
  <version>2.23.1</version>
</dependency>
<dependency>
  <groupId>org.apache.logging.log4j</groupId>
  <artifactId>log4j-core</artifactId>
  <version>2.23.1</version>
</dependency>
<dependency>
  <groupId>com.lmax</groupId>
  <artifactId>disruptor</artifactId>
  <version>3.4.2</version>
</dependency>
```

Enable AsyncLogger:

```properties
# log4j2.component.properties
log4j2.contextSelector=org.apache.logging.log4j.core.async.AsyncLoggerContextSelector
```

Log4j2 Configuration:

```xml
<!-- log4j2.xml -->
<?xml version="1.0" encoding="UTF-8"?>
<Configuration status="WARN">
  <Appenders>
    <Console name="Console" target="SYSTEM_OUT">
      <PatternLayout pattern="%d{HH:mm:ss.SSS} [%t] %-5level %logger{36} - %msg%n"/>
    </Console>

    <!-- RollingRandomAccessFile, for text output demo -->
    <RollingRandomAccessFile name="my_appender"
                             fileName="logs/compress.log"
                             filePattern="logs/compress-%d{yyyy-MM-dd}-%i.log"
                             immediateFlush="false">
      <PatternLayout>
        <Pattern>%d{yyyy-MM-dd HH:mm:ss} [%t] %-5level %logger{36} - %msg%n</Pattern>
      </PatternLayout>
      <Policies>
        <TimeBasedTriggeringPolicy interval="1" modulate="true"/>
      </Policies>
      <DefaultRolloverStrategy max="5"/>
    </RollingRandomAccessFile>

    <!-- Async Appender -->
    <Async name="Async" includeLocation="false" bufferSize="262144">
      <!-- <AppenderRef ref="Console"/> -->
      <AppenderRef ref="my_appender"/>
    </Async>
  </Appenders>

  <Loggers>
    <Root level="info">
      <AppenderRef ref="Async"/>
    </Root>
  </Loggers>
</Configuration>
```

Source Code:

```java
package bq.benchmark.log4j;

import java.util.Scanner;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.core.async.AsyncLoggerContextSelector;

import static org.apache.logging.log4j.util.Unbox.box;

public class main {

    public static final Logger log_obj = LogManager.getLogger(main.class);

    static abstract class benchmark_thread implements Runnable {
        protected int idx;
        protected Logger log_obj;
        public benchmark_thread(int idx, Logger log_obj) {
            this.idx = idx;
            this.log_obj = log_obj;
        }
    }

    private static void test_text_multi_param(int thread_count) throws Exception {
        System.out.println("============================================================");
        System.out.println("============Begin Text File Log Test 1, 4 params============");
        Thread[] threads = new Thread[thread_count];

        long start_time = System.currentTimeMillis();
        System.out.println("Now Begin, each thread will write 2000000 log entries, please wait the result...");

        for (int idx = 0; idx < thread_count; ++idx) {
            Runnable r = new benchmark_thread(idx, log_obj) {
                @Override
                public void run() {
                    for (int i = 0; i < 2000000; ++i) {
                        log_obj.info("idx:{}, num:{}, This test, {}, {}",
                            box(idx), box(i), box(2.4232f), box(true));
                    }
                }
            };
            threads[idx] = new Thread(r);
            threads[idx].start();
        }

        for (int idx = 0; idx < thread_count; ++idx) {
            threads[idx].join();
        }

        org.apache.logging.log4j.core.LoggerContext context =
            (org.apache.logging.log4j.core.LoggerContext) LogManager.getContext(false);
        context.stop();
        LogManager.shutdown();

        long flush_time = System.currentTimeMillis();
        System.out.println("Time Cost:" + (flush_time - start_time));
        System.out.println("============================================================");
        System.out.println();
    }

    private static void test_text_no_param(int thread_count) throws Exception {
        System.out.println("============================================================");
        System.out.println("============Begin Text File Log Test 1, no param============");
        Thread[] threads = new Thread[thread_count];

        long start_time = System.currentTimeMillis();
        System.out.println("Now Begin, each thread will write 2000000 log entries, please wait the result...");

        for (int idx = 0; idx < thread_count; ++idx) {
            Runnable r = new benchmark_thread(idx, log_obj) {
                @Override
                public void run() {
                    for (int i = 0; i < 2000000; ++i) {
                        log_obj.info("Empty Log, No Param");
                    }
                }
            };
            threads[idx] = new Thread(r);
            threads[idx].start();
        }

        for (int idx = 0; idx < thread_count; ++idx) {
            threads[idx].join();
        }

        org.apache.logging.log4j.core.LoggerContext context =
            (org.apache.logging.log4j.core.LoggerContext) LogManager.getContext(false);
        context.stop();
        LogManager.shutdown();

        long flush_time = System.currentTimeMillis();
        System.out.println("Time Cost:" + (flush_time - start_time));
        System.out.println("============================================================");
        System.out.println();
    }

    public static void main(String[] args) throws Exception {
        System.out.println("Please input the number of threads which will write log simultaneously:");
        int thread_count = 0;

        try (Scanner scanner = new Scanner(System.in)) {
            thread_count = scanner.nextInt();
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        System.out.println("Is Async:" + AsyncLoggerContextSelector.isSelected());

        // These two tests need to run separately, because LoggerContext is no longer available after force stop.
        test_text_multi_param(thread_count);
        // test_text_no_param(thread_count);
    }

}
```

### 5. Benchmark results

All time costs are in milliseconds, smaller values mean higher performance.
From the results:

- In TextFileAppender scenario, BqLog has about **3 times** performance advantage over Log4j2;
- In CompressedFileAppender scenario, BqLog has about **10 times or more** performance advantage over Log4j2 text output;
- Compared with BqLog 1.5 version, 2.x average performance improved by about **40%**.


> For clarity, BqLog 1.5 data is not included, you can compare directly with [Benchmark](https://github.com/Tencent/BqLog/blob/stable_1.5/README.md#5-benchmark-results) results in old version documentation. Same hardware and OS environment, same test cases, same Log4j results were used. From results, BqLog 2.x version has about 40% performance improvement over 1.5 version.
#### Total Time Cost with 4 parameters (ms)

|                         | 1 Thread | 2 Threads | 3 Threads | 4 Threads | 5 Threads | 6 Threads | 7 Threads | 8 Threads | 9 Threads | 10 Threads |
|-------------------------|--------|--------|--------|--------|--------|--------|--------|--------|--------|---------|
| BqLog Compress (C++)    | 110    | 125    | 188    | 256    | 318    | 374    | 449    | 511    | 583    | 642     |
| BqLog Text (C++)        | 344    | 699    | 1036   | 1401   | 1889   | 2211   | 2701   | 3121   | 3393   | 3561    |
| BqLog Compress (Java)   | 129    | 141    | 215    | 292    | 359    | 421    | 507    | 568    | 640    | 702     |
| BqLog Text (Java)       | 351    | 702    | 1052   | 1399   | 1942   | 2301   | 2754   | 3229   | 3506   | 3695    |
| Log4j2 Text             | 1065   | 2583   | 4249   | 4843   | 5068   | 6195   | 6424   | 7943   | 8794   | 9254    |

<img src="img/benchmark_4_params.png" alt="Results of 4 params" style="width: 100%;">

#### Total Time Cost without parameters (ms)

An interesting phenomenon is that in "no parameter" case, Log4j2's time cost is slightly lower than its "with parameter" case, but still significantly slower than BqLog.

|                         | 1 Thread | 2 Threads | 3 Threads | 4 Threads | 5 Threads | 6 Threads | 7 Threads | 8 Threads | 9 Threads | 10 Threads |
|-------------------------|--------|--------|--------|--------|--------|--------|--------|--------|--------|---------|
| BqLog Compress (C++)    | 97     | 101    | 155    | 228    | 290    | 341    | 415    | 476    | 541    | 601     |
| BqLog Text (C++)        | 153    | 351    | 468    | 699    | 916    | 1098   | 1212   | 1498   | 1733   | 1908    |
| BqLog Compress (Java)   | 109    | 111    | 178    | 240    | 321    | 378    | 449    | 525    | 592    | 670     |
| BqLog Text (Java)       | 167    | 354    | 491    | 718    | 951    | 1139   | 1278   | 1550   | 1802   | 1985    |
| Log4j2 Text             | 3204   | 6489   | 7702   | 8485   | 9640   | 10458  | 11483  | 12853  | 13995  | 14633   |

<img src="img/benchmark_no_param.png" alt="Results of no param" style="width: 100%;">
