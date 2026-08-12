import { ScheduleItem } from './UdpClient';
import router from '@ohos.router';

@Component
export struct DoorLockCard {
  @Prop locked: boolean = true;
  @Prop manual: boolean = false;
  @Prop autoUnlock: boolean = false;
  @Prop deviceTime: string = '';
  @Prop pirEvent: string = '';
  @Prop timeSchedules: ScheduleItem[] = [];
  @Prop isConnected: boolean = false;
  onLock: () => void = () => {};
  onUnlock: () => void = () => {};
  onAutoUnlockToggle: (enable: boolean) => void = () => {};
  onAddSchedule: () => void = () => {};
  onEditSchedule: (schedule: ScheduleItem, index: number) => void = () => {};

  private getSchedules(): ScheduleItem[] {
    const schedules: ScheduleItem[] = [];
    for (let i = 0; i < 5; i++) {
      if (i < this.timeSchedules.length) {
        schedules.push(this.timeSchedules[i]);
      } else {
        schedules.push({
          index: i,
          enabled: false,
          start_hour: 0,
          start_minute: 0,
          end_hour: 0,
          end_minute: 0
        });
      }
    }
    return schedules;
  }

  private formatTime(hour: number, minute: number): string {
    return `${hour.toString().padStart(2, '0')}:${minute.toString().padStart(2, '0')}`;
  }

  build() {
    Column() {
      Row() {
        Column() {
          Text(this.locked ? '🔒' : '🔓')
            .fontSize(40)
        }
        .width(80)
        .height(80)
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)

        Column() {
          Text('智能门锁')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#212121')

          Row() {
            Text(this.isConnected ? (this.locked ? '已锁定' : '已解锁') : '离线')
              .fontSize(13)
              .fontColor(this.locked ? '#EF5350' : '#43A047')

            if (this.isConnected) {
              Text(this.manual ? ' · 手动' : ' · 自动')
                .fontSize(13)
                .fontColor('#9E9E9E')
            }
          }
          .margin({ top: 6 })

          if (this.isConnected && this.deviceTime !== '') {
            Text(`设备时间: ${this.deviceTime}`)
              .fontSize(12)
              .fontColor('#757575')
              .margin({ top: 4 })
          }

          if (this.isConnected) {
            Text(this.pirEvent === 'detected' ? '⚡ 感应到有人' : '未检测到人')
              .fontSize(13)
              .fontColor(this.pirEvent === 'detected' ? '#FF6F00' : '#9E9E9E')
              .margin({ top: 4 })
          }
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)
        .margin({ left: 12 })

        Button(this.locked ? '解锁' : '锁定')
          .height(40)
          .borderRadius(10)
          .fontSize(14)
          .fontWeight(FontWeight.Medium)
          .fontColor(Color.White)
          .backgroundColor(this.locked ? '#43A047' : '#EF5350')
          .onClick(() => {
            if (!this.isConnected) return;
            if (this.locked) {
              this.onUnlock();
            } else {
              this.onLock();
            }
          })
      }
      .width('100%')

      Divider()
        .color('#E0E0E0')
        .margin({ top: 12, left: 16, right: 16 })

      Row() {
        Text('自动解锁')
          .fontSize(14)
          .fontColor('#424242')
          .layoutWeight(1)

        Toggle({ type: ToggleType.Switch, isOn: this.autoUnlock })
          .selectedColor('#43A047')
          .switchPointColor(Color.White)
          .onChange((isOn: boolean) => {
            if (!this.isConnected) return;
            this.onAutoUnlockToggle(isOn);
          })
      }
      .width('100%')
      .padding({ top: 12, left: 16, right: 16 })
      .alignItems(VerticalAlign.Center)

      Divider()
        .color('#E0E0E0')
        .margin({ top: 12, left: 16, right: 16 })

      Row() {
        Text('时间计划')
          .fontSize(14)
          .fontWeight(FontWeight.Bold)
          .fontColor('#424242')
          .layoutWeight(1)

        Button('+ 添加')
          .height(28)
          .borderRadius(6)
          .fontSize(12)
          .fontColor(Color.White)
          .backgroundColor('#43A047')
          .onClick(() => {
            if (!this.isConnected) return;
            this.onAddSchedule();
          })
      }
      .width('100%')
      .padding({ top: 12, left: 16, right: 16 })

      Row() {
        Text('查看全部计划 >')
          .fontSize(13)
          .fontColor('#1E88E5')
          .onClick(() => {
            router.pushUrl({
              url: 'pages/ScheduleListPage',
              params: { timeSchedules: this.timeSchedules }
            });
          })
      }
      .width('100%')
      .padding({ top: 8, bottom: 4, left: 16 })
    }
    .width('100%')
    .padding({ top: 16, bottom: 16, left: 16, right: 16 })
    .margin({ left: 16, right: 16, top: 16 })
    .borderRadius(16)
    .backgroundColor(Color.White)
    .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })
  }
}
