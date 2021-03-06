#include "client_login.h"
#include "server_login.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#include <string.h>
#include <gmp.h>
#include <sodium.h>

std::string binary_conv(unsigned char* binary, size_t size) {
  std::stringstream ss;
  for (int i = 0; i < size; ++i) {
    ss << std::hex << std::setfill('0') << std::setw(2) << (int) binary[i];
  }
  return ss.str();
}

int compare_hex(unsigned char str1[], unsigned char str2[], size_t size) {
  for (int i = 0; i < size; ++i) {
    if (str1[i] != str2[i]) {
      return i+1;
    } 
  }
  return 0;
}

int test_login() {
  int size = lib_bytes_size();

  char username[] = "username";
  char password[] = "password";
  char userpass[80];
  strcpy(userpass, username);
  strcat(userpass, ":");
  strcat(userpass, password);

  unsigned char s[size];
  unsigned char v[size];
  memset(s, 0, sizeof(s));
  memset(v, 0, sizeof(v));
  if (generate_registration(userpass, s, v)) {
    return 1;
  }

  unsigned char a[size];
  unsigned char A[size];
  memset(a, 0, sizeof(a));
  memset(A, 0, sizeof(A));
  if (generate_a(a, A)) {
    return 1;
  }

  unsigned char b[size];
  unsigned char B[size];
  unsigned char n[lib_key_size()];
  unsigned char h[lib_key_size()];
  memset(b, 0, sizeof(b));
  memset(B, 0, sizeof(B));
  memset(n, 0, sizeof(n));
  memset(h, 0, sizeof(h));
  if (generate_b(v, b, B, n, h)) {
    return 1;
  }

  unsigned char vm[size];
  unsigned char cv[size];
  unsigned char ks[lib_key_size()];
  unsigned char kc[lib_key_size()];
  unsigned char ch[lib_key_size()];
  unsigned char cm1[lib_hash_size()];
  unsigned char cm2[lib_hash_size()];
  unsigned char sm1[lib_hash_size()];
  unsigned char sm2[lib_hash_size()];
  memset(ks, 0, sizeof(ks));
  memset(kc, 0, sizeof(kc));
  memset(ch, 0, sizeof(ch));
  if (generate_ck(username, userpass, a, A, B, s, n, kc, cm1, cm2, ch)) {
    return 1;
  }

  if (generate_sk(username, A, b, B, s, v, ks, sm1, sm2)) {
    return 1;
  }

  if (compare_hex(sm1, cm1, lib_hash_size()) != 0) {
    return 1;
  }

  if (compare_hex(sm2, cm2, lib_hash_size()) != 0) {
    return 1;
  }

  if (compare_hex(h, ch, lib_key_size()) != 0) {
    std::cout << "Hash issue" << std::endl;
    std::cout << binary_conv(h, 32) << std::endl;
    std::cout << binary_conv(ch, 32) << std::endl;

    unsigned char hr[32];
    crypto_generichash_state h_state;
    crypto_generichash_init(&h_state, NULL, 0, lib_key_size());
    crypto_generichash_update(&h_state, v, size);
    crypto_generichash_update(&h_state, n, 32);
    crypto_generichash_final(&h_state, hr, lib_key_size());
    std::cout << binary_conv(hr, 32) << std::endl;

    std::cout << binary_conv(v, 128) << std::endl;
    std::cout << binary_conv(vm, 128) << std::endl;
    
    return 1;
  }

  return compare_hex(ks, kc, lib_key_size());
}

int main(int argc, char** argv) {
  if (init_server_lib()) {
    return 1;
  }

  if (init_client_lib()) {
    return 1;
  }

  for (int i = 0 ; i < 1000; ++i) {
    if (test_login()) {
      std::cout << "ERROR " << i << std::endl;
    }
  }
    
  return 0;
}
