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
    s_bytes = server_lib.hexToBytes(connection.recv(256).decode('ascii'))
    v_bytes = server_lib.hexToBytes(connection.recv(256).decode('ascii'))
    username_bytes = bytearray(80)
    last_byte = bytes({10})
    loc = 0
    while last_byte[0] is not 0 and loc < 80:
        last_byte = connection.recv(1)
        username_bytes[loc] = last_byte[0] 
        loc = loc + 1
    A_bytes = server_lib.hexToBytes(connection.recv(256).decode('ascii'))
    
    b_bytes, B_bytes, n_bytes, h_bytes = server_lib.generate_b(v_bytes)
    connection.send(server_lib.bytesToHex(B_bytes).encode('ascii'))
    connection.send(server_lib.bytesToHex(n_bytes).encode('ascii'))
    sk_bytes, m1_bytes, m2_bytes = server_lib.generate_sk(username_bytes, A_bytes, b_bytes, B_bytes, s_bytes, v_bytes)
    mv = server_lib.hexToBytes(connection.recv(128).decode('ascii'))
    hv = server_lib.hexToBytes(connection.recv(64).decode('ascii'))
    connection.send(server_lib.bytesToHex(m2_bytes).encode('ascii'))
    if mv == m1_bytes:
        print("Login success :)")
    else:
        print("Login failure :(")

if __name__ == "__main__":
    run_server()
