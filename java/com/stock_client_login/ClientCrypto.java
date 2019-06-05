package com.stock_client_login;

import java.util.Base64;
import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;
import javax.crypto.spec.IvParameterSpec;
import java.security.*;
import java.security.SecureRandom;
import java.security.spec.X509EncodedKeySpec;
import java.lang.System.*;

public class ClientCrypto {
    private static final String publicKeyString = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAnQGlH1nBCp8Fj31GpMjeQh4YQ5fz2SPwJQSMAUpRLrPMCRvb2jvxKeRO4nw4anIzXR0Xaao1UV0obIaJT87sayE6HfHLH3pXTT7psJj0QWS6R/kKVWT5X/i5RT3C7sWJnIs/XlKCADc2t9YzWcbPjDPyR6tXTURXzJ/RZbValGBK3CSVYopOTI+Y/8fVC1XoW1NQynf7ShDwoHvhpBPh916ezCz11le6kz0HW2X2qTHT2Xg+xEkgCzt4xsVdT5M7TpI6foedR8HRiVE7M212wjD7j5wis4bQgsLWGMoFWWEOWpsFl9HESicqnyZObz+uymw4ND8VuxgDaz6wXmEzkwIDAQAB";
    private static final int AES_BLOCK_SIZE = 32;
    private static final int AES_RAND_SIZE = 16;
    
    private Cipher encrypter;
    private Base64.Encoder encoder;
    private Base64.Decoder decoder;
    private byte[] iv;
    private IvParameterSpec ivSpec;
    private SecretKeySpec skey;
    
    public static void main(String[] args) {
	try {
	    System.out.println((new ClientCrypto()).encrypt(args[0]));
	    System.out.println((new ClientCrypto()).encrypt(args[0]));
	    ClientCrypto crypto = new ClientCrypto();
	    crypto.setAesKey(Base64.getDecoder().decode("p5R0Bwkokg1hlvN709mT67EV3U1w4g7f0hEBMJnuSzE=".getBytes("ascii")));
	    System.out.println(crypto.aes_encrypt("cats"));
	    System.out.println(crypto.aes_decrypt("lsrOG+qlzWeRjzNxgKiQTrRfLQC44mYYI+La07nI23Y="));
	} catch (Exception e) {
	    e.printStackTrace();
	    System.out.println(e);
	}
    }

    public ClientCrypto() throws Exception {
	encoder = Base64.getEncoder();
	decoder = Base64.getDecoder();

	PublicKey pub = makePublicKey();
	encrypter = Cipher.getInstance("RSA/ECB/OAEPWithSHA-256AndMGF1Padding");
	encrypter.init(Cipher.ENCRYPT_MODE, pub);

	iv = new byte[AES_RAND_SIZE];
	(new SecureRandom()).nextBytes(iv);
	ivSpec = new IvParameterSpec(iv);
    }

    public String encrypt(String plaintext) throws Exception {
	return new String(encoder.encode(encrypter.doFinal(plaintext.getBytes("UTF-8"))));
    }
    
    public void setAesKey(byte[] key) throws Exception {
	if (key.length != 32)
	    return;
	skey = new SecretKeySpec(key, "AES");
    }

    public String aes_encrypt(String plaintext) throws Exception {
	Cipher ci = Cipher.getInstance("AES/CBC/PKCS5Padding");
	ci.init(Cipher.ENCRYPT_MODE, skey, ivSpec);
	byte[] ct = ci.doFinal(plaintext.getBytes("UTF-8"));
	byte[] ctiv = new byte[ct.length+iv.length];
	for (int i = 0; i < iv.length; ++ i)
	    ctiv[i] = iv[i];
	for (int i = 0; i < ct.length; ++ i)
	    ctiv[i+iv.length] = ct[i];
	return new String(encoder.encode(ctiv));
    }
    
    public String aes_decrypt(String ciphertext) throws Exception {	
	byte[] ctiv = decoder.decode(ciphertext);
	
	byte[] siv = new byte[AES_RAND_SIZE];
	for (int i = 0; i < siv.length; ++i)
	    siv[i] = ctiv[i];

	byte[] ct = new byte[ctiv.length - AES_RAND_SIZE];
	for (int i = 0; i < ct.length; ++i)
	    ct[i] = ctiv[i+AES_RAND_SIZE];
	
	IvParameterSpec sivSpec = new IvParameterSpec(siv);
	
	Cipher ci = Cipher.getInstance("AES/CBC/PKCS5Padding");
	ci.init(Cipher.DECRYPT_MODE, skey, sivSpec);
	byte[] pt = ci.doFinal(ct);

	return new String(pt, "UTF-8");
    }


    public PublicKey makePublicKey() throws Exception {
	byte[] keyBytes = decoder.decode(publicKeyString.getBytes("ascii"));

	X509EncodedKeySpec spec =
	    new X509EncodedKeySpec(keyBytes);
	KeyFactory kf = KeyFactory.getInstance("RSA");
	return kf.generatePublic(spec);
    }

    
}
