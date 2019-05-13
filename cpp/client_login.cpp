#include "client_login.h"

#include <string.h>
#include <stdint.h>
#include <gmp.h>
#include <sodium.h>

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

bool is_initialized = false;
mpz_t N;
mpz_t g;
mpz_t k;

const int BYTES_SIZE = sizeof(prime);
const int HASH_SIZE = 
  crypto_generichash_BYTES_MAX < BYTES_SIZE ? 
  crypto_generichash_BYTES_MAX : BYTES_SIZE;

int lib_bytes_size() {
  return BYTES_SIZE;
}

int lib_hash_size() {
  return HASH_SIZE;
}

int init_client_lib() {
  if (is_initialized) {
    return 0;
  }
  if (sodium_init() < 0) {
    return 1;
  }
  
  /* Initialize N, a large prime which is the base for the ring of integers */
  
  mpz_init(N);
  mpz_import(N, 32, 1, 4, 0, 0, prime);

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
  return 0;
}

int generate_registration(const char* user_pass_in, unsigned char* s_bytes_out, 
			  unsigned char* v_bytes_out) {
  mpz_t x, v;
  mpz_init(x);
  mpz_init(v);

  randombytes_buf(s_bytes_out, BYTES_SIZE);

  unsigned char hash[HASH_SIZE];
  crypto_generichash_state hash_state;
  crypto_generichash_init(&hash_state, NULL, 0, HASH_SIZE);
  crypto_generichash_update(&hash_state, (unsigned char*) s_bytes_out, BYTES_SIZE);
  crypto_generichash_update(&hash_state, (unsigned char*) user_pass_in, strlen(user_pass_in));
  crypto_generichash_final(&hash_state, hash, sizeof hash);

  mpz_import(x, HASH_SIZE, -1, 1, 0, 0, hash);
  mpz_powm(v, g, x, N);
  
  if (mpz_size(v)*sizeof(mp_limb_t) > BYTES_SIZE) {
    return 1;
  }
  
  mpz_export(v_bytes_out, NULL, -1, 1, 0, 0, v);
  return 0;
}

int generate_a(unsigned char* a_bytes_out, unsigned char* A_bytes_out) {
  mpz_t a, A;
  mpz_init(a);
  mpz_init(A);
  
  randombytes_buf(a_bytes_out, BYTES_SIZE);
  mpz_import(a, BYTES_SIZE, -1, 1, 0, 0, a_bytes_out);
  
  mpz_powm(A, g, a, N);
  if (mpz_size(A)*sizeof(mp_limb_t) > BYTES_SIZE) {
    return 1;
  }
  mpz_export(A_bytes_out, NULL, -1, 1, 0, 0, A);
  return 0;
}

int generate_cs(const char* user_pass_in, const unsigned char* a_bytes_in,
		const unsigned char* A_bytes_in, const unsigned char* B_bytes_in,
		const unsigned char* salt_bytes_in, unsigned char* cs_bytes_out,
		unsigned char* m1_bytes_out, unsigned char* m2_bytes_out) {
  mpz_t a, A, B, u, x, cs, cb, ce;
  mpz_init(a);
  mpz_init(A);
  mpz_init(B);
  mpz_init(u);
  mpz_init(x);
  mpz_init(cs);
  mpz_init(cb);
  mpz_init(ce);

  unsigned char u_bytes[HASH_SIZE];
  crypto_generichash_state h_state;
  crypto_generichash_init(&h_state, NULL, 0, HASH_SIZE);
  crypto_generichash_update(&h_state, A_bytes_in, 128);
  crypto_generichash_update(&h_state, B_bytes_in, 128);
  crypto_generichash_final(&h_state, u_bytes, sizeof(u_bytes));
  mpz_import(u, sizeof(u_bytes), -1, 1, 0, 0, u_bytes);
  mpz_import(a, BYTES_SIZE, -1, 1, 0, 0, a_bytes_in);
  mpz_import(B, BYTES_SIZE, -1, 1, 0, 0, B_bytes_in);
  
  unsigned char hash[HASH_SIZE];
  crypto_generichash_init(&h_state, NULL, 0, HASH_SIZE);
  crypto_generichash_update(&h_state, (unsigned char*) salt_bytes_in, BYTES_SIZE);
  crypto_generichash_update(&h_state, (unsigned char*) user_pass_in, strlen(user_pass_in));
  crypto_generichash_final(&h_state, hash, sizeof hash);
  mpz_import(x, HASH_SIZE, -1, 1, 0, 0, hash);

  mpz_powm(cb, g, x, N);
  mpz_mul(cb, cb, k);
  mpz_sub(cb, B, cb);
  mpz_mod(cb, cb, N);
  mpz_mul(ce, u, x);
  mpz_add(ce, ce, a);
  mpz_powm(cs, cb, ce, N);

  if (mpz_size(cs)*sizeof(mp_limb_t) > BYTES_SIZE) {
    return 1;
  }
  mpz_export(cs_bytes_out, NULL, -1, 1, 0, 0, cs);

  crypto_generichash_init(&h_state, NULL, 0, HASH_SIZE);
  crypto_generichash_update(&h_state, A_bytes_in, BYTES_SIZE);
  crypto_generichash_update(&h_state, B_bytes_in, BYTES_SIZE);
  crypto_generichash_update(&h_state, cs_bytes_out, BYTES_SIZE);
  crypto_generichash_final(&h_state, m1_bytes_out, HASH_SIZE);

  crypto_generichash_init(&h_state, NULL, 0, HASH_SIZE);
  crypto_generichash_update(&h_state, A_bytes_in, BYTES_SIZE);
  crypto_generichash_update(&h_state, m1_bytes_out, HASH_SIZE);
  crypto_generichash_update(&h_state, cs_bytes_out, BYTES_SIZE);
  crypto_generichash_final(&h_state, m2_bytes_out, HASH_SIZE);

  return 0;
}
