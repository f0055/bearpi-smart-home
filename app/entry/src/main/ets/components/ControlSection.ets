@Component
export struct ControlSection {
  @Prop lightOn: boolean = false;
  @Prop lightBrightness: number = 0;
  @Prop isConnected: boolean = false;
  onLightToggle: (isOn: boolean) => void = () => {};
  onBrightnessChange: (value: number) => void = () => {};

  build() {
    Column() {
      Row() {
        Text('电源开关')
          .fontSize(16)
          .fontColor('#424242')
          .layoutWeight(1)

        Text(this.lightOn ? '已开启' : '已关闭')
          .fontSize(13)
          .fontColor(this.lightOn ? '#43A047' : '#9E9E9E')
          .margin({ right: 12 })

        Toggle({ type: ToggleType.Switch, isOn: this.lightOn })
          .selectedColor('#FFC107')
          .switchPointColor(Color.White)
          .onChange((isOn: boolean) => {
            this.onLightToggle(isOn);
          })
      }
      .width('100%')
      .padding({ top: 16, bottom: 12, left: 16, right: 16 })
      .alignItems(VerticalAlign.Center)

      Divider()
        .color('#E0E0E0')
        .margin({ left: 16, right: 16 })

      Column() {
        Row() {
          Text('亮度调节')
            .fontSize(16)
            .fontColor('#424242')
            .layoutWeight(1)

          Text(`${this.lightBrightness}%`)
            .fontSize(16)
            .fontColor('#1E88E5')
            .fontWeight(FontWeight.Bold)
        }
        .width('100%')
        .padding({ top: 12 })

        Slider({
          value: this.lightBrightness,
          min: 0,
          max: 100,
          step: 1
        })
          .blockColor('#1E88E5')
          .trackColor('#E0E0E0')
          .selectedColor('#1E88E5')
          .margin({ top: 8 })
          .onChange((value: number, mode: SliderChangeMode) => {
            this.onBrightnessChange(Math.round(value));
          })
      }
      .width('100%')
      .padding({ left: 16, right: 16, bottom: 16 })
    }
    .width('100%')
    .margin({ left: 16, right: 16, top: 16 })
    .borderRadius(16)
    .backgroundColor(Color.White)
    .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })
  }
}
