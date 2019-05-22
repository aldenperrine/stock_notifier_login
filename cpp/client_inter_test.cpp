#include "client_login.h"

#include <vector>
#include <thread>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

const int NUM_TESTS = 1;

int compare_hex(unsigned char str1[], unsigned char str2[], size_t size) {
  for (int i = 0; i < size; ++i) {
    if (str1[i] != str2[i]) {
      return i+1;
    }
  }
  return 0;
}

int test_client(int fd, int thread_num) {
  int size = lib_bytes_size();
  int hash_size = lib_hash_size();
  unsigned char buffer[1024];

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
    printf("Thread %d registration failed\n", thread_num);
    close(fd);
    return 1;
  }
  memcpy(buffer, s, size);
  memcpy(buffer+size, v, size);
  write(fd, buffer, size*2);

  write(fd, username, strlen(username)+1);
  printf("Wrote s, v, and username\n");
  
  unsigned char a[size];
  unsigned char A[size];
  memset(a, 0, sizeof(a));
  memset(A, 0, sizeof(A));
  if (generate_a(a, A)) {
    printf("Thread %d A creation failed\n", thread_num);
    close(fd);
    return 1;
  }
  memcpy(buffer, A, size);
  write(fd, buffer, size);

  printf("Wrote A, waiting for B\n");
  
  unsigned char B[size];
  read(fd, buffer, 1024);
  memcpy(B, buffer, size);

  unsigned char kc[lib_key_size()];
  unsigned char m1[hash_size];
  unsigned char m2[hash_size];
  memset(kc, 0, sizeof(kc));
  memset(m1, 0, sizeof(m1));
  memset(m2, 0, sizeof(m2));
  if (generate_ck(username, userpass, a, A, B, s, kc, m1, m2)) {
    printf("Thread %d s creation failed\n", thread_num);
    close(fd);
    return 1;
  }
  memcpy(buffer, m1, hash_size);
  write(fd, buffer, hash_size);
  printf("Sent m1, waiting for m2\n");
  
  unsigned char mv[hash_size];
  read(fd, buffer, 1024);
  memcpy(mv, buffer, hash_size);
  
  if (compare_hex(mv, m2, hash_size)) {
    printf("Thread %d m validation failed\n", thread_num);
    close(fd);
    return 1;
  }

  printf("Thread %d finished\n", thread_num);
  close(fd);
  return 0;
}

int connect_server(const char* ip) {
  int serverfds[NUM_TESTS];
  struct sockaddr_in address;
  struct hostent* server;
  for (int i = 0; i < NUM_TESTS; ++i) {
    if((serverfds[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      printf("Failed to create socket\n%s\n", strerror(errno));
      return 1;
    }
    if((server = gethostbyname(ip)) == NULL) {
      printf("Failed to find the server\n%s\n", strerror(errno));
      return 1;
    }
    bzero((char *) &address, sizeof(address));
    address.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
	  (char *)&address.sin_addr.s_addr,
	  server->h_length);
    address.sin_port = htons((short) 8000);
    if (connect(serverfds[i],
		(struct sockaddr *) &address,
		sizeof(address)) < 0) {
      printf("Failed to connect socket\n%s\n", strerror(errno));
      return 1;
    }
  }

  std::vector<std::thread> threads;
  for (int i = 0; i < NUM_TESTS; ++i) {
    threads.emplace_back(test_client, serverfds[i], i);
  }
  for (int i = 0; i < NUM_TESTS; ++i) {
    threads[i].join();
  }
  return 0;
}

int main(int argc, char** argv) {
  if (init_client_lib()) {
    printf("Failed to init client library\n");
    return 1;
  }

  if (argc != 2) {
    printf("Invalid number of arguments\n");
    return 1;
  }
  
  return connect_server(argv[1]);
}
