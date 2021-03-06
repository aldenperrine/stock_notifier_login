package com.stock_client_login;

import java.io.*;
import java.net.*;
import java.util.Arrays;
import java.nio.charset.*;

public class ClientBridgeTest {
    public static void main(String[] args) {
	System.out.println("Starting client test...");
	ClientBridge.jni_init_client_lib();
  
	int NUM_TESTS = 1;
	TestThread[] threads = new TestThread[NUM_TESTS];
	for (int i = 0; i < NUM_TESTS; ++i) {
	    threads[i] = new TestThread();
	    threads[i].start();
	}
	for (int i = 0; i < NUM_TESTS; ++i) {
	    try {
		threads[i].join();
	    } catch (Exception e) {
		continue;
	    }
	}
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
	    
	    String un = "username";
	    byte[] user_bytes = un.getBytes(StandardCharsets.US_ASCII);
	    byte[] null_byte = new byte[1];
	    null_byte[0] = 0;
	    String up = "username:password";
	    int byteSize = ClientBridge.jni_lib_bytes_size();
	    int hashSize = ClientBridge.jni_lib_hash_size();
	    int keySize = ClientBridge.jni_lib_key_size();
	    
	    RegistrationValues rvals = ClientBridge.jni_generate_registration(up);
	    AValues avals = ClientBridge.jni_generate_a();
	    
	    out.write(ClientBridge.bytesToHex(rvals.s).getBytes(StandardCharsets.US_ASCII), 0, byteSize*2);
	    out.write(ClientBridge.bytesToHex(rvals.v).getBytes(StandardCharsets.US_ASCII), 0, byteSize*2);
	    out.write(user_bytes, 0, user_bytes.length);
	    out.write(null_byte, 0, 1);
	    out.write(ClientBridge.bytesToHex(avals.A).getBytes(StandardCharsets.US_ASCII), 0, byteSize*2);
	    

	    byte[] BHex = new byte[byteSize*2];
	    in.readFully(BHex, 0, byteSize*2);
	    String BStr = new String(BHex);
	    
	    byte[] nHex = new byte[keySize*2];
	    in.readFully(nHex, 0, keySize*2);
	    String nStr = new String(nHex);

	    VerificationValues vvals = ClientBridge.jni_generate_ck(un, up, avals.a, avals.A,
								    ClientBridge.hexToBytes(BStr),
								    rvals.s,
								    ClientBridge.hexToBytes(nStr));
	    
	    out.write(ClientBridge.bytesToHex(vvals.m1).getBytes(StandardCharsets.US_ASCII), 0, hashSize*2);

	    out.write(ClientBridge.bytesToHex(vvals.hv).getBytes(StandardCharsets.US_ASCII), 0, keySize*2);
	    
	    byte[] mvHex = new byte[hashSize*2];
	    in.readFully(mvHex, 0, hashSize*2);
	    byte[] mv = ClientBridge.hexToBytes(new String(mvHex));
	    
	    
	    System.out.println(Arrays.equals(mv, vvals.m2));
	
	} catch (IOException e) {
	    System.out.println("Caught an IOException");
	    System.out.println(e.getStackTrace().toString());
	}
    }
}
