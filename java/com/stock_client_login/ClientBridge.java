package com.stock_client_login;

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
	VerificationValues vvals = jni_generate_cs("Cats", avals.a, avals.A, avals.A, rvals.s);
	System.out.println(vvals.m2.length);
    }

    public static native int jni_init_client_lib();

    public static native int jni_lib_bytes_size();

    public static native int jni_lib_hash_size();

    public static native RegistrationValues jni_generate_registration(String user_pass);

    public static native AValues jni_generate_a();
    
    public static native VerificationValues 
	jni_generate_cs(String user_pass, byte[] a,
			byte[] A, byte[] B, byte[] s);
    
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
    public VerificationValues(byte[] m1i, byte[] m2i) {
	m1 = m1i;
	m2 = m2i;
    }

    public byte[] m1;
    public byte[] m2;
}