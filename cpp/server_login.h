#ifndef STOCK_SERVER_LOGIN
#define STOCK_SERVER_LOGIN

#ifdef __cplusplus
extern "C" {
#endif

int lib_bytes_size();
  
int lib_hash_size();

int lib_key_size();
  
int init_server_lib();

int generate_b(unsigned char* v_bytes_in, unsigned char* b_bytes_out,
               unsigned char* B_bytes_out);

int generate_sk(const char* username_in, unsigned char* A_bytes_in,
	        unsigned char* b_bytes_in, unsigned char* B_bytes_in,
	        unsigned char* salt_bytes_in, unsigned char* v_bytes_in,
                unsigned char* k_bytes_out, unsigned char* m1_bytes_out,
	        unsigned char* m2_bytes_out);

#ifdef __cplusplus
}
#endif

#endif
