@Component
export struct TitleBar {
  @Prop statusText: string = '未连接';

  build() {
    Row() {
      Column() {
        Text('智能照明/门锁/感应 控制系统')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .fontColor(Color.White)

        Text(this.statusText)
          .fontSize(12)
          .fontColor('#B3FFFFFF')
          .margin({ top: 2 })
      }
      .alignItems(HorizontalAlign.Start)
    }
    .width('100%')
    .height(64)
    .linearGradient({
      direction: GradientDirection.Right,
      colors: [['#1565C0', 0], ['#1E88E5', 1]]
    })
    .padding({ left: 20, right: 20 })
    .alignItems(VerticalAlign.Center)
  }
}
