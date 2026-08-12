import { DiscoveryInfo } from './DiscoveryListener';

@Component
export struct DeviceDiscoveryTab {
  @Prop discoveredDevices: DiscoveryInfo[] = [];
  @Prop broadcastOn: boolean = false;
  @Prop isConnected: boolean = false;
  onBroadcastToggle: (isOn: boolean) => void = () => {};
  onDeviceClick: (ip: string) => void = () => {};

  build() {
    Column() {
      Row() {
        Text('📡')
          .fontSize(20)
        Text('设备发现')
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .fontColor('#424242')
          .margin({ left: 8 })
          .layoutWeight(1)

        Text('UDP:9999')
          .fontSize(12)
          .fontColor('#9E9E9E')

        Text(this.broadcastOn ? '已开启' : '已关闭')
          .fontSize(12)
          .fontColor(this.broadcastOn ? '#43A047' : '#9E9E9E')
          .margin({ left: 8 })

        Toggle({ type: ToggleType.Switch, isOn: this.broadcastOn })
          .selectedColor('#43A047')
          .switchPointColor(Color.White)
          .onChange((isOn: boolean) => {
            this.onBroadcastToggle(isOn);
          })
      }
      .width('100%')
      .padding({ top: 16, left: 16, right: 16 })

      if (this.discoveredDevices.length === 0) {
        Column() {
          Text('🔍')
            .fontSize(48)
            .margin({ top: 24 })

          Text('正在搜索局域网设备...')
            .fontSize(14)
            .fontColor('#9E9E9E')
            .margin({ top: 12 })

          Text('请确保设备已上电并在同一局域网')
            .fontSize(12)
            .fontColor('#BDBDBD')
            .margin({ top: 8 })
        }
        .width('100%')
        .padding({ top: 20, bottom: 40 })
        .alignItems(HorizontalAlign.Center)

      } else {
        ForEach(this.discoveredDevices, (device: DiscoveryInfo) => {
          Row() {
            Column() {
              Text(device.device)
                .fontSize(16)
                .fontWeight(FontWeight.Bold)
                .fontColor('#212121')

              Text(device.ip)
                .fontSize(13)
                .fontColor('#757575')
                .margin({ top: 4 })

              Row() {
                Circle({ width: 8, height: 8 })
                  .fill(device.status === 'online' ? '#43A047' : '#EF5350')
                Text(device.status === 'online' ? '在线' : '离线')
                  .fontSize(12)
                  .fontColor(device.status === 'online' ? '#43A047' : '#EF5350')
                  .margin({ left: 6 })
              }
              .margin({ top: 6 })
            }
            .alignItems(HorizontalAlign.Start)
            .layoutWeight(1)

            Column() {
              Row() {
                Text('🌡️')
                  .fontSize(14)
                Text(`${device.temperature.toFixed(1)}℃`)
                  .fontSize(13)
                  .fontColor('#1565C0')
                  .margin({ left: 4 })
              }

              Row() {
                Text('💧')
                  .fontSize(14)
                Text(`${device.humidity.toFixed(1)}%`)
                  .fontSize(13)
                  .fontColor('#00897B')
                  .margin({ left: 4 })
              }
              .margin({ top: 4 })

              Row() {
                Text(device.locked ? '🔒' : '🔓')
                  .fontSize(14)
                Text(device.locked ? '锁定' : '解锁')
                  .fontSize(12)
                  .fontColor(device.locked ? '#EF5350' : '#43A047')
                  .margin({ left: 4 })
              }
              .margin({ top: 4 })
            }
            .alignItems(HorizontalAlign.End)
          }
          .width('100%')
          .padding(16)
          .margin({ left: 16, right: 16, top: 12 })
          .borderRadius(12)
          .backgroundColor(Color.White)
          .shadow({ radius: 4, color: '#0D000000', offsetX: 0, offsetY: 1 })
          .onClick(() => {
            this.onDeviceClick(device.ip);
          })
        })
      }
    }
    .width('100%')
    .margin({ left: 16, right: 16, top: 16, bottom: 16 })
  }
}
