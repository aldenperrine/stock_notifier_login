package com.stock_client_login;

import java.util.Arrays;

public class ClientBridge {
    static {
	System.loadLibrary("login_client");
    }

    public static void main(String[] args) {
	System.out.println("Yay!");
	jni_init_client_lib();
	System.out.println(jni_lib_bytes_size());
	System.out.println(jni_lib_hash_size());
	RegistrationValues rvals = jni_generate_registration("Cats");
	AValues avals = jni_generate_a();
	VerificationValues vvals = jni_generate_ck("Cats", "Cats", avals.a, avals.A, avals.A, rvals.s, rvals.s);
	System.out.println(vvals.m2.length);
	System.out.println(Arrays.equals(hexToBytes(bytesToHex(rvals.s)), rvals.s));
    }

    public static native int jni_init_client_lib();

    public static native int jni_lib_bytes_size();

    public static native int jni_lib_key_size();
    
    public static native int jni_lib_hash_size();

    public static native RegistrationValues jni_generate_registration(String user_pass);

    public static native AValues jni_generate_a();
    
    public static native VerificationValues 
	jni_generate_ck(String username, String user_pass, byte[] a,
			byte[] A, byte[] B, byte[] s, byte[] n);
    
    private final static char[] hexArray = "0123456789ABCDEF".toCharArray();
    public static String bytesToHex(byte[] bytes) {
	char[] hexChars = new char[bytes.length * 2];
	for (int j = 0; j < bytes.length; j++) {
	    int v = bytes[j] & 0xFF;
	    hexChars[j * 2] = hexArray[v >>> 4];
	    hexChars[j * 2 + 1] = hexArray[v & 0x0F];
	}
	return new String(hexChars);
    }

    public static byte[] hexToBytes(String s) {
	byte[] bytes = new byte[s.length()/2];
	for(int i = 0; i < s.length(); i+=2) {	    
	    bytes[i/2] = (byte) ((Character.digit(s.charAt(i), 16) << 4) +
				 Character.digit(s.charAt(i+1), 16));
	}
	return bytes;
    }

}

class RegistrationValues {
    public RegistrationValues(byte[] si, byte[] vi) {
	s = si;
	v = vi;
    }

    public byte[] s;
    public byte[] v;
}

class AValues {
    public AValues(byte[] ai, byte[] Ai) {
	a = ai;
	A = Ai;
    }

    public byte[] a;
    public byte[] A;
}

class VerificationValues {
    public VerificationValues(byte[] cki, byte[] m1i, byte[] m2i, byte[] hvi) {
	ck = cki;
	m1 = m1i;
	m2 = m2i;
	hv = hvi;
    }

    public byte[] ck;
    public byte[] m1;
    public byte[] m2;
    public byte[] hv;
}
