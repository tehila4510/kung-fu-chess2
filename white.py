import asyncio, websockets

async def go():
    async with websockets.connect('ws://127.0.0.1:9002') as ws:
        print('--- welcome ---')
        print(await ws.recv())
        print('--- initial state ---')
        print(await ws.recv())
        input(">>> Press Enter to send WMe2e4...")
        await ws.send('WMe2e4')
        print('--- after move ---')
        print(await ws.recv())
        input(">>> Press Enter to close White...")

asyncio.run(go())