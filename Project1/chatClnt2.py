import asyncio
import websockets
import tkinter as tk
import aiohttp
from aiohttp import web


class ChatClient(tk.Tk):
    def __init__(self, loop):
        super().__init__()
        self.title("Client")
        self.loop = loop
        self.websocket = None

        self.chat_box = tk.Text(self, state='disabled')
        self.chat_box.pack()

        self.msg_entry = tk.Entry(self)
        self.msg_entry.bind("<Return>", self.on_entry_return)
        self.msg_entry.pack()

        self.protocol("WM_DELETE_WINDOW", self.on_closing)

    async def connect(self):
        self.websocket = await websockets.connect('ws://localhost:8765')

    async def receive_message(self):
        while True:
            msg = await self.websocket.recv()
            self.chat_box.configure(state='normal')
            self.chat_box.insert('end', f"Other: {msg}\n")
            self.chat_box.configure(state='disabled')
            print(f"Other : {msg}")  # 받은 내용을 출력

    def on_entry_return(self, event):
        msg = self.msg_entry.get()
        self.msg_entry.delete(0, 'end')
        self.loop.create_task(self.websocket.send(msg))
        self.chat_box.configure(state='normal')
        self.chat_box.insert('end', f"Me: {msg}\n")
        self.chat_box.configure(state='disabled')
        print(f"Me : {msg}")  # 보낸 내용을 출력

    def on_closing(self):
        self.loop.create_task(self.websocket.close())
        self.destroy()


if __name__ == "__main__":
    loop = asyncio.get_event_loop()
    chat_client = ChatClient(loop)

    loop.run_until_complete(chat_client.connect())
    loop.create_task(chat_client.receive_message())

    def run_tk(root, interval=0.05):  # 50 ms
        def update():
            root.update()
            loop.call_later(interval, update)
        loop.call_soon(update)
        loop.run_forever()

    run_tk(chat_client)
