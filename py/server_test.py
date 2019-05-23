import socket
import sys
import server_login

def run_server():
    try:
        server_lib = server_login.ServerLoginLibrary()
    except Exception:
        print("Failed to get server lib")
        return

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = ('127.0.0.1', 8000)
    sock.bind(server_address)
    sock.listen(1)
    connection, client_address = sock.accept()
    s_bytes = connection.recv(128)
    v_bytes = connection.recv(128)
    username_bytes = bytearray(80)
    last_byte = bytes({10})
    loc = 0
    while last_byte[0] is not 0 and loc < 80:
        last_byte = connection.recv(1)
        username_bytes[loc] = last_byte[0] 
        loc = loc + 1
    A_bytes = connection.recv(128)
    
    b_bytes, B_bytes, n_bytes, h_bytes = server_lib.generate_b(v_bytes)
    connection.send(B_bytes)
    connection.send(n_bytes)
    sk_bytes, m1_bytes, m2_bytes = server_lib.generate_sk(username_bytes, A_bytes, b_bytes, B_bytes, s_bytes, v_bytes)
    mv = connection.recv(64)
    hv = connection.recv(32)
    connection.send(m2_bytes)
    if mv == m1_bytes:
        print("Login success :)")
    else:
        print("Login failure :(")

if __name__ == "__main__":
    run_server()
