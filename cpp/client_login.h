#ifndef STOCK_CLIENT_LOGIN
#define STOCK_CLIENT_LOGIN

#ifdef __cplusplus
extern "C" {
#endif

int lib_bytes_size();

int lib_hash_size();

int lib_key_size();
  
int init_client_lib();

int generate_registration(const char* user_pass_in,
			  unsigned char* s_bytes_out,
                          unsigned char* v_bytes_out);

int generate_a(unsigned char* a_bytes_out,
	       unsigned char* A_bytes_out);

int generate_ck(const char* username_in,
		const char* user_pass_in,
		const unsigned char* a_bytes_in,
		const unsigned char* A_bytes_in,
		const unsigned char* B_bytes_in,
		const unsigned char* s_bytes_in,
		const unsigned char* nonce_bytes_in,
		unsigned char* k_bytes_out,
		unsigned char* m1_bytes_out,
		unsigned char* m2_bytes_out,
		unsigned char* hn_bytes_out);

#ifdef __cplusplus
}
#endif

#endif
