import asyncio, websockets

SERVER = 'ws://127.0.0.1:9002'

async def test_new_user():
    print("=== 1. משתמש חדש: AUTH tehila 1234 ===")
    async with websockets.connect(SERVER) as ws:
        print("expect auth_required:", await ws.recv())
        await ws.send("AUTH tehila 1234")
        print("expect auth_ok, rating 1200:", await ws.recv())
        print("expect welcome:", await ws.recv())
        print("expect initial state:", await ws.recv())

async def test_wrong_password():
    print("\n=== 2. אותו משתמש, סיסמה שגויה ===")
    async with websockets.connect(SERVER) as ws:
        print("expect auth_required:", await ws.recv())
        await ws.send("AUTH tehila WRONG_PASSWORD")
        print("expect bad_password:", await ws.recv())

async def test_correct_login():
    print("\n=== 3. אותו משתמש, סיסמה נכונה (login חוזר) ===")
    async with websockets.connect(SERVER) as ws:
        print("expect auth_required:", await ws.recv())
        await ws.send("AUTH tehila 1234")
        print("expect auth_ok, same rating:", await ws.recv())

async def test_malformed():
    print("\n=== 4. פקודה שגויה (לפני AUTH) ===")
    async with websockets.connect(SERVER) as ws:
        print("expect auth_required:", await ws.recv())
        await ws.send("WMe2e4")  # שולחים move במקום AUTH
        print("expect invalid_auth / rejected:", await ws.recv())

async def main():
    await test_new_user()
    await test_wrong_password()
    await test_correct_login()
    await test_malformed()

asyncio.run(main())