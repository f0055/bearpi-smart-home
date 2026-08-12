import asyncio
import aiohttp
import logging
logging.basicConfig(level=logging.DEBUG)

from midealocal.cloud import MeijuCloud, SUPPORTED_CLOUDS

async def main():
    cloud_name = list(SUPPORTED_CLOUDS.keys())[0]
    
    async with aiohttp.ClientSession() as session:
        cloud = MeijuCloud(
            cloud_name=cloud_name,
            session=session,
            account="13592299788",
            password="20060505Lxy",
        )
        
        result = await cloud.login()
        print(f"登录: {result}")
        
        if result:
            device_id = 212305744218566
            print(f"\n尝试获取设备 {device_id} 的token...")
            token_key = await cloud.get_cloud_keys(device_id)
            print(f"结果: {token_key}")
            
            if not token_key:
                print("\n尝试方法2：直接调用API...")
                # 手动调用getToken API
                for method in [1, 2]:
                    udp_id = cloud._security.get_udp_id(device_id, method)
                    print(f"\n方法{method}, UDP ID: {udp_id}")
                    
                    data = {
                        "udpid": udp_id,
                        "applianceCodes": str(device_id),
                    }
                    
                    # 获取access token后再发请求
                    response = await cloud._api_request(
                        endpoint="/v1/iot/secure/getToken",
                        data=data,
                    )
                    print(f"API响应: {response}")

asyncio.run(main())
