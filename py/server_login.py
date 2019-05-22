from ctypes import *
import binascii

class ServerLoginLibrary:
    def __init__(self):
        self.server_lib = CDLL("../cpp/server_lib.so")
        self.byte_size = 0
        self.hash_size = 0
        self.key_size = 0
        self.initialize()

    def get_byte_size(self):
        return self.byte_size

    def get_hash_size(self):
        return self.hash_size

    def get_key_size(self):
        return self.key_size
    
    def initialize(self):
        self.server_lib.init_server_lib()
        self.byte_size = self.server_lib.lib_bytes_size()
        self.hash_size = self.server_lib.lib_hash_size()
        self.key_size = self.server_lib.lib_key_size()

    def generate_b(self, v_bytes):
        v = (c_ubyte * self.byte_size).from_buffer_copy(v_bytes)
        b = (c_ubyte * self.byte_size)()
        B = (c_ubyte * self.byte_size)()
        self.server_lib.generate_b(v, b, B)
        b_bytes = bytearray(b)
        B_bytes = bytearray(B)
        return (b_bytes, B_bytes)

    def generate_sk(self, un, A_bytes, b_bytes, B_bytes, s_bytes, v_bytes):
        username = c_char_p(un)
        A = (c_ubyte * self.byte_size).from_buffer_copy(A_bytes)
        b = (c_ubyte * self.byte_size).from_buffer_copy(b_bytes)
        B = (c_ubyte * self.byte_size).from_buffer_copy(B_bytes)
        s = (c_ubyte * self.byte_size).from_buffer_copy(s_bytes)
        v = (c_ubyte * self.byte_size).from_buffer_copy(v_bytes)
        sk = (c_ubyte * self.key_size)()
        m1 = (c_ubyte * self.hash_size)()
        m2 = (c_ubyte * self.hash_size)()
        self.server_lib.generate_sk(username, A, b, B, s, v, sk, m1, m2)
        sk_bytes = bytearray(sk)
        m1_bytes = bytearray(m1)
        m2_bytes = bytearray(m2)
        return (sk_bytes, m1_bytes, m2_bytes)
