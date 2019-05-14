package com.reactlibrary;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import com.facebook.react.ReactPackage;
import com.facebook.react.bridge.NativeModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.uimanager.ViewManager;
import com.facebook.react.bridge.JavaScriptModule;
public class ClientLoginPackage implements ReactPackage {
  @Override
  public List<NativeModule> createNativeModules(ReactApplicationContext rC) {
	return Arrays.<NativeModule>asList(new RNClientLoginModule(rC));
  }

  public List<Class<? extends JavaScriptModule>> createJSModules() {
	return Collections.emptyList();
  }

  @Override
      public List<ViewManager> createViewManagers(ReactApplicationContext rC) {
	return Collections.emptyList();
    }
}