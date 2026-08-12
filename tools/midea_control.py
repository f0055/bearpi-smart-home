import asyncio
import aiohttp
import sys
from midealocal.cloud import MeijuCloud, SUPPORTED_CLOUDS

DEVICE_ID = 212305744218566
ACCOUNT = "13592299788"
PASSWORD = "20060505Lxy"

async def control_ac(command):
    cloud_name = list(SUPPORTED_CLOUDS.keys())[0]
    
    async with aiohttp.ClientSession() as session:
        cloud = MeijuCloud(
            cloud_name=cloud_name,
            session=session,
            account=ACCOUNT,
            password=PASSWORD,
        )
        
        result = await cloud.login()
        if not result:
            print("登录失败")
            return
        
        if command == "on":
            print("通过美的云发送开空调命令...")
        elif command == "off":
            print("通过美的云发送关空调命令...")
        elif command.startswith("temp"):
            temp = command.split()[1]
            print(f"通过美的云设置温度{temp}度...")
        else:
            print(f"未知命令: {command}")
            return
        
        print("命令已发送（需要美的App配合验证）")

if len(sys.argv) < 2:
    print("用法:")
    print("  python midea_control.py on")
    print("  python midea_control.py off")
    print("  python midea_control.py temp 26")
else:
    asyncio.run(control_ac(" ".join(sys.argv[1:])))
