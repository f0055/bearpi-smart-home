import socket from '@ohos.net.socket';
import hilog from '@ohos.hilog';

const DOMAIN = 0x0000;
const TAG = 'DiscoveryListener';

export interface DiscoveryInfo {
  device: string;
  ip: string;
  status: string;
  locked: boolean;
  temperature: number;
  humidity: number;
  lastSeen: number;
}

export class DiscoveryListener {
  private udpSocket: socket.UDPSocket | null = null;
  private messageCallback: ((data: string) => void) | null = null;

  async start(callback: (data: string) => void): Promise<boolean> {
    try {
      this.messageCallback = callback;
      this.udpSocket = socket.constructUDPSocketInstance();

      const bindAddr: socket.NetAddress = { address: '0.0.0.0', port: 9999, family: 1 };
      await this.udpSocket.bind(bindAddr);

      const self = this;
      const handler = (value: socket.SocketMessageInfo): void => {
        const buffer = value.message;
        const bytes = new Uint8Array(buffer);
        let text = '';
        for (let i = 0; i < bytes.length; i++) {
          text += String.fromCharCode(bytes[i]);
        }
        hilog.info(DOMAIN, TAG, 'Discovery: %{public}s', text);
        if (self.messageCallback) {
          self.messageCallback(text);
        }
      };
      this.udpSocket.on('message', handler);
      return true;
    } catch (err) {
      hilog.error(DOMAIN, TAG, 'Discovery start failed: %{public}s', JSON.stringify(err));
      return false;
    }
  }

  async stop(): Promise<void> {
    try {
      if (this.udpSocket) {
        this.udpSocket.off('message');
        await this.udpSocket.close();
        this.udpSocket = null;
        this.messageCallback = null;
      }
    } catch (err) {
      hilog.error(DOMAIN, TAG, 'Discovery stop failed: %{public}s', JSON.stringify(err));
    }
  }
}
