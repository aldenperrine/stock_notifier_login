import requests
from ctypes import *
import sys
import getpass

class client_login:
     __hexArray = "0123456789ABCDEF"

     def __init__(self):
          self.client_lib = CDLL("../cpp/client_lib.so")
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
          self.client_lib.init_client_lib()
          self.byte_size = self.client_lib.lib_bytes_size()
          self.hash_size = self.client_lib.lib_hash_size()
          self.key_size = self.client_lib.lib_key_size()

     def generate_registration(self, user_pass):
          up_chars = (c_char * len(user_pass)).from_buffer(bytearray(user_pass, 'ascii'))
          up_pointer = c_char_p(addressof(up_chars))
          s = (c_ubyte * self.byte_size)()
          v = (c_ubyte * self.byte_size)()
          self.client_lib.generate_registration(up_pointer, s, v)
          s_bytes = bytearray(s)
          v_bytes = bytearray(v)
          return (s_bytes, v_bytes)

     def generate_a(self):
          a = (c_ubyte * self.byte_size)()
          A = (c_ubyte * self.byte_size)()
          self.client_lib.generate_a(a, A)
          a_bytes = bytearray(a)
          A_bytes = bytearray(A)
          return (a_bytes, A_bytes)
    
     def generate_ck(self, username, user_pass, a_bytes, A_bytes, B_bytes, s_bytes, n_bytes):
          un = (c_char * len(username)).from_buffer(bytearray(username, 'ascii'))
          un_pointer = c_char_p(addressof(un))
          up = (c_char * len(user_pass)).from_buffer(bytearray(user_pass, 'ascii'))
          up_pointer = c_char_p(addressof(up))
          a = (c_ubyte * self.byte_size).from_buffer_copy(a_bytes)
          A = (c_ubyte * self.byte_size).from_buffer_copy(A_bytes)
          B = (c_ubyte * self.byte_size).from_buffer_copy(B_bytes)
          s = (c_ubyte * self.byte_size).from_buffer_copy(s_bytes)
          n = (c_ubyte * self.key_size).from_buffer_copy(n_bytes)
          ck = (c_ubyte * self.key_size)()
          m1 = (c_ubyte * self.hash_size)()
          m2 = (c_ubyte * self.hash_size)()
          hn = (c_ubyte * self.key_size)()
          self.client_lib.generate_ck(un_pointer, up_pointer, a, A, B, s, n, ck, m1, m2, hn)
          ck_bytes = bytearray(ck)
          m1_bytes = bytearray(m1)
          m2_bytes = bytearray(m2)
          hn_bytes = bytearray(hn)
          return (ck_bytes, m1_bytes, m2_bytes, hn_bytes)

     def bytesToHex(self, data):
          return ''.join(format(d, '02x') for d in data)

     def hexToBytes(self, string):
          return bytearray.fromhex(string)

   
def run_client(address):
     try:
          client_lib = client_login()
     except Exception:
          print("Failed to get client lib")
          return
     
     username = input("Username: ")
     password = getpass.getpass()
     user_pass = username+":"+password

     salt_request = requests.post(address+"/login/get_salt", json={'username': username})
     s_bytes = bytearray()
     if (salt_request.status_code != 200 and salt_request.json()['error'] == 0):
          print("Make registration")
          s_bytes, v_bytes = client_lib.generate_registration(user_pass)
          register = requests.post(address+"/register", json={'username': username,
                                                              'user_salt': client_lib.bytesToHex(s_bytes),
                                                              'user_verifier': client_lib.bytesToHex(v_bytes)})
          if(register.status_code != 200):
               print(register.text)
     else:
          print("User exists")
          print(salt_request.text)
          s_bytes = client_lib.hexToBytes(salt_request.json()['user_salt'])

     print("Logining in")
     a_bytes, A_bytes = client_lib.generate_a()
     b_request = requests.post(address+"/login/get_b", json={'username': username,
                                                             'big_a':client_lib.bytesToHex(A_bytes)})
     b_contents = b_request.json()
     B_bytes = client_lib.hexToBytes(b_contents['big_b'])
     n_bytes = client_lib.hexToBytes(b_contents['nonce'])
     ck_bytes, m1_bytes, m2_bytes, h_bytes = client_lib.generate_ck(username, user_pass, a_bytes, A_bytes, B_bytes, s_bytes, n_bytes)
     session_id = client_lib.bytesToHex(h_bytes)
     m_request = requests.post(address+"/login/get_m2", json={'username': username,
                                                              'session_id': session_id,
                                                              'm1': client_lib.bytesToHex(m1_bytes),
                                                              'device_id': 'literally_desktop'})
     if (client_lib.hexToBytes(m_request.json()['m2']) == m2_bytes):
          print("Validation succeeded")
     else:
          print("Validation issues")


     all_request = requests.get(address+"/all_tickers")
     print(all_request.text)

     add_request = requests.get(address+"/add_ticker", json={'username': username, 'session_id': session_id, 'tickers': ['AMD']})
     print(add_request.text)
     
     ut_request = requests.get(address+"/user_tickers", json={'username': username, 'session_id': session_id})
     print(ut_request.text)

     rem_request = requests.get(address+"/delete_ticker", json={'username': username, 'session_id': session_id, 'tickers': ['AMD']})
     print(rem_request.text)

     ut_request = requests.get(address+"/user_tickers", json={'username': username, 'session_id': session_id})
     print(ut_request.text)
     
     d_request = requests.post(address+"/login/terminate", json={'username': username, 'session_id': session_id})
     print(d_request.text)
     
if __name__ == "__main__":
     if (len(sys.argv) < 2):
          print("No address given")
     else:
          run_client(sys.argv[1])
          
