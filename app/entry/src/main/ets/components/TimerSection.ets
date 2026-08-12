@Component
export struct TimerSection {
  @Prop timerDelay: string = '';
  @Prop timerActionIndex: number = 0;
  @Prop timerMessage: string = '';
  onDelayChange: (value: string) => void = () => {};
  onActionSelect: (index: number) => void = () => {};
  onSetTimer: () => void = () => {};

  private timerActions: string[] = ['开灯', '关灯'];

  build() {
    Column() {
      Text('定时控制')
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor('#424242')
        .width('100%')
        .padding({ top: 16, left: 16, right: 16 })

      Row() {
        Text('延迟秒数')
          .fontSize(14)
          .fontColor('#616161')
          .width(80)

        TextInput({ placeholder: '输入秒数', text: this.timerDelay })
          .layoutWeight(1)
          .height(40)
          .borderRadius(8)
          .backgroundColor('#F5F5F5')
          .type(InputType.Number)
          .fontSize(14)
          .onChange((value: string) => {
            this.onDelayChange(value);
          })
      }
      .width('100%')
      .padding({ top: 12, left: 16, right: 16 })
      .alignItems(VerticalAlign.Center)

      Row() {
        Text('执行动作')
          .fontSize(14)
          .fontColor('#616161')
          .width(80)

        Select(this.timerActions.map((item: string): SelectOption => {
          return { value: item };
        }))
          .selected(this.timerActionIndex)
          .value(this.timerActions[this.timerActionIndex])
          .layoutWeight(1)
          .height(40)
          .borderRadius(8)
          .fontColor('#424242')
          .selectedOptionFontColor('#1E88E5')
          .onSelect((_index: number, value: string) => {
            this.onActionSelect(this.timerActions.indexOf(value));
          })
      }
      .width('100%')
      .padding({ top: 12, left: 16, right: 16 })
      .alignItems(VerticalAlign.Center)

      Button('设置定时')
        .width('100%')
        .height(44)
        .borderRadius(10)
        .margin({ top: 12, left: 16, right: 16 })
        .fontSize(15)
        .fontWeight(FontWeight.Medium)
        .fontColor(Color.White)
        .backgroundColor('#1E88E5')
        .onClick(() => {
          this.onSetTimer();
        })

      if (this.timerMessage !== '') {
        Text(this.timerMessage)
          .fontSize(13)
          .fontColor('#43A047')
          .margin({ top: 8, left: 16, right: 16 })
      }
    }
    .width('100%')
    .padding({ bottom: 16 })
    .margin({ left: 16, right: 16, top: 16 })
    .borderRadius(16)
    .backgroundColor(Color.White)
    .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })
  }
}
