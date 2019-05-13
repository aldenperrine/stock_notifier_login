package com.stock_client_login;

import java.io.*;
import java.net.*;
import java.util.Arrays;

public class ClientBridgeTest {
    public static void main(String[] args) {
	System.out.println("Starting client test...");
	ClientBridge.jni_init_client_lib();
  
	int NUM_TESTS = 100;
	TestThread[] threads = new TestThread[NUM_TESTS];
	for (int i = 0; i < NUM_TESTS; ++i) {
	    threads[i] = new TestThread();
	    threads[i].start();
	}
	//for (int i = 0; i < NUM_TESTS; ++i) {
	//    threads[i].join();
	//}
    }

    private final static char[] hexArray = "0123456789abcdef".toCharArray();
    public static String bytesToHex(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for ( int j = 0; j < bytes.length; j++ ) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = hexArray[v >>> 4];
            hexChars[j * 2 + 1] = hexArray[v & 0x0F];
        }
        return new String(hexChars);
    }
}

class TestThread extends Thread {
    public void run() {
	try {
	    Socket socket = new Socket("127.0.0.1", 8000);
	    DataOutputStream out = new DataOutputStream(socket.getOutputStream());
	    DataInputStream in = new DataInputStream(socket.getInputStream());
	    
	    
	    String up = "username:password";
	    int byteSize = ClientBridge.jni_lib_bytes_size();
	    int hashSize = ClientBridge.jni_lib_hash_size();
	    
	    RegistrationValues rvals = ClientBridge.jni_generate_registration(up);
	    AValues avals = ClientBridge.jni_generate_a();
	    
	    out.write(rvals.s, 0, byteSize);
	    out.write(rvals.v, 0, byteSize);
	    
	    out.write(avals.A, 0, byteSize);
	    

	    byte[] B = new byte[byteSize];
	    in.readFully(B, 0, byteSize);


	    VerificationValues vvals = ClientBridge.jni_generate_cs(up, avals.a, avals.A,
						   B, rvals.s);
	    
	    out.write(vvals.m1, 0, hashSize);

	    byte[] mv = new byte[hashSize];
	    in.readFully(mv, 0, hashSize);
	    
	    System.out.println(Arrays.equals(mv, vvals.m2));
	
	} catch (IOException e) {
	    System.out.println("Caught an IOException");
	    System.out.println(e.getStackTrace());
	}
    }
}