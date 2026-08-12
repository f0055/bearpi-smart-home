import socket from '@ohos.net.socket';
import hilog from '@ohos.hilog';

const DOMAIN = 0x0000;
const TAG = 'UdpClient';
const DEVICE_PORT = 8888;

export interface DeviceStatus {
  status: string;
  brightness: number;
  uptime: number;
  locked: boolean;
  manual: boolean;
  pir: boolean;
  door_open: boolean;
  auto_unlock: boolean;
  temperature: number;
  humidity: number;
  ac_on: boolean;
  ac_mode: string;
  ac_target_temp: number;
  ac_brand: string;
  broadcast: boolean;
  scene: string;
  midea_found: boolean;
  midea_ip: string;
  time: string;
  schedule_count: number;
}

export interface ScheduleItem {
  index: number;
  enabled: boolean;
  start_hour: number;
  start_minute: number;
  end_hour: number;
  end_minute: number;
}

@Observed
export class ScheduleEditData {
  index: number = -1;
  startHour: number = 8;
  startMinute: number = 0;
  endHour: number = 9;
  endMinute: number = 0;
  enabled: boolean = true;

  loadFrom(item: ScheduleItem): void {
    this.index = item.index;
    this.startHour = item.start_hour;
    this.startMinute = item.start_minute;
    this.endHour = item.end_hour;
    this.endMinute = item.end_minute;
    this.enabled = item.enabled;
  }

  reset(): void {
    this.index = -1;
    this.startHour = 8;
    this.startMinute = 0;
    this.endHour = 9;
    this.endMinute = 0;
    this.enabled = false;
  }
}

export class UdpClient {
  private udpSocket: socket.UDPSocket | null = null;
  private remoteIP: string = '';
  private messageCallback: ((data: string) => void) | null = null;
  private statusTimer: number = -1;

  async connect(ip: string, callback: (data: string) => void): Promise<boolean> {
    try {
      this.remoteIP = ip;
      this.messageCallback = callback;

      this.udpSocket = socket.constructUDPSocketInstance();

      const bindAddr: socket.NetAddress = { address: '0.0.0.0', port: 0, family: 1 };
      await this.udpSocket.bind(bindAddr);

      const self = this;
      const handler = (value: socket.SocketMessageInfo): void => {
        const buffer = value.message;
        const bytes = new Uint8Array(buffer);
        let text = '';
        for (let i = 0; i < bytes.length; i++) {
          text += String.fromCharCode(bytes[i]);
        }
        hilog.info(DOMAIN, TAG, 'Received: %{public}s', text);
        if (self.messageCallback) {
          self.messageCallback(text);
        }
      };
      this.udpSocket.on('message', handler);

      this.startStatusPolling();
      return true;
    } catch (err) {
      hilog.error(DOMAIN, TAG, 'Connect failed: %{public}s', JSON.stringify(err));
      return false;
    }
  }

  async send(message: string): Promise<boolean> {
    try {
      if (!this.udpSocket || !this.remoteIP) {
        return false;
      }

      const sendAddr: socket.NetAddress = { address: this.remoteIP, family: 1, port: DEVICE_PORT };
      const sendOpts: socket.UDPSendOptions = { address: sendAddr, data: message };
      await this.udpSocket.send(sendOpts);

      hilog.info(DOMAIN, TAG, 'Sent: %{public}s', message);
      return true;
    } catch (err) {
      hilog.error(DOMAIN, TAG, 'Send failed: %{public}s', JSON.stringify(err));
      return false;
    }
  }

  async sendStatusRequest(): Promise<boolean> {
    return this.send('{"action":"status"}');
  }

  async sendLightOn(): Promise<boolean> {
    return this.send('{"action":"on"}');
  }

  async sendLightOff(): Promise<boolean> {
    return this.send('{"action":"off"}');
  }

  async sendBrightness(value: number): Promise<boolean> {
    return this.send(`{"action":"brightness","value":${value}}`);
  }

  async sendLock(): Promise<boolean> {
    return this.send('{"action":"lock"}');
  }

  async sendUnlock(): Promise<boolean> {
    return this.send('{"action":"unlock"}');
  }

  async sendAutoUnlock(enable: boolean): Promise<boolean> {
    return this.send(`{"auto_unlock":${enable}}`);
  }

  async sendAcOn(): Promise<boolean> {
    return this.send('{"action":"ac_on"}');
  }

  async sendAcOff(): Promise<boolean> {
    return this.send('{"action":"ac_off"}');
  }

  async sendAcMode(mode: string): Promise<boolean> {
    return this.send(`{"ac_mode":"${mode}"}`);
  }

  async sendAcTargetTemp(temp: number): Promise<boolean> {
    return this.send(`{"ac_target_temp":${temp}}`);
  }

  async sendAcBrand(brand: string): Promise<boolean> {
    return this.send(`{"ac_brand":"${brand}"}`);
  }

  async sendBroadcast(enable: boolean): Promise<boolean> {
    return this.send(`{"broadcast":${enable}}`);
  }

  async sendScene(scene: string): Promise<boolean> {
    return this.send(`{"scene":"${scene}"}`);
  }

  async sendTimeSync(hour: number, minute: number): Promise<boolean> {
    return this.send(`{"time_sync":{"hour":${hour},"minute":${minute}}}`);
  }

  async sendSchedule(schedule: ScheduleItem): Promise<boolean> {
    return this.send(`{"set_schedule":{"index":${schedule.index},"enabled":${schedule.enabled},"start_hour":${schedule.start_hour},"start_minute":${schedule.start_minute},"end_hour":${schedule.end_hour},"end_minute":${schedule.end_minute}}}`);
  }

  async sendScheduleDelete(index: number): Promise<boolean> {
    return this.send(`{"set_schedule":{"index":${index},"enabled":false,"start_hour":0,"start_minute":0,"end_hour":0,"end_minute":0}}`);
  }

  async sendMideaDiscover(): Promise<boolean> {
    return this.send('{"midea_discover":true}');
  }

  async sendMideaControl(on: boolean, temp: number, mode: string): Promise<boolean> {
    return this.send(`{"midea_control":{"on":${on},"temp":${temp},"mode":"${mode}"}}`);
  }

  private startStatusPolling(): void {
    this.stopStatusPolling();
    this.statusTimer = setInterval(() => {
      this.sendStatusRequest();
    }, 3000);
  }

  private stopStatusPolling(): void {
    if (this.statusTimer >= 0) {
      clearInterval(this.statusTimer);
      this.statusTimer = -1;
    }
  }

  async disconnect(): Promise<void> {
    try {
      this.stopStatusPolling();

      if (this.udpSocket) {
        this.udpSocket.off('message');
        await this.udpSocket.close();
        this.udpSocket = null;
        this.remoteIP = '';
        this.messageCallback = null;
      }
    } catch (err) {
      hilog.error(DOMAIN, TAG, 'Disconnect failed: %{public}s', JSON.stringify(err));
    }
  }

  getRemoteIP(): string {
    return this.remoteIP;
  }
}

export const sharedUdpClient: UdpClient = new UdpClient();
