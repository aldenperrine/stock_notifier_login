/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_stock_client_login_ClientBridge */

#ifndef _Included_com_stock_client_login_ClientBridge
#define _Included_com_stock_client_login_ClientBridge
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_stock_client_login_ClientBridge
 * Method:    jni_init_client_lib
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1init_1client_1lib
  (JNIEnv *, jclass);

/*
 * Class:     com_stock_client_login_ClientBridge
 * Method:    jni_lib_bytes_size
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1lib_1bytes_1size
  (JNIEnv *, jclass);

/*
 * Class:     com_stock_client_login_ClientBridge
 * Method:    jni_lib_key_size
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1lib_1key_1size
  (JNIEnv *, jclass);

/*
 * Class:     com_stock_client_login_ClientBridge
 * Method:    jni_lib_hash_size
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1lib_1hash_1size
  (JNIEnv *, jclass);

/*
 * Class:     com_stock_client_login_ClientBridge
 * Method:    jni_generate_registration
 * Signature: (Ljava/lang/String;)Lcom/stock_client_login/RegistrationValues;
 */
JNIEXPORT jobject JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1generate_1registration
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_stock_client_login_ClientBridge
 * Method:    jni_generate_a
 * Signature: ()Lcom/stock_client_login/AValues;
 */
JNIEXPORT jobject JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1generate_1a
  (JNIEnv *, jclass);

/*
 * Class:     com_stock_client_login_ClientBridge
 * Method:    jni_generate_ck
 * Signature: (Ljava/lang/String;Ljava/lang/String;[B[B[B[B[B)Lcom/stock_client_login/VerificationValues;
 */
JNIEXPORT jobject JNICALL Java_com_stock_1client_1login_ClientBridge_jni_1generate_1ck
  (JNIEnv *, jclass, jstring, jstring, jbyteArray, jbyteArray, jbyteArray, jbyteArray, jbyteArray);

#ifdef __cplusplus
}
#endif
#endif
