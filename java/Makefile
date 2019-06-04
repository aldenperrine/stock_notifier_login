LIB_NAME = liblogin_client.so

default: jni_lib

# Ensure you have exported the JAVA_HOME variable
# should be where the home directory of the JDK is installed
# Example on OSX is /Library/Java/JavaVirtualMachines/jdk1.8.0_101.jdk/Contents/Home

jni_lib:
	@javac -h . com/stock_client_login/ClientBridge.java
	@g++ -fPIC -I"$(JAVA_HOME)/include/" -I"$(JAVA_HOME)/include/linux" -L"../cpp" -I"../cpp" -shared -o $(LIB_NAME) com_stock_client_login_ClientBridge.cpp ../cpp/client_login.cpp -lgmp -lsodium

test:
	@javac com/stock_client_login/ClientBridgeTest.java

crypto:
	javac com/stock_client_login/ClientCrypto.java

clean:
	@rm -rf *~ com/stock_client_login/*.class liblogin_client.dylib com/stock_client_login/*~
