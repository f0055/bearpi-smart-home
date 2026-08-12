import { ScheduleItem, ScheduleEditData } from './UdpClient';

@Component
export struct ScheduleEditDialog {
  @Link show: boolean;
  @Link data: ScheduleEditData;
  @Prop showDelete: boolean = false;
  onSave: () => void = () => {};
  onDelete: () => void = () => {};
  onCancel: () => void = () => {};

  private formatTime(hour: number, minute: number): string {
    return `${hour.toString().padStart(2, '0')}:${minute.toString().padStart(2, '0')}`;
  }

  build() {
    if (!this.show) {
      Column() {}
    } else {
      Column() {
        Column() {
          Row() {
            Text(this.data.index >= 0 ? `编辑计划 ${this.data.index + 1}` : '添加新计划')
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .fontColor('#212121')
              .layoutWeight(1)

            Text('✕')
              .fontSize(18)
              .fontColor('#9E9E9E')
              .onClick(() => {
                this.onCancel();
              })
          }
          .width('100%')
          .padding({ left: 16, right: 16, top: 16 })

          Row() {
            Text('启用')
              .fontSize(14)
              .fontColor('#424242')
              .layoutWeight(1)

            Toggle({ type: ToggleType.Switch, isOn: this.data.enabled })
              .selectedColor('#43A047')
              .switchPointColor(Color.White)
              .onChange((isOn: boolean) => {
                this.data.enabled = isOn;
              })
          }
          .width('100%')
          .padding({ top: 12, left: 16, right: 16 })
          .alignItems(VerticalAlign.Center)

          Row() {
            Text('开始')
              .fontSize(14)
              .fontColor('#424242')
              .width(40)

            Text(this.formatTime(this.data.startHour, this.data.startMinute))
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .fontColor('#1E88E5')
              .width(52)

            Text('-')
              .width(30)
              .height(30)
              .textAlign(TextAlign.Center)
              .fontSize(18)
              .fontColor('#666666')
              .backgroundColor('#F0F0F0')
              .borderRadius(6)
              .onClick(() => { this.data.startHour = (this.data.startHour + 23) % 24; })

            Text('+')
              .width(30)
              .height(30)
              .textAlign(TextAlign.Center)
              .fontSize(18)
              .fontColor('#666666')
              .backgroundColor('#F0F0F0')
              .borderRadius(6)
              .margin({ left: 4 })
              .onClick(() => { this.data.startHour = (this.data.startHour + 1) % 24; })

            Text('时')
              .fontSize(12)
              .fontColor('#9E9E9E')
              .margin({ left: 2 })

            Text('-')
              .width(30)
              .height(30)
              .textAlign(TextAlign.Center)
              .fontSize(18)
              .fontColor('#666666')
              .backgroundColor('#F0F0F0')
              .borderRadius(6)
              .margin({ left: 8 })
              .onClick(() => { this.data.startMinute = (this.data.startMinute + 55) % 60; })

            Text('+')
              .width(30)
              .height(30)
              .textAlign(TextAlign.Center)
              .fontSize(18)
              .fontColor('#666666')
              .backgroundColor('#F0F0F0')
              .borderRadius(6)
              .margin({ left: 4 })
              .onClick(() => { this.data.startMinute = (this.data.startMinute + 5) % 60; })

            Text('分')
              .fontSize(12)
              .fontColor('#9E9E9E')
              .margin({ left: 2 })
          }
          .width('100%')
          .padding({ top: 12, left: 16, right: 16 })
          .alignItems(VerticalAlign.Center)

          Row() {
            Text('结束')
              .fontSize(14)
              .fontColor('#424242')
              .width(40)

            Text(this.formatTime(this.data.endHour, this.data.endMinute))
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .fontColor('#EF5350')
              .width(52)

            Text('-')
              .width(30)
              .height(30)
              .textAlign(TextAlign.Center)
              .fontSize(18)
              .fontColor('#666666')
              .backgroundColor('#F0F0F0')
              .borderRadius(6)
              .onClick(() => { this.data.endHour = (this.data.endHour + 23) % 24; })

            Text('+')
              .width(30)
              .height(30)
              .textAlign(TextAlign.Center)
              .fontSize(18)
              .fontColor('#666666')
              .backgroundColor('#F0F0F0')
              .borderRadius(6)
              .margin({ left: 4 })
              .onClick(() => { this.data.endHour = (this.data.endHour + 1) % 24; })

            Text('时')
              .fontSize(12)
              .fontColor('#9E9E9E')
              .margin({ left: 2 })

            Text('-')
              .width(30)
              .height(30)
              .textAlign(TextAlign.Center)
              .fontSize(18)
              .fontColor('#666666')
              .backgroundColor('#F0F0F0')
              .borderRadius(6)
              .margin({ left: 8 })
              .onClick(() => { this.data.endMinute = (this.data.endMinute + 55) % 60; })

            Text('+')
              .width(30)
              .height(30)
              .textAlign(TextAlign.Center)
              .fontSize(18)
              .fontColor('#666666')
              .backgroundColor('#F0F0F0')
              .borderRadius(6)
              .margin({ left: 4 })
              .onClick(() => { this.data.endMinute = (this.data.endMinute + 5) % 60; })

            Text('分')
              .fontSize(12)
              .fontColor('#9E9E9E')
              .margin({ left: 2 })
          }
          .width('100%')
          .padding({ top: 8, left: 16, right: 16 })
          .alignItems(VerticalAlign.Center)

          Row() {
            if (this.showDelete) {
              Button('删除')
                .height(40)
                .borderRadius(10)
                .layoutWeight(1)
                .margin({ right: 8 })
                .fontSize(14)
                .fontColor('#EF5350')
                .backgroundColor('#FFEBEE')
                .onClick(() => {
                  this.onDelete();
                })
            }

            Button('保存')
              .height(40)
              .borderRadius(10)
              .layoutWeight(1)
              .margin(this.showDelete ? { left: 8 } : { left: 16, right: 16 })
              .fontSize(14)
              .fontWeight(FontWeight.Medium)
              .fontColor(Color.White)
              .backgroundColor('#43A047')
              .onClick(() => {
                this.onSave();
              })
          }
          .width('100%')
          .padding({ top: 16, left: 16, right: 16, bottom: 16 })
          .alignItems(VerticalAlign.Center)
        }
        .width(300)
        .backgroundColor(Color.White)
        .borderRadius(16)
        .shadow({ radius: 12, color: '#3D000000', offsetX: 0, offsetY: 4 })
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#3D000000')
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center)
      .onClick(() => {
        this.onCancel();
      })
    }
  }
}
