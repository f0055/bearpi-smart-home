@Component
export struct PIRCard {
  @Prop pir: boolean = false;
  @Prop isConnected: boolean = false;

  build() {
    Row() {
      Column() {
        Text('🚶')
          .fontSize(40)
      }
      .width(80)
      .height(80)
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center)

      Column() {
        Text('人体感应')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#212121')

        Text(this.isConnected ? (this.pir ? '已检测到有人' : '无人') : '离线')
          .fontSize(13)
          .fontColor(this.pir ? '#FF8F00' : '#9E9E9E')
          .margin({ top: 6 })
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)
      .margin({ left: 12 })

      Circle({ width: 12, height: 12 })
        .fill(this.pir ? '#FF8F00' : '#E0E0E0')
    }
    .width('100%')
    .padding(16)
    .margin({ left: 16, right: 16, top: 16 })
    .borderRadius(16)
    .backgroundColor(Color.White)
    .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })
  }
}
