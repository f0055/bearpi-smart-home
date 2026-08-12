import { UdpClient, sharedUdpClient, DeviceStatus, ScheduleItem, ScheduleEditData } from '../components/UdpClient';
import { DiscoveryListener, DiscoveryInfo } from '../components/DiscoveryListener';
import { TitleBar } from '../components/TitleBar';
import { ConnectionBar } from '../components/ConnectionBar';
import { DeviceCard } from '../components/DeviceCard';
import { SceneCard } from '../components/SceneCard';
import { ControlSection } from '../components/ControlSection';
import { TimerSection } from '../components/TimerSection';
import { DoorLockCard } from '../components/DoorLockCard';
import { PIRCard } from '../components/PIRCard';
import { TempHumidityCard } from '../components/TempHumidityCard';
import { ACControlCard } from '../components/ACControlCard';
import { DeviceDiscoveryTab } from '../components/DeviceDiscoveryTab';
import { ScheduleEditDialog } from '../components/ScheduleEditDialog';

@Entry
@Component
struct Index {
  @State deviceIP: string = '';
  @State isConnected: boolean = false;
  @State statusText: string = '未连接';

  @State lightOn: boolean = false;
  @State lightBrightness: number = 0;
  @State uptime: number = 0;
  @State locked: boolean = true;
  @State pir: boolean = false;
  @State pirEvent: string = '';
  @State manual: boolean = false;
  @State doorOpen: boolean = false;
  @State temperature: number = 0;
  @State humidity: number = 0;
  @State acOn: boolean = false;
  @State acMode: string = 'cool';
  @State acTargetTemp: number = 26;
  @State acBrand: string = 'midea';
  @State scene: string = '';
  @State autoUnlock: boolean = false;
  @State deviceTime: string = '';
  @State scheduleCount: number = 0;
  @StorageLink('schedules') timeSchedules: ScheduleItem[] = [];

  @State timerDelay: string = '';
  @State timerActionIndex: number = 0;
  @State timerMessage: string = '';

  @State showScheduleEditor: boolean = false;
  @State editData: ScheduleEditData = new ScheduleEditData();

  @State currentTab: number = 0;
  @State discoveredDevices: DiscoveryInfo[] = [];
  @State broadcastOn: boolean = false;

  private udpClient: UdpClient = sharedUdpClient;
  private discoveryListener: DiscoveryListener = new DiscoveryListener();

  aboutToAppear(): void {
    AppStorage.setOrCreate('schedules', []);
    this.startDiscovery();
  }

  aboutToDisappear(): void {
    this.stopDiscovery();
  }

  async startDiscovery(): Promise<void> {
    await this.discoveryListener.start((data: string) => {
      this.handleDiscoveryMessage(data);
    });
  }

  async stopDiscovery(): Promise<void> {
    await this.discoveryListener.stop();
  }

  handleDiscoveryMessage(data: string) {
    try {
      const json = JSON.parse(data) as DiscoveryInfo;
      const now = Date.now();
      const existingIndex = this.discoveredDevices.findIndex((d: DiscoveryInfo) => d.ip === json.ip);

      if (existingIndex >= 0) {
        this.discoveredDevices[existingIndex] = {
          device: json.device,
          ip: json.ip,
          status: json.status,
          locked: json.locked,
          temperature: json.temperature,
          humidity: json.humidity,
          lastSeen: now
        };
      } else {
        this.discoveredDevices.push({
          device: json.device,
          ip: json.ip,
          status: json.status,
          locked: json.locked,
          temperature: json.temperature,
          humidity: json.humidity,
          lastSeen: now
        });
      }
    } catch (err) {
    }
  }

  build() {
    Stack() {
      Column() {
        TitleBar({ statusText: this.statusText })

        Tabs({ barPosition: BarPosition.End, index: this.currentTab }) {
          TabContent() {
            Scroll() {
              Column() {
                ConnectionBar({
                  deviceIP: this.deviceIP,
                  isConnected: this.isConnected,
                  onIPChange: (value: string) => { this.deviceIP = value; },
                  onConnectClick: () => {
                    if (this.isConnected) {
                      this.disconnectDevice();
                    } else {
                      this.connectDevice();
                    }
                  }
                })

                DeviceCard({
                  lightOn: this.lightOn,
                  uptime: this.uptime,
                  isConnected: this.isConnected
                })

                SceneCard({
                  scene: this.scene,
                  onSceneChange: (scene: string) => {
                    if (this.isConnected) {
                      this.udpClient.sendScene(scene);
                    }
                  }
                })

                ControlSection({
                  lightOn: this.lightOn,
                  lightBrightness: this.lightBrightness,
                  isConnected: this.isConnected,
                  onLightToggle: (isOn: boolean) => {
                    if (this.isConnected) {
                      this.udpClient.send(isOn ? '{"action":"on"}' : '{"action":"off"}');
                    }
                    this.lightOn = isOn;
                    if (isOn) {
                      this.lightBrightness = 100;
                    }
                  },
                  onBrightnessChange: (value: number) => {
                    this.lightBrightness = value;
                    if (this.isConnected) {
                      this.udpClient.sendBrightness(value);
                    }
                  }
                })

                TimerSection({
                  timerDelay: this.timerDelay,
                  timerActionIndex: this.timerActionIndex,
                  timerMessage: this.timerMessage,
                  onDelayChange: (value: string) => { this.timerDelay = value; },
                  onActionSelect: (index: number) => { this.timerActionIndex = index; },
                  onSetTimer: () => { this.setTimer(); }
                })

                DoorLockCard({
                  locked: this.locked,
                  manual: this.manual,
                  isConnected: this.isConnected,
                  autoUnlock: this.autoUnlock,
                  deviceTime: this.deviceTime,
                  pirEvent: this.pirEvent,
                  timeSchedules: this.timeSchedules,
                  onLock: () => { this.udpClient.sendLock(); },
                  onUnlock: () => { this.udpClient.sendUnlock(); },
                  onAutoUnlockToggle: (enable: boolean) => {
                    this.autoUnlock = enable;
                    this.udpClient.sendAutoUnlock(enable);
                  },
                  onAddSchedule: () => {
                    this.editData.reset();
                    this.showScheduleEditor = true;
                  },
                  onEditSchedule: (schedule: ScheduleItem, index: number) => {
                    this.editData.loadFrom(schedule);
                    this.showScheduleEditor = true;
                  }
                })

                PIRCard({
                  pir: this.pir,
                  isConnected: this.isConnected
                })

                TempHumidityCard({
                  temperature: this.temperature,
                  humidity: this.humidity,
                  isConnected: this.isConnected
                })

                ACControlCard({
                  acOn: this.acOn,
                  acMode: this.acMode,
                  acTargetTemp: this.acTargetTemp,
                  acBrand: this.acBrand,
                  isConnected: this.isConnected,
                onAcToggle: (isOn: boolean) => {
                  if (isOn) {
                    this.udpClient.sendAcOn();
                  } else {
                    this.udpClient.sendAcOff();
                  }
                },
                onModeChange: (mode: string) => {
                  this.acMode = mode;
                  this.udpClient.sendAcMode(mode);
                },
                onTempChange: (temp: number) => {
                  this.acTargetTemp = temp;
                  this.udpClient.sendAcTargetTemp(temp);
                },
                onBrandChange: (brand: string) => {
                  this.acBrand = brand;
                  this.udpClient.sendAcBrand(brand);
                }
                })
              }
              .width('100%')
            }
            .layoutWeight(1)
            .scrollBar(BarState.Off)
          }
          .tabBar(this.TabBarBuilder('设备控制', 0))

          TabContent() {
            Scroll() {
              Column() {
                DeviceDiscoveryTab({
                  discoveredDevices: this.discoveredDevices,
                  broadcastOn: this.broadcastOn,
                  isConnected: this.isConnected,
                  onBroadcastToggle: (isOn: boolean) => {
                    this.broadcastOn = isOn;
                    if (this.isConnected) {
                      this.udpClient.sendBroadcast(isOn);
                    }
                  },
                  onDeviceClick: (ip: string) => {
                    this.deviceIP = ip;
                    this.connectDevice();
                  }
                })
              }
              .width('100%')
            }
            .layoutWeight(1)
            .scrollBar(BarState.Off)
          }
          .tabBar(this.TabBarBuilder('设备发现', 1))
        }
        .onChange((index: number) => {
          this.currentTab = index;
        })
        .layoutWeight(1)
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F0F2F5')

      if (this.showScheduleEditor) {
        ScheduleEditDialog({
          show: $showScheduleEditor,
          data: $editData,
          showDelete: this.editData.index >= 0,
          onSave: () => { this.saveSchedule(); },
          onDelete: () => { this.deleteSchedule(); },
          onCancel: () => { this.showScheduleEditor = false; }
        })
      }
    }
    .width('100%')
    .height('100%')
  }

  @Builder
  TabBarBuilder(title: string, index: number) {
    Column() {
      Text(title)
        .fontSize(14)
        .fontWeight(this.currentTab === index ? FontWeight.Bold : FontWeight.Normal)
        .fontColor(this.currentTab === index ? '#1E88E5' : '#9E9E9E')
    }
    .width('100%')
    .height(48)
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
  }

  saveSchedule(): void {
    let maxIdx = -1;
    for (let i = 0; i < this.timeSchedules.length; i++) {
      if (this.timeSchedules[i].index > maxIdx) {
        maxIdx = this.timeSchedules[i].index;
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
    this.udpClient.sendSchedule(schedule);

    const newList: ScheduleItem[] = [];
    for (let i = 0; i < this.timeSchedules.length; i++) {
      newList.push(this.timeSchedules[i]);
    }
    if (idx < newList.length) {
      newList[idx] = schedule;
    } else {
      newList.push(schedule);
    }
    this.timeSchedules = newList;
    this.showScheduleEditor = false;
  }

  deleteSchedule(): void {
    if (this.editData.index < 0) {
      this.showScheduleEditor = false;
      return;
    }
    const idx = this.editData.index;
    this.udpClient.sendScheduleDelete(idx);

    const newList: ScheduleItem[] = [];
    for (let i = 0; i < this.timeSchedules.length; i++) {
      if (this.timeSchedules[i].index !== idx) {
        newList.push(this.timeSchedules[i]);
      }
    }
    this.timeSchedules = newList;
    this.showScheduleEditor = false;
  }

  async connectDevice() {
    if (this.deviceIP === '') {
      this.timerMessage = '请输入IP地址';
      return;
    }

    const success = await this.udpClient.connect(this.deviceIP, (data: string) => {
      this.handleMessage(data);
    });

    if (success) {
      this.isConnected = true;
      this.statusText = `已连接 - ${this.deviceIP}`;
      await this.udpClient.sendStatusRequest();

      const now = new Date();
      await this.udpClient.sendTimeSync(now.getHours(), now.getMinutes());
    } else {
      this.isConnected = false;
      this.statusText = '连接失败';
    }
  }

  async disconnectDevice() {
    await this.udpClient.disconnect();
    this.isConnected = false;
    this.statusText = '未连接';
    this.lightOn = false;
    this.lightBrightness = 0;
    this.uptime = 0;
    this.locked = true;
    this.pir = false;
    this.pirEvent = '';
    this.manual = false;
    this.doorOpen = false;
    this.temperature = 0;
    this.humidity = 0;
    this.acOn = false;
    this.acMode = 'cool';
    this.acTargetTemp = 26;
    this.acBrand = 'midea';
    this.scene = '';
    this.autoUnlock = false;
    this.deviceTime = '';
    this.scheduleCount = 0;
    this.timeSchedules = [];
  }

  handleMessage(data: string) {
    try {
      const json = JSON.parse(data) as DeviceStatus;

      if (json.status !== undefined) {
        this.lightOn = json.status === 'on';
      }
      if (json.brightness !== undefined) {
        this.lightBrightness = json.brightness;
      }
      if (json.uptime !== undefined) {
        this.uptime = json.uptime as number;
      }
      if (json.locked !== undefined) {
        this.locked = json.locked as boolean;
      }
      if (json.pir !== undefined) {
        this.pir = json.pir as boolean;
      }
      if (json.manual !== undefined) {
        this.manual = json.manual as boolean;
      }
      if (json.door_open !== undefined) {
        this.doorOpen = json.door_open as boolean;
      }
      if (json.temperature !== undefined) {
        this.temperature = json.temperature as number;
      }
      if (json.humidity !== undefined) {
        this.humidity = json.humidity as number;
      }
      if (json.ac_on !== undefined) {
        this.acOn = json.ac_on as boolean;
      }
      if (json.ac_mode !== undefined) {
        this.acMode = json.ac_mode as string;
      }
      if (json.ac_target_temp !== undefined) {
        this.acTargetTemp = json.ac_target_temp as number;
      }
      if (json.ac_brand !== undefined) {
        this.acBrand = json.ac_brand as string;
      }
      if (json.scene !== undefined) {
        this.scene = json.scene as string;
      }
      if (json.auto_unlock !== undefined) {
        this.autoUnlock = json.auto_unlock as boolean;
      }
      if (json.time !== undefined) {
        this.deviceTime = json.time as string;
      }
      if (json.schedule_count !== undefined) {
        this.scheduleCount = json.schedule_count as number;
      }
    } catch (err) {
    }

    try {
      const evtJson = JSON.parse(data) as Record<string, string>;
      if (evtJson['event'] === 'door_open') {
        this.doorOpen = true;
      } else if (evtJson['event'] === 'pir_detected') {
        this.pirEvent = 'detected';
        this.pir = true;
      } else if (evtJson['event'] === 'pir_left') {
        this.pirEvent = 'left';
        this.pir = false;
      }
    } catch (_) {
    }
  }

  setTimer() {
    const delayStr = this.timerDelay;
    if (delayStr === '') {
      this.timerMessage = '请输入延迟秒数';
      return;
    }

    const delaySeconds = parseInt(delayStr);
    if (isNaN(delaySeconds) || delaySeconds <= 0) {
      this.timerMessage = '请输入有效的秒数';
      return;
    }

    if (!this.isConnected) {
      this.timerMessage = '请先连接设备';
      return;
    }

    const action = this.timerActions[this.timerActionIndex] === '开灯' ? 'on' : 'off';
    const actionText = action === 'on' ? '开灯' : '关灯';
    this.timerMessage = `定时已设置: ${delaySeconds}秒后${actionText}`;

    setTimeout(() => {
      if (this.isConnected) {
        this.udpClient.send(action === 'on' ? '{"action":"on"}' : '{"action":"off"}');
      }
      this.timerMessage = '';
    }, delaySeconds * 1000);
  }

  private timerActions: string[] = ['开灯', '关灯'];
}
