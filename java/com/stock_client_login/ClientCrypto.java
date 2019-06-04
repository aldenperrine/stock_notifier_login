package com.stock_client_login;

import java.util.Base64;
import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;
import java.security.*;
import java.security.spec.X509EncodedKeySpec;
import java.lang.System.*;

public class ClientCrypto {
    private final String publicKeyString = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAnQGlH1nBCp8Fj31GpMjeQh4YQ5fz2SPwJQSMAUpRLrPMCRvb2jvxKeRO4nw4anIzXR0Xaao1UV0obIaJT87sayE6HfHLH3pXTT7psJj0QWS6R/kKVWT5X/i5RT3C7sWJnIs/XlKCADc2t9YzWcbPjDPyR6tXTURXzJ/RZbValGBK3CSVYopOTI+Y/8fVC1XoW1NQynf7ShDwoHvhpBPh916ezCz11le6kz0HW2X2qTHT2Xg+xEkgCzt4xsVdT5M7TpI6foedR8HRiVE7M212wjD7j5wis4bQgsLWGMoFWWEOWpsFl9HESicqnyZObz+uymw4ND8VuxgDaz6wXmEzkwIDAQAB";
    
    private Cipher encrypter;
    private Base64.Encoder encoder;
    
    public static void main(String[] args) {
	try {
	    System.out.println((new ClientCrypto()).encrypt(args[0]));
	    
	} catch (Exception e) {
	    e.printStackTrace();
	    System.out.println(e);
	}
    }

    public ClientCrypto() throws Exception {
	PublicKey pub = makePublicKey();
	encrypter = Cipher.getInstance("RSA/ECB/OAEPWithSHA-256AndMGF1Padding");
	encrypter.init(Cipher.ENCRYPT_MODE, pub);

	encoder = Base64.getEncoder();
    }

    public String encrypt(String plaintext) throws GeneralSecurityException {
	return new String(encoder.encode(encrypter.doFinal(plaintext.getBytes())));
    }
    
    public PublicKey makePublicKey() throws Exception {
	byte[] keyBytes = Base64.getDecoder().decode(publicKeyString);

	X509EncodedKeySpec spec =
	    new X509EncodedKeySpec(keyBytes);
	KeyFactory kf = KeyFactory.getInstance("RSA");
	return kf.generatePublic(spec);
    }

    
}
