/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class leveldb_sax_db */

#ifndef _Included_leveldb_sax_db
#define _Included_leveldb_sax_db
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     leveldb_sax_db
 * Method:    saxt_from_ts
 * Signature: ([B)[B
 */
JNIEXPORT jbyteArray JNICALL Java_leveldb_1sax_db_saxt_1from_1ts
  (JNIEnv *, jobject, jbyteArray);

/*
 * Class:     leveldb_sax_db
 * Method:    open
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_leveldb_1sax_db_open
  (JNIEnv *, jobject, jstring);

/*
 * Class:     leveldb_sax_db
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_leveldb_1sax_db_close
  (JNIEnv *, jobject);

/*
 * Class:     leveldb_sax_db
 * Method:    init
 * Signature: ([BI)V
 */
JNIEXPORT void JNICALL Java_leveldb_1sax_db_init
  (JNIEnv *, jobject, jbyteArray, jint);

/*
 * Class:     leveldb_sax_db
 * Method:    put
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_leveldb_1sax_db_put
  (JNIEnv *, jobject, jbyteArray);

#ifdef __cplusplus
}
#endif
#endif