#include "com_stock_client_login_ClientBridge.h"

#include "client_login.h"

#include <iostream>
#include <sstream>
#include <iomanip>

std::string binary_conv(unsigned char* binary, size_t size) {
  std::stringstream ss;
  for (int i = 0; i < size; ++i) {
    ss << std::hex << std::setfill('0') << std::setw(2) << (int) binary[i];
  }
  return ss.str();
}

JNIEXPORT jint JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1init_1client_1lib
(JNIEnv * env, jclass) {
  return init_client_lib();
}

JNIEXPORT jint JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1lib_1bytes_1size
(JNIEnv * env, jclass) {
  return lib_bytes_size();
}

JNIEXPORT jint JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1lib_1hash_1size
(JNIEnv * env, jclass) {
  return lib_hash_size();
}

JNIEXPORT jint JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1lib_1key_1size
(JNIEnv * env, jclass) {
  return lib_key_size();
}

JNIEXPORT jobject JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1generate_1registration
(JNIEnv * env, jclass, jstring up) {
  const char* user_pass = env->GetStringUTFChars(up, NULL);
  const int size = lib_bytes_size();

  unsigned char s[size];
  unsigned char v[size];
  if (generate_registration(user_pass, s, v)) {
    return NULL;
  }

  jbyteArray s_array = env->NewByteArray(size);
  env->SetByteArrayRegion(s_array, 0, size, (jbyte*) s);

  jbyteArray v_array = env->NewByteArray(size);
  env->SetByteArrayRegion(v_array, 0, size, (jbyte*) v);

  jclass r_ret_class = env->FindClass("com/stock_client_login/RegistrationValues");
  jmethodID construct = env->GetMethodID(r_ret_class, "<init>", "([B[B)V");
  jobject retval = env->NewObject(r_ret_class, construct, s_array, v_array);
  return retval;
}

JNIEXPORT jobject JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1generate_1a
(JNIEnv * env, jclass) {
  const int size = lib_bytes_size();

  unsigned char a[size];
  unsigned char A[size];
  if (generate_a(a, A)) {
    return NULL;
  }

  jbyteArray a_array = env->NewByteArray(size);
  env->SetByteArrayRegion(a_array, 0, size, (jbyte*) a);

  jbyteArray A_array = env->NewByteArray(size);
  env->SetByteArrayRegion(A_array, 0, size, (jbyte*) A);

  jclass r_ret_class = env->FindClass("com/stock_client_login/AValues");
  jmethodID construct = env->GetMethodID(r_ret_class, "<init>", "([B[B)V");
  jobject retval = env->NewObject(r_ret_class, construct, a_array, A_array);
  return retval;
}


JNIEXPORT jobject JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1generate_1ck
(JNIEnv * env, jclass,
 jstring un,
 jstring up,
 jbyteArray aArray,
 jbyteArray AArray,
 jbyteArray BArray,
 jbyteArray sArray,
 jbyteArray nArray) {
  const int size = lib_hash_size();
  const int key_size = lib_key_size();
  const char* user_pass = env->GetStringUTFChars(up, NULL);
  const char* username = env->GetStringUTFChars(un, NULL);
  
  jbyte* a = env->GetByteArrayElements(aArray, NULL);
  jbyte* A = env->GetByteArrayElements(AArray, NULL);
  jbyte* B = env->GetByteArrayElements(BArray, NULL);
  jbyte* s = env->GetByteArrayElements(sArray, NULL);
  jbyte* n = env->GetByteArrayElements(nArray, NULL);
  
  unsigned char sk[key_size];
  unsigned char m1[size];
  unsigned char m2[size];
  unsigned char hv[key_size];
  
  if (generate_ck(username, user_pass, (unsigned char*) a, (unsigned char*) A,
		  (unsigned char*) B, (unsigned char*) s, (unsigned char*) n,
		  sk, m1, m2, hv)) {
    return NULL;
  }

  env->ReleaseByteArrayElements(aArray, a, 0);
  env->ReleaseByteArrayElements(AArray, A, 0);
  env->ReleaseByteArrayElements(BArray, B, 0);
  env->ReleaseByteArrayElements(sArray, s, 0);
  env->ReleaseByteArrayElements(nArray, n, 0);
  
  jbyteArray sk_array = env->NewByteArray(key_size);
  env->SetByteArrayRegion(sk_array, 0, key_size, (jbyte*) sk);
  
  jbyteArray m1_array = env->NewByteArray(size);
  env->SetByteArrayRegion(m1_array, 0, size, (jbyte*) m1);

  jbyteArray m2_array = env->NewByteArray(size);
  env->SetByteArrayRegion(m2_array, 0, size, (jbyte*) m2);

  jbyteArray hv_array = env->NewByteArray(key_size);
  env->SetByteArrayRegion(hv_array, 0, key_size, (jbyte*) hv);
  
  jclass r_ret_class = env->FindClass("com/stock_client_login/VerificationValues");
  jmethodID construct = env->GetMethodID(r_ret_class, "<init>", "([B[B[B[B)V");
  jobject retval = env->NewObject(r_ret_class, construct, sk_array, m1_array, m2_array, hv_array);

  return retval;
}
