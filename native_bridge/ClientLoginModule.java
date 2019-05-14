package com.reactlibrary;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.Callback;

public class ClientLoginModule extends ReactContextBaseJavaModule {
  private final ReactApplicationContext reactContext;
  private boolean isInit;

  public ClientLoginModule(ReactApplicationContext reactContext) {
    super(reactContext);
    this.reactContext = reactContext;
    isInit = ClientBridge.jni_init_client_lib() == 0;
  }

  @Override
  public String getName() {
      return "ClientLogin";
  }

  @ReactMethod
  public void test(Promise p) {
      p.resolve("Yay!");
  }

  @ReactMethod
  public void generateAs(Promise p) {
      if (isInit) {
	  AValues avals = ClientBridge.jni_generate_a();
	  if (avals == NULL || avals.a == NULL) {
	      p.reject("ERR", "Could not generate as");
	      return;
	  }
	  WritableMap retval = new WritableNativeMap();
	  retval.putString("a", bytesToHex(avals.a));
	  retval.putString("A", bytesToHex(avals.A));
	  p.resolve(retval);
      } else {
	  p.reject("ERR", "Library not initialized");
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
