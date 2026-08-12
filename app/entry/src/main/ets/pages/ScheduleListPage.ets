import router from '@ohos.router';
import { sharedUdpClient, ScheduleItem, ScheduleEditData } from '../components/UdpClient';
import { ScheduleEditDialog } from '../components/ScheduleEditDialog';

@Entry
@Component
struct ScheduleListPage {
  @StorageLink('schedules') schedules: ScheduleItem[] = [];
  @State showEditor: boolean = false;
  @State editData: ScheduleEditData = new ScheduleEditData();

  aboutToAppear(): void {
    AppStorage.setOrCreate('schedules', this.schedules);
  }

  private formatTime(hour: number, minute: number): string {
    return `${hour.toString().padStart(2, '0')}:${minute.toString().padStart(2, '0')}`;
  }

  private getActiveSchedules(): ScheduleItem[] {
    const result: ScheduleItem[] = [];
    for (let i = 0; i < this.schedules.length; i++) {
      const s = this.schedules[i];
      if (s.enabled || (s.start_hour !== 0 || s.start_minute !== 0 || s.end_hour !== 0 || s.end_minute !== 0)) {
        result.push(s);
      }
    }
    return result;
  }

  private openEdit(schedule: ScheduleItem): void {
    this.editData.loadFrom(schedule);
    this.showEditor = true;
  }

  private saveSchedule(): void {
    let maxIdx = -1;
    for (let i = 0; i < this.schedules.length; i++) {
      if (this.schedules[i].index > maxIdx) {
        maxIdx = this.schedules[i].index;
      }
    }
    const idx = this.editData.index >= 0 ? this.editData.index : maxIdx + 1;
    const schedule: ScheduleItem = {
      index: idx,
      enabled: this.editData.enabled,
      start_hour: this.editData.startHour,
      start_minute: this.editData.startMinute,
      end_hour: this.editData.endHour,
      end_minute: this.editData.endMinute
    };
    sharedUdpClient.sendSchedule(schedule);

    const newList: ScheduleItem[] = [];
    for (let i = 0; i < this.schedules.length; i++) {
      newList.push(this.schedules[i]);
    }
    if (idx < newList.length) {
      newList[idx] = schedule;
    } else {
      newList.push(schedule);
    }
    this.schedules = newList;
    this.showEditor = false;
  }

  private deleteSchedule(): void {
    if (this.editData.index < 0) {
      this.showEditor = false;
      return;
    }
    const idx = this.editData.index;
    sharedUdpClient.sendScheduleDelete(idx);

    const newList: ScheduleItem[] = [];
    for (let i = 0; i < this.schedules.length; i++) {
      if (this.schedules[i].index !== idx) {
        newList.push(this.schedules[i]);
      }
    }
    this.schedules = newList;
    this.showEditor = false;
  }

  build() {
    Stack() {
      Column() {
        Row() {
          Text('<')
            .fontSize(20)
            .fontColor('#1E88E5')
            .width(40)
            .height(40)
            .textAlign(TextAlign.Center)
            .onClick(() => {
              router.back();
            })

          Text('时间计划记录')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#212121')
            .layoutWeight(1)

          Button('+ 添加')
            .height(32)
            .borderRadius(8)
            .fontSize(13)
            .fontColor(Color.White)
            .backgroundColor('#43A047')
            .onClick(() => {
              this.editData.reset();
              this.showEditor = true;
            })
        }
        .width('100%')
        .height(56)
        .padding({ left: 8, right: 16 })
        .alignItems(VerticalAlign.Center)
        .backgroundColor(Color.White)

        Divider()
          .color('#E0E0E0')

        if (this.getActiveSchedules().length === 0) {
          Column() {
            Text('暂无时间计划')
              .fontSize(16)
              .fontColor('#9E9E9E')
          }
          .width('100%')
          .layoutWeight(1)
          .justifyContent(FlexAlign.Start)
          .alignItems(HorizontalAlign.Center)
          .padding({ top: 40 })
        } else {
          Scroll() {
            Column() {
              ForEach(this.getActiveSchedules(), (schedule: ScheduleItem, index: number) => {
                Row() {
                  Column() {
                    Row() {
                      Text(schedule.enabled ? '✅' : '⬜')
                        .fontSize(18)
                        .margin({ right: 8 })

                      Text(`计划 ${schedule.index + 1}`)
                        .fontSize(15)
                        .fontWeight(FontWeight.Medium)
                        .fontColor('#212121')
                    }
                    .alignItems(VerticalAlign.Center)

                    Text(`${this.formatTime(schedule.start_hour, schedule.start_minute)} - ${this.formatTime(schedule.end_hour, schedule.end_minute)}`)
                      .fontSize(20)
                      .fontWeight(FontWeight.Bold)
                      .fontColor(schedule.enabled ? '#1E88E5' : '#9E9E9E')
                      .margin({ top: 6 })
                  }
                  .alignItems(HorizontalAlign.Start)
                  .layoutWeight(1)

                  Column() {
                    Text(schedule.enabled ? '已启用' : '未启用')
                      .fontSize(12)
                      .fontColor(schedule.enabled ? '#43A047' : '#9E9E9E')
                      .backgroundColor(schedule.enabled ? '#E8F5E9' : '#F5F5F5')
                      .borderRadius(12)
                      .padding({ left: 10, right: 10, top: 4, bottom: 4 })

                    Row() {
                      Text('编辑')
                        .fontSize(12)
                        .fontColor('#1E88E5')
                        .margin({ top: 10, right: 12 })
                        .onClick(() => {
                          this.openEdit(schedule);
                        })

                      Text('删除')
                        .fontSize(12)
                        .fontColor('#EF5350')
                        .margin({ top: 10 })
                        .onClick(() => {
                          sharedUdpClient.sendScheduleDelete(schedule.index);
                          const newList: ScheduleItem[] = [];
                          for (let i = 0; i < this.schedules.length; i++) {
                            if (this.schedules[i].index !== schedule.index) {
                              newList.push(this.schedules[i]);
                            }
                          }
                          this.schedules = newList;
                        })
                    }
                  }
                  .alignItems(HorizontalAlign.End)
                }
                .width('100%')
                .padding({ top: 16, bottom: 16, left: 20, right: 20 })
                .backgroundColor(Color.White)
                .borderRadius(12)
                .margin({ left: 16, right: 16, top: 12 })
                .shadow({ radius: 4, color: '#0D000000', offsetX: 0, offsetY: 1 })
              })
            }
            .width('100%')
            .alignItems(HorizontalAlign.Start)
            .padding({ top: 4, bottom: 20 })
          }
          .layoutWeight(1)
          .scrollBar(BarState.Off)
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F0F2F5')

      if (this.showEditor) {
        ScheduleEditDialog({
          show: $showEditor,
          data: $editData,
          showDelete: this.editData.index >= 0,
          onSave: () => { this.saveSchedule(); },
          onDelete: () => { this.deleteSchedule(); },
          onCancel: () => { this.showEditor = false; }
        })
      }
    }
    .width('100%')
    .height('100%')
  }
}
