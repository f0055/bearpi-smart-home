@Component
export struct TempHumidityCard {
  @Prop temperature: number = 0;
  @Prop humidity: number = 0;
  @Prop isConnected: boolean = false;

  build() {
    Row() {
      Column() {
        Row() {
          Text('🌡️')
            .fontSize(20)
          Text('温度')
            .fontSize(14)
            .fontColor('#616161')
            .margin({ left: 6 })
        }
        .alignItems(VerticalAlign.Center)

        Text(this.isConnected ? `${this.temperature.toFixed(1)}℃` : '--.-℃')
          .fontSize(28)
          .fontWeight(FontWeight.Bold)
          .fontColor('#1565C0')
          .margin({ top: 8 })

        Progress({ value: this.isConnected ? this.temperature : 0, total: 50, type: ProgressType.Linear })
          .width('100%')
          .height(6)
          .color('#1565C0')
          .backgroundColor('#E3F2FD')
          .margin({ top: 8 })
      }
      .layoutWeight(1)
      .padding(16)
      .borderRadius(12)
      .backgroundColor('#FAFAFA')

      Divider()
        .vertical(true)
        .height(60)
        .color('#E0E0E0')
        .margin({ left: 12, right: 12 })

      Column() {
        Row() {
          Text('💧')
            .fontSize(20)
          Text('湿度')
            .fontSize(14)
            .fontColor('#616161')
            .margin({ left: 6 })
        }
        .alignItems(VerticalAlign.Center)

        Text(this.isConnected ? `${this.humidity.toFixed(1)}%` : '--.-%')
          .fontSize(28)
          .fontWeight(FontWeight.Bold)
          .fontColor('#00897B')
          .margin({ top: 8 })

        Progress({ value: this.isConnected ? this.humidity : 0, total: 100, type: ProgressType.Linear })
          .width('100%')
          .height(6)
          .color('#00897B')
          .backgroundColor('#E0F2F1')
          .margin({ top: 8 })
      }
      .layoutWeight(1)
      .padding(16)
      .borderRadius(12)
      .backgroundColor('#FAFAFA')
    }
    .width('100%')
    .padding({ left: 16, right: 16, top: 12, bottom: 16 })
    .margin({ left: 16, right: 16, top: 16 })
    .borderRadius(16)
    .backgroundColor(Color.White)
    .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })
  }
}
