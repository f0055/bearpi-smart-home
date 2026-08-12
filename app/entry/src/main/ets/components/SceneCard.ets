interface SceneOption {
  key: string;
  icon: string;
  label: string;
}

@Component
export struct SceneCard {
  @Prop scene: string = '';
  onSceneChange: (scene: string) => void = () => {};

  private sceneOptions: SceneOption[] = [
    { key: 'home', icon: '🏠', label: '回家模式' },
    { key: 'sleep', icon: '🌙', label: '睡眠模式' },
    { key: 'away', icon: '🚪', label: '离家模式' }
  ];

  build() {
    Row() {
      ForEach(this.sceneOptions, (item: SceneOption) => {
        Column() {
          Text(item.icon)
            .fontSize(28)

          Text(item.label)
            .fontSize(13)
            .fontWeight(this.scene === item.key ? FontWeight.Bold : FontWeight.Normal)
            .fontColor(this.scene === item.key ? '#1E88E5' : '#424242')
            .margin({ top: 6 })
        }
        .layoutWeight(1)
        .padding({ top: 14, bottom: 14 })
        .borderRadius(12)
        .backgroundColor(this.scene === item.key ? '#E3F2FD' : '#F5F5F5')
        .alignItems(HorizontalAlign.Center)
        .onClick(() => {
          this.onSceneChange(item.key);
        })
      })
    }
    .width('100%')
    .margin({ left: 16, right: 16, top: 16 })
    .borderRadius(16)
    .backgroundColor(Color.White)
    .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })
  }
}
