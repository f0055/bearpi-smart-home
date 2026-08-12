@Component
export struct ConnectionBar {
  @Prop deviceIP: string = '';
  @Prop isConnected: boolean = false;
  onIPChange: (value: string) => void = () => {};
  onConnectClick: () => void = () => {};

  build() {
    Row() {
      TextInput({ placeholder: '输入开发板IP地址', text: this.deviceIP })
        .layoutWeight(1)
        .height(44)
        .borderRadius(10)
        .backgroundColor(Color.White)
        .fontSize(15)
        .onChange((value: string) => {
          this.onIPChange(value);
        })

      Button(this.isConnected ? '断开' : '连接')
        .height(44)
        .borderRadius(10)
        .margin({ left: 10 })
        .fontSize(15)
        .fontWeight(FontWeight.Medium)
        .fontColor(Color.White)
        .backgroundColor(this.isConnected ? '#EF5350' : '#43A047')
        .onClick(() => {
          this.onConnectClick();
        })
    }
    .width('100%')
    .padding({ left: 16, right: 16, top: 16, bottom: 8 })
  }
}
