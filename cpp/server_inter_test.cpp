#include "server_login.h"

#include <vector>
#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <mutex>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

const int NUM_TESTS = 10;
std::mutex output_mutex;

int compare_hex(unsigned char str1[], unsigned char str2[], size_t size) {
  for (int i = 0; i < size; ++i) {
    if (str1[i] != str2[i]) {
      return i+1;
    }
  }
  return 0;
}

std::string binary_conv(unsigned char* binary, size_t size) {
  std::stringstream ss;
  for (int i = 0; i < size; ++i) {
    ss << std::hex << std::setfill('0') << std::setw(2) << (int) binary[i];
  }
  return ss.str();
}



int read_string(int fd, char* string, int max_size) {
  char last_char[2] = "\0";
  int loc = 0;
  do {
    read(fd, &last_char, 1);
    string[loc++] = last_char[0];
  } while(last_char[0] != '\0' && loc <= max_size);
  return loc <= max_size ? 0 : 1;
}

int test_client(int fd, int thread_num) {
  int size = lib_bytes_size();
  int hash_size = lib_hash_size();
  int key_size = lib_key_size();
  unsigned char buffer[1024];

  unsigned char s[size];
  unsigned char v[size];
  int num_read = read(fd, buffer, size);
  memcpy(s, buffer, size);

  read(fd, buffer, size);
  memcpy(v, buffer, size);

  char username[size];
  if (read_string(fd, username, size)) {
    output_mutex.lock();
    printf("Unable to process username\n");
    output_mutex.unlock();
    close(fd);
    return 1;
  }

  unsigned char A[size];

  read(fd, buffer, size);
  memcpy(A, buffer, size);
 

  unsigned char b[size];
  unsigned char B[size];
  unsigned char n[key_size];
  unsigned char h[key_size];
  memset(b, 0, sizeof(b));
  memset(B, 0, sizeof(B));
  memset(n, 0, sizeof(n));
  memset(h, 0, sizeof(h));
    if (generate_b(v, b, B, n, h)) {
    output_mutex.lock();
    printf("Thread %d b creation failed\n", thread_num);
    output_mutex.unlock();
    close(fd);
    return 1;
  }
  memcpy(buffer, B, size);
  write(fd, buffer, size);
  write(fd, n, key_size);
  
  unsigned char sk[key_size];
  unsigned char m1[hash_size];
  unsigned char m2[hash_size];
  memset(sk, 0, sizeof(sk));
  memset(m1, 0, sizeof(m1));
  memset(m2, 0, sizeof(m2));
  if (generate_sk(username, A, b, B, s, v, sk, m1, m2)) {
    output_mutex.lock();
    printf("Thread %d s creation failed\n", thread_num);
    output_mutex.unlock();
    close(fd);
    return 1;
  }

  unsigned char mv[hash_size];
  read(fd, mv, hash_size);

  unsigned char hv[key_size];
  read(fd, hv, key_size);

  if (compare_hex(hv, h, key_size)) {
    output_mutex.lock();
    printf("Thread %d hash failed\n", thread_num);
    output_mutex.unlock();
    close(fd);
    return 1;
  }
  
  if (compare_hex(mv, m1, hash_size)) {
    output_mutex.lock();
    printf("Thread %d m validation failed\n", thread_num);
    output_mutex.unlock();
    close(fd);
    return 1;
  }

  memcpy(buffer, m2, hash_size);
  write(fd, buffer, hash_size);

  output_mutex.lock();
  printf("Thread %d finished\n", thread_num);
  output_mutex.unlock();
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
