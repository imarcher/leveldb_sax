//
// Created by hh on 2022/12/26.
//

#include "leveldb_sax_db.h"
#include "leveldb/db.h"
#include "send_master.h"

static leveldb::DB* db;
static leveldb::WriteOptions writeOptions;
static leveldb::ReadOptions readOptions;

JNIEXPORT void JNICALL Java_leveldb_1sax_db_saxt_1from_1ts
    (JNIEnv *env, jobject, jbyteArray ts, jbyteArray saxt_out) {

  ts_only tsArr;
  saxt_only saxtArr;
  memset(saxtArr.asaxt, 0, sizeof(saxt_only));
  env->GetByteArrayRegion(ts, 0, sizeof(ts_only), (jbyte*)tsArr.ts);
  saxt_from_ts(tsArr.ts, saxtArr.asaxt);
//  jbyteArray newArr = env->NewByteArray(sizeof(saxt_only));
//  env->SetByteArrayRegion(newArr, 0, sizeof(saxt_only), (jbyte*)saxtArr.asaxt);
  env->SetByteArrayRegion(saxt_out, 0, sizeof(saxt_only), (jbyte*)saxtArr.asaxt);
//  return newArr;
}


JNIEXPORT void JNICALL Java_leveldb_1sax_db_open
    (JNIEnv *env, jobject, jstring dbname) {
  //把java环境拿到
  env->GetJavaVM(&gs_jvm);
  const char* str;
  jboolean isCopy;
  str = env->GetStringUTFChars(dbname, &isCopy);
  if(str == nullptr) return;
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, str, &db);
  env->ReleaseStringUTFChars(dbname, str);
  assert(status.ok());
}


JNIEXPORT void JNICALL Java_leveldb_1sax_db_close
    (JNIEnv *, jobject) {
  delete db;
}


JNIEXPORT void JNICALL Java_leveldb_1sax_db_init
    (JNIEnv *env, jobject, jbyteArray leafTimeKeys, jint leafKeysNum) {
  int len = sizeof(LeafTimeKey)*leafKeysNum;
  LeafTimeKey* data = (LeafTimeKey*)malloc(len);
  env->GetByteArrayRegion(leafTimeKeys, 0, len, (jbyte*)data);
  db->Init(data, leafKeysNum);
  free(data);
}


JNIEXPORT void JNICALL Java_leveldb_1sax_db_put
    (JNIEnv *env, jobject, jbyteArray leafTimeKey) {
  LeafTimeKey key;
  env->GetByteArrayRegion(leafTimeKey, 0, sizeof(LeafTimeKey), (jbyte*)&key);
  db->Put(writeOptions, key);
}
