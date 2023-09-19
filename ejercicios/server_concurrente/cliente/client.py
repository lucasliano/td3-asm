import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 5050  # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    # s.sendall(b"Hello, world")

    for _ in range (4):
        data = s.recv(100)
        print(f"Received {str(data)}")

    s.close()
