#include "server_login.h"

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

const int NUM_TESTS = 100;

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

  unsigned char s[size];
  unsigned char v[size];
  int num_read = read(fd, buffer, size);
  memcpy(s, buffer, size);

  read(fd, buffer, size);
  memcpy(v, buffer, size);
  

  unsigned char A[size];

  read(fd, buffer, size);
  memcpy(A, buffer, size);
 

  unsigned char b[size];
  unsigned char B[size];
  memset(b, 0, sizeof(b));
  memset(B, 0, sizeof(B));
  if (generate_b(v, b, B)) {
    printf("Thread %d b creation failed\n", thread_num);
    close(fd);
    return 1;
  }
  memcpy(buffer, B, size);
  write(fd, buffer, size);

  unsigned char ss[size];
  unsigned char m1[hash_size];
  unsigned char m2[hash_size];
  memset(ss, 0, sizeof(ss));
  memset(m1, 0, sizeof(m1));
  memset(m2, 0, sizeof(m2));
  if (generate_ss(A, b, B, v, ss, m1, m2)) {
    printf("Thread %d s creation failed\n", thread_num);
    close(fd);
    return 1;
  }

  unsigned char mv[hash_size];
  read(fd, buffer, hash_size);
  memcpy(mv, buffer, hash_size);

  if (compare_hex(mv, m1, hash_size)) {
    printf("Thread %d m validation failed\n", thread_num);
    close(fd);
    return 1;
  }

  memcpy(buffer, m2, hash_size);
  write(fd, buffer, hash_size);

  printf("Thread %d finished\n", thread_num);
  close(fd);
  return 0;
}

int recieve_client_conns() {
  int sockfd;
  unsigned int len;
  struct sockaddr_in serv, cli;
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("socket() error!\n %s\n", strerror(errno));
    return 1;
  }
  bzero((char*) &serv, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_addr.s_addr = INADDR_ANY;
  serv.sin_port = htons((short) 8000);
  if(bind(sockfd, (struct sockaddr *) &serv, sizeof(serv)) < 0) {
    printf("bind() error!\n %s\n", strerror(errno));
    return 1;
  }
  if(listen(sockfd, 0) < 0) {
    printf("listen() error!\n %s\n", strerror(errno));
    return 1;
  }
  len = sizeof(cli);
  std::vector<std::thread> threads;
  for (int i = 0; i < NUM_TESTS; ++i) {
    int fd;
    if((fd = accept(sockfd, (struct sockaddr *) &cli, &len)) < 0) {
      printf("accept() error!\n %s\n", strerror(errno));
      return 1;
    }
    threads.push_back(std::thread(test_client, fd, i));
  }
  for (int i = 0; i < NUM_TESTS; ++i) {
    threads[i].join();
  }
  close(sockfd);
  return 0;
}

int main(int argc, char** argv) {
  if (init_server_lib()) {
    printf("Failed to init server library\n");
    return 1;
  }

  return recieve_client_conns();
}
