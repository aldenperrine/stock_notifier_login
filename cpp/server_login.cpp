#include "server_login.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include <mutex>
#include <stdint.h>
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


/* From RFC5114 */
const uint32_t prime[32] = {0xB10B8F96, 0xA080E01D, 0xDE92DE5E, 0xAE5D54EC,
			    0x52C99FBC, 0xFB06A3C6, 0x9A6A9DCA, 0x52D23B61,
			    0x6073E286, 0x75A23D18, 0x9838EF1E, 0x2EE652C0,
			    0x13ECB4AE, 0xA9061123, 0x24975C3C, 0xD49B83BF,
			    0xACCBDD7D, 0x90C4BD70, 0x98488E9C, 0x219A7372,
			    0x4EFFD6FA, 0xE5644738, 0xFAA31A4F, 0xF55BCCC0,
			    0xA151AF5F, 0x0DC8B4BD, 0x45BF37DF, 0x365C1A65,
			    0xE68CFDA7, 0x6D4DA708, 0xDF1FB2BC, 0x2E4A4371};
	
const uint32_t gener[32] = {0xA4D1CBD5, 0xC3FD3412, 0x6765A442, 0xEFB99905,
			    0xF8104DD2, 0x58AC507F, 0xD6406CFF, 0x14266D31, 
			    0x266FEA1E, 0x5C41564B, 0x777E690F, 0x5504F213,
			    0x160217B4, 0xB01B886A, 0x5E91547F, 0x9E2749F4,
			    0xD7FBD7D3, 0xB9A92EE1, 0x909D0D22, 0x63F80A76,
			    0xA6A24C08, 0x7A091F53, 0x1DBF0A01, 0x69B6A28A,
			    0xD662A4D1, 0x8E73AFA3, 0x2D779D59, 0x18D08BC8,
			    0x858F4DCE, 0xF97C2A24, 0x855E6EEB, 0x22B3B2E5};

std::mutex init_mutex;
bool is_initialized = false;
mpz_t N;
mpz_t g;
mpz_t k;
const int BYTES_SIZE = sizeof(prime);
const int HASH_SIZE = 
  crypto_generichash_BYTES_MAX < BYTES_SIZE ? 
  crypto_generichash_BYTES_MAX : BYTES_SIZE;
const int KEY_SIZE = 32;

int lib_bytes_size() {
  return BYTES_SIZE;
}

int lib_hash_size() {
  return HASH_SIZE;
}

int lib_key_size() {
  return KEY_SIZE;
}

int init_server_lib() {
  init_mutex.lock();
  if (is_initialized) {
    init_mutex.unlock();
    return 0;
  }
  if (sodium_init() < 0) {
    init_mutex.unlock();
    return 1;
  }
  
  /* Initialize N, a large prime which is the base for the ring of integers */
  
  mpz_init(N);
  mpz_import(N, 32, 1, 4, 0, 0, prime);

  unsigned char nb[BYTES_SIZE];
  mpz_export(nb, NULL, -1, 1, 0, 0, N);
  
  /* Initialize g, the generator for the group */
  mpz_init(g);
  mpz_import(g, 32, 1, 4, 0, 0, gener);

  /* Initialize k, k = H(g, N) */
  unsigned char k_bytes[HASH_SIZE];
  crypto_generichash_state h_state;
  crypto_generichash_init(&h_state, NULL, 0, HASH_SIZE);
  crypto_generichash_update(&h_state, (unsigned char*) gener, 128);
  crypto_generichash_update(&h_state, (unsigned char*) prime, 128);
  crypto_generichash_final(&h_state, k_bytes, sizeof(k_bytes));
  mpz_init(k);
  mpz_import(k, sizeof(k_bytes), -1, 1, 0, 0, k_bytes);
  mpz_mod(k, k, N);

  is_initialized = true;
  init_mutex.unlock();
  return 0;
}

int generate_b(unsigned char* v_bytes_in,
	       unsigned char* b_bytes_out, 
	       unsigned char* B_bytes_out,
	       unsigned char* n_bytes_out,
	       unsigned char* hn_bytes_out) {
  mpz_t v, b, B;
  mpz_init(b);
  mpz_init(B);
  mpz_init(v);
  
  mpz_import(v, 128, -1, 1, 0, 0, v_bytes_in);
  
  randombytes_buf(b_bytes_out, BYTES_SIZE);
  mpz_import(b, BYTES_SIZE, -1, 1, 0, 0, b_bytes_out);
  
  mpz_powm(B, g, b, N);
  mpz_mul(v, v, k);
  mpz_add(B, B, v);
  mpz_mod(B, B, N);
  if (mpz_size(B)*sizeof(mp_limb_t) > BYTES_SIZE) {
    return 1;
  }
  mpz_export(B_bytes_out, NULL, -1, 1, 0, 0, B);

  randombytes_buf(n_bytes_out, KEY_SIZE);
  crypto_generichash_state h_state;
  crypto_generichash_init(&h_state, NULL, 0, KEY_SIZE);
  crypto_generichash_update(&h_state, v_bytes_in, BYTES_SIZE);
  crypto_generichash_update(&h_state, n_bytes_out, KEY_SIZE);
  crypto_generichash_final(&h_state, hn_bytes_out, KEY_SIZE);
  
  return 0;
}

int generate_sk(const char* username_in, unsigned char* A_bytes_in,
	        unsigned char* b_bytes_in, unsigned char* B_bytes_in,
	        unsigned char* salt_bytes_in, unsigned char* v_bytes_in,
	        unsigned char* k_bytes_out, unsigned char* m1_bytes_out,
	        unsigned char* m2_bytes_out) {
  mpz_t A, b, B, v, u, ss;
  mpz_init(A);
  mpz_init(b);
  mpz_init(B);
  mpz_init(v);
  mpz_init(u);
  mpz_init(ss);

  unsigned char u_bytes[HASH_SIZE];
  crypto_generichash_state h_state;
  crypto_generichash_init(&h_state, NULL, 0, HASH_SIZE);
  crypto_generichash_update(&h_state, A_bytes_in, BYTES_SIZE);
  crypto_generichash_update(&h_state, B_bytes_in, BYTES_SIZE);
  crypto_generichash_final(&h_state, u_bytes, sizeof(u_bytes));
  mpz_import(u, sizeof(u_bytes), -1, 1, 0, 0, u_bytes);
  mpz_import(A, BYTES_SIZE, -1, 1, 0, 0, A_bytes_in);
  mpz_import(B, BYTES_SIZE, -1, 1, 0, 0, B_bytes_in);
  mpz_import(b, BYTES_SIZE, -1, 1, 0, 0, b_bytes_in);
  mpz_import(v, BYTES_SIZE, -1, 1, 0, 0, v_bytes_in);
  
  mpz_powm(ss, v, u, N);
  mpz_mul(ss, ss, A);
  mpz_mod(ss, ss, N);
  mpz_powm(ss, ss, b, N);

  if (mpz_size(ss)*sizeof(mp_limb_t) > BYTES_SIZE) {
    return 1;
  }
  unsigned char ss_bytes[BYTES_SIZE];
  mpz_export(ss_bytes, NULL, -1, 1, 0, 0, ss);

  crypto_generichash_init(&h_state, NULL, 0, KEY_SIZE);
  crypto_generichash_update(&h_state, ss_bytes, BYTES_SIZE);
  crypto_generichash_final(&h_state, k_bytes_out, KEY_SIZE);

  crypto_generichash_init(&h_state, NULL, 0, HASH_SIZE);
  crypto_generichash_update(&h_state, (const unsigned char*) username_in, strlen(username_in));
  crypto_generichash_update(&h_state, salt_bytes_in, BYTES_SIZE);
  crypto_generichash_update(&h_state, A_bytes_in, BYTES_SIZE);
  crypto_generichash_update(&h_state, B_bytes_in, BYTES_SIZE);
  crypto_generichash_update(&h_state, k_bytes_out, KEY_SIZE);
  crypto_generichash_final(&h_state, m1_bytes_out, HASH_SIZE);

  crypto_generichash_init(&h_state, NULL, 0, HASH_SIZE);
  crypto_generichash_update(&h_state, A_bytes_in, BYTES_SIZE);
  crypto_generichash_update(&h_state, m1_bytes_out, HASH_SIZE);
  crypto_generichash_update(&h_state, k_bytes_out, KEY_SIZE);
  crypto_generichash_final(&h_state, m2_bytes_out, HASH_SIZE);

  return 0;
}

