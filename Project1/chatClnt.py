import socket
import threading
import tkinter as tk

HOST = 'localhost'
PORT = 9020


class ChatWindow:
    def __init__(self, master):
        self.master = master
        master.title("Chat Client")

        self.chat_log = tk.Text(master)
        self.chat_log.pack()

        self.input_label = tk.Label(master, text="Enter your message:")
        self.input_label.pack()

        self.input_field = tk.Entry(master, font=("Helvetica", 16))
        self.input_field.pack()
        self.input_field.bind("<Return>\n", self.send_msg)

        self.send_button = tk.Button(
            master, text="Send", command=self.send_msg)
        self.send_button.pack()

        self.quit_button = tk.Button(
            master, text="Quit", command=self.quit_chat)
        self.quit_button.pack()

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((HOST, PORT))

        self.receive_thread = threading.Thread(target=self.receive_msgs)
        self.receive_thread.daemon = True
        self.receive_thread.start()

    def send_msg(self, event=None):
        msg = self.input_field.get()
        self.input_field.delete(0, tk.END)
        self.sock.send(msg.encode())

    def receive_msgs(self):
        while True:
            data = self.sock.recv(1024)
            if not data:
                break
            # append newline character to message before inserting into chat log
            self.chat_log.insert(tk.END, data.decode() + "\n")

    def quit_chat(self):
        self.sock.send('/quit'.encode())
        self.sock.close()
        self.master.destroy()


root = tk.Tk()
chat_window = ChatWindow(root)
root.mainloop()
