import { bq } from 'bqlog';

@Entry
@Component
struct Index {
  @State logOutput: string = 'Click buttons to test BqLog';

  aboutToAppear(): void {
  }

  build() {
    Column() {
      Text('BqLog Example')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 20, bottom: 20 })

      Text(this.logOutput)
        .fontSize(14)
        .fontColor('#333333')
        .padding(16)
        .width('90%')
        .backgroundColor('#F5F5F5')
        .borderRadius(8)
        .margin({ bottom: 20 })
        .textAlign(TextAlign.Start)

      Button('Create Log Instance')
        .width('80%')
        .margin({ bottom: 12 })
        .onClick(() => {
          try {
            // Create a console appender log instance
            const log = bq.log.create('example_log', `
              appenders_config.appender_0.type=console
              appenders_config.appender_0.time_zone=8
              appenders_config.appender_0.levels=[verbose,debug,info,warning,error,fatal]
            `);
            if (log) {
              this.logOutput = 'Log instance created successfully!';
              log.verbose('This is a verbose message');
              log.debug('This is a debug message');
              log.info('This is an info message');
              log.warning('This is a warning message');
              log.error('This is an error message');
              this.logOutput += '\nLog messages sent to console.';
            } else {
              this.logOutput = 'Failed to create log instance.';
            }
          } catch (e) {
            this.logOutput = `Error: ${JSON.stringify(e)}`;
          }
        })

      Button('Test Log Levels')
        .width('80%')
        .margin({ bottom: 12 })
        .onClick(() => {
          try {
            const log = bq.log.get_log_by_name('example_log');
            if (log) {
              log.verbose('Verbose level log');
              log.debug('Debug level log');
              log.info('Info level log');
              log.warning('Warning level log');
              log.error('Error level log');
              log.fatal('Fatal level log');
              this.logOutput = 'All log levels tested.\nCheck console output for results.';
            } else {
              this.logOutput = 'Please create log instance first.';
            }
          } catch (e) {
            this.logOutput = `Error: ${JSON.stringify(e)}`;
          }
        })

    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Start)
    .alignItems(HorizontalAlign.Center)
  }
}
