interface BrandOption {
  key: string;
  label: string;
}

@Component
export struct ACControlCard {
  @Prop acOn: boolean = false;
  @Prop acMode: string = 'cool';
  @Prop acTargetTemp: number = 26;
  @Prop acBrand: string = 'midea';
  @Prop isConnected: boolean = false;
  onAcToggle: (isOn: boolean) => void = () => {};
  onModeChange: (mode: string) => void = () => {};
  onTempChange: (temp: number) => void = () => {};
  onBrandChange: (brand: string) => void = () => {};

  private brandOptions: BrandOption[] = [
    { key: 'midea', label: '美的' },
    { key: 'gree', label: '格力' },
    { key: 'haier', label: '海尔' },
    { key: 'aux', label: '奥克斯' }
  ];

  private getBrandName(key: string): string {
    if (key === 'gree') return '格力';
    if (key === 'midea') return '美的';
    if (key === 'haier') return '海尔';
    return '奥克斯';
  }

  private getModeName(mode: string): string {
    if (mode === 'cool') return '制冷';
    if (mode === 'heat') return '制热';
    if (mode === 'auto') return '自动';
    return '送风';
  }

  private getModeColor(mode: string): string {
    if (mode === 'cool') return '#1E88E5';
    if (mode === 'heat') return '#EF5350';
    if (mode === 'auto') return '#43A047';
    return '#FF8F00';
  }

  build() {
    Column() {
      Row() {
        Text('❄️')
          .fontSize(20)
        Text('空调控制')
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .fontColor('#424242')
          .margin({ left: 8 })

        if (this.isConnected) {
          Text(`· ${this.getBrandName(this.acBrand)}`)
            .fontSize(13)
            .fontColor('#9E9E9E')
            .margin({ left: 6 })
        }

        Blank()
          .layoutWeight(1)

        Toggle({ type: ToggleType.Switch, isOn: this.acOn })
          .selectedColor('#1E88E5')
          .switchPointColor(Color.White)
          .onChange((isOn: boolean) => {
            this.onAcToggle(isOn);
          })
      }
      .width('100%')
      .padding({ top: 16, left: 16, right: 16 })
      .alignItems(VerticalAlign.Center)

      Row() {
        Text('运行状态')
          .fontSize(14)
          .fontColor('#616161')
          .layoutWeight(1)

        Text(this.isConnected ? (this.acOn ? '已开启' : '已关闭') : '离线')
          .fontSize(14)
          .fontColor(this.acOn ? '#1E88E5' : '#9E9E9E')
          .fontWeight(FontWeight.Medium)
      }
      .width('100%')
      .padding({ top: 12, left: 16, right: 16 })

      Divider()
        .color('#E0E0E0')
        .margin({ top: 12, left: 16, right: 16 })

      Column() {
        Text('品牌选择')
          .fontSize(14)
          .fontColor('#616161')
          .width('100%')

        Row() {
          ForEach(this.brandOptions, (item: BrandOption) => {
            Button(item.label)
              .height(36)
              .borderRadius(8)
              .fontSize(13)
              .fontWeight(FontWeight.Medium)
              .fontColor(Color.White)
              .backgroundColor(this.acBrand === item.key ? '#1E88E5' : '#BDBDBD')
              .onClick(() => {
                this.onBrandChange(item.key);
              })
          })
        }
        .width('100%')
        .justifyContent(FlexAlign.SpaceBetween)
        .padding({ top: 10 })
      }
      .width('100%')
      .padding({ top: 12, left: 16, right: 16 })
      .alignItems(HorizontalAlign.Start)

      Divider()
        .color('#E0E0E0')
        .margin({ top: 12, left: 16, right: 16 })

      Column() {
        Text('模式选择')
          .fontSize(14)
          .fontColor('#616161')
          .width('100%')

        Row() {
          ForEach(['cool', 'heat', 'auto', 'fan'], (mode: string) => {
            Button(this.getModeName(mode))
              .height(36)
              .borderRadius(8)
              .fontSize(13)
              .fontWeight(FontWeight.Medium)
              .fontColor(Color.White)
              .backgroundColor(this.acMode === mode ? this.getModeColor(mode) : '#BDBDBD')
              .onClick(() => {
                this.onModeChange(mode);
              })
          })
        }
        .width('100%')
        .justifyContent(FlexAlign.SpaceBetween)
        .padding({ top: 10 })
      }
      .width('100%')
      .padding({ top: 12, left: 16, right: 16 })
      .alignItems(HorizontalAlign.Start)

      Divider()
        .color('#E0E0E0')
        .margin({ top: 12, left: 16, right: 16 })

      Column() {
        Row() {
          Text('目标温度')
            .fontSize(14)
            .fontColor('#616161')
            .layoutWeight(1)
        }
        .width('100%')

        Row() {
          Button('-')
            .width(48)
            .height(40)
            .borderRadius(10)
            .fontSize(22)
            .fontColor('#424242')
            .backgroundColor('#F0F0F0')
            .enabled(this.acTargetTemp > 16)
            .onClick(() => {
              if (this.acTargetTemp > 16) {
                this.onTempChange(this.acTargetTemp - 1);
              }
            })

          Text(`${this.acTargetTemp}℃`)
            .fontSize(18)
            .fontColor('#1E88E5')
            .fontWeight(FontWeight.Bold)
            .width(70)
            .textAlign(TextAlign.Center)

          Button('+')
            .width(48)
            .height(40)
            .borderRadius(10)
            .fontSize(22)
            .fontColor('#424242')
            .backgroundColor('#F0F0F0')
            .enabled(this.acTargetTemp < 30)
            .onClick(() => {
              if (this.acTargetTemp < 30) {
                this.onTempChange(this.acTargetTemp + 1);
              }
            })
        }
        .width('100%')
        .padding({ top: 10 })
        .justifyContent(FlexAlign.Center)
        .alignItems(VerticalAlign.Center)
      }
      .width('100%')
      .padding({ top: 12, left: 16, right: 16, bottom: 16 })
    }
    .width('100%')
    .margin({ left: 16, right: 16, top: 16 })
    .borderRadius(16)
    .backgroundColor(Color.White)
    .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })
    .opacity(this.isConnected ? 1 : 0.5)
  }
}
