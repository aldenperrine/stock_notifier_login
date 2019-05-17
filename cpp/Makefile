default: test server_test client_test

server:	
	@g++ -shared -o server_lib.so -fPIC server_login.cpp -lgmp -lsodium
	@cp server_lib.so /usr/local/lib

client:
	@g++ -shared -o client_lib.so -fPIC client_login.cpp -lgmp -lsodium
	@cp client_lib.so /usr/local/lib

test: client server
	@g++ -L. -o login_test login_test.cpp client_lib.so server_lib.so -lgmp -lsodium

server_test: server
	@g++ -std=c++11 -L. -o server_test server_inter_test.cpp server_lib.so -lgmp -lsodium -lpthread

client_test: client
	@g++ --std=c++11 -L. -o client_test client_inter_test.cpp client_lib.so -lgmp -lsodium -lpthread

clean:
	@-rm -rf server_lib.so client_lib.so login_test server_test client_test *~

