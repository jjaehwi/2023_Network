import asyncio
import websockets
import tkinter as tk


class ChatServer(tk.Tk):
    def __init__(self, loop):
        super().__init__()
        self.title("Server")
        self.loop = loop
        self.websocket = None

        self.chat_box = tk.Text(self, state='disabled')
        self.chat_box.pack()

        self.msg_entry = tk.Entry(self)
        self.msg_entry.bind("<Return>", self.on_entry_return)
        self.msg_entry.pack()

        self.protocol("WM_DELETE_WINDOW", self.on_closing)

    async def start_server(self):
        self.websocket = await websockets.serve(self.server, 'localhost', 8765)

    async def server(self, websocket, path):
        connected.add(websocket)
        try:
            while True:
                message = await websocket.recv()
                print(f"Other: {message}")  # 받은 내용을 출력
                self.chat_box.configure(state='normal')
                self.chat_box.insert('end', f"Other: {message}\n")
                self.chat_box.configure(state='disabled')

                # 에코 서버 예제
                # for conn in connected:
                #     await conn.send(message)  # 받은 내용을 연결된 모든 클라이언트로 전송
                #     print(f"보낸 내용: {message}")  # 보낸 내용을 출력
                #     self.chat_box.configure(state='normal')
                #     self.chat_box.insert('end', f"보낸 내용: {message}\n")
                #     self.chat_box.configure(state='disabled')

        except websockets.exceptions.ConnectionClosed:
            pass
        finally:
            connected.remove(websocket)

    def on_entry_return(self, event):
        msg = self.msg_entry.get()
        self.msg_entry.delete(0, 'end')

        for conn in connected:
            self.loop.create_task(conn.send(msg))
            print(f"Me: {msg}")  # 서버에서 클라이언트로 보낸 메시지를 출력
            self.chat_box.configure(state='normal')
            self.chat_box.insert('end', f"Me: {msg}\n")
            self.chat_box.configure(state='disabled')

    def on_closing(self):
        for conn in connected:
            self.loop.create_task(conn.close())

        self.destroy()


if __name__ == "__main__":
    connected = set()

    loop = asyncio.get_event_loop()
    chat_server = ChatServer(loop)

    loop.run_until_complete(chat_server.start_server())

    def run_tk():
        chat_server.update()
        loop.call_later(0.05, run_tk)

    run_tk()
    loop.run_forever()
