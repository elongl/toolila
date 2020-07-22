from pathlib import Path
import socket

from pwn import p64


class CommandTypes:
    SHELL = b'\x00'.ljust(4, b'\x00')
    UPLOAD_FILE = b'\x01'.ljust(4, b'\x00')


class Controller:
    DEFAULT_AGENT_PORT = 5000

    def __init__(self, agent_ip, agent_port=DEFAULT_AGENT_PORT):
        self.sock = socket.socket()
        self.agent_ip = agent_ip
        self.agent_port = agent_port

    def connect(self):
        print('[*] Connecting to Toolila.')
        self.sock.connect((self.agent_ip, self.agent_port))

    def shell(self, cmd):
        print(f'[*] Running \'{cmd}\'')
        self.sock.send(CommandTypes.SHELL)
        self.sock.send(cmd.encode().ljust(1024, b'\0'))
        return self.sock.recv(4096).replace(b'\0', b'').decode().strip()

    def upload(self, local_path, remote_path):
        self.sock.send(CommandTypes.UPLOAD_FILE)
        self.sock.send(remote_path.encode().ljust(255, b'\0'))
        local_file_size = Path(local_path).stat().st_size
        self.sock.send(p64(local_file_size))
        with open(local_path, 'rb') as local_file:
            self.sock.send(local_file.read())
