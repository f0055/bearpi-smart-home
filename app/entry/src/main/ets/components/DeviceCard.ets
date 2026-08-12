@Component
export struct DeviceCard {
  @Prop lightOn: boolean = false;
  @Prop uptime: number = 0;
  @Prop isConnected: boolean = false;

  private formatUptime(seconds: number): string {
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const secs = seconds % 60;

    if (hours > 0) {
      return `${hours}时${minutes}分${secs}秒`;
    } else if (minutes > 0) {
      return `${minutes}分${secs}秒`;
    } else {
      return `${secs}秒`;
    }
  }

  build() {
    Row() {
      Column() {
        if (this.lightOn) {
          Stack() {
            Circle({ width: 60, height: 60 })
              .fill('#FFF9C4')
              .opacity(0.4)
            Circle({ width: 48, height: 48 })
              .fill('#FFEE58')
            Text('💡')
              .fontSize(32)
          }
          .width(64)
          .height(64)
        } else {
          Stack() {
            Circle({ width: 48, height: 48 })
              .fill('#E0E0E0')
            Text('⚫')
              .fontSize(28)
          }
          .width(64)
          .height(64)
        }
      }
      .width(80)
      .height(80)
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center)

      Column() {
        Text('智能灯具')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#212121')

        Row() {
          Circle({ width: 8, height: 8 })
            .fill(this.isConnected ? '#43A047' : '#9E9E9E')
          Text(this.isConnected ? '在线' : '离线')
            .fontSize(13)
            .fontColor(this.isConnected ? '#43A047' : '#9E9E9E')
            .margin({ left: 6 })
        }
        .margin({ top: 6 })

        if (this.isConnected) {
          Text(`运行时长: ${this.formatUptime(this.uptime)}`)
            .fontSize(12)
            .fontColor('#757575')
            .margin({ top: 4 })
        }
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)
      .margin({ left: 12 })

      Column() {
        Text(this.lightOn ? '已开启' : '已关闭')
          .fontSize(14)
          .fontColor(this.lightOn ? '#FF8F00' : '#9E9E9E')
          .fontWeight(FontWeight.Medium)
      }
      .alignItems(HorizontalAlign.End)
    }
    .width('100%')
    .padding(16)
    .margin({ left: 16, right: 16 })
    .borderRadius(16)
    .backgroundColor(Color.White)
    .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })
  }
}
