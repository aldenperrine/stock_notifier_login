default: test server_test client_test

server:	
	g++ -shared -lgmp -lsodium -Wl,-install_name,server_lib.so -o server_lib.so -fPIC server_login.cpp

client:
	g++ -shared -lgmp -lsodium -Wl,-install_name,client_lib.so -o client_lib.so -fPIC client_login.cpp

test: client server
	g++ -lgmp -lsodium -L. -o login_test login_test.cpp client_lib.so server_lib.so

server_test: server
	g++ -std=c++11 -L. -o server_test server_inter_test.cpp server_lib.so

client_test: client
	g++ -std=c++11 -L. -o client_test client_inter_test.cpp client_lib.so

clean:
	rm -rf server_lib.so client_lib.so login_test server_test client_test

