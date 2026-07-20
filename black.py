import asyncio, websockets

async def go():
    async with websockets.connect('ws://127.0.0.1:9002') as ws:
        print('--- welcome (should say color B) ---')
        print(await ws.recv())
        print('--- initial state ---')
        print(await ws.recv())
        input(">>> Press Enter to request STATE...")
        await ws.send('STATE')
        print('--- state (after white moved) ---')
        print(await ws.recv())

asyncio.run(go())