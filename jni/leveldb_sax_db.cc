//
// Created by hh on 2022/12/26.
//

#include "leveldb_sax_db.h"
#include "threadPool_2.h"
#include "leveldb/db.h"


static leveldb::DB* db;
static leveldb::WriteOptions writeOptions;
static leveldb::ReadOptions readOptions;
static JavaVM* gs_jvm;
//static ThreadPool *pool;

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

JNIEXPORT void JNICALL Java_leveldb_1sax_db_paa_1saxt_1from_1ts
    (JNIEnv *env, jobject, jbyteArray ts, jbyteArray saxt_out, jfloatArray paa_out) {
  ts_only tsArr;
  saxt_only saxtArr;
  memset(saxtArr.asaxt, 0, sizeof(saxt_only));
  paa_only paaArr;

  env->GetByteArrayRegion(ts, 0, sizeof(ts_only), (jbyte*)tsArr.ts);
  paa_saxt_from_ts(tsArr.ts, saxtArr.asaxt, paaArr.apaa);

  env->SetByteArrayRegion(saxt_out, 0, sizeof(saxt_only), (jbyte*)saxtArr.asaxt);
  env->SetFloatArrayRegion(paa_out, 0, sizeof(paa_only), paaArr.apaa);
}


JNIEXPORT void JNICALL Java_leveldb_1sax_db_open
    (JNIEnv *env, jobject, jstring dbname) {
//  pool = new ThreadPool(30);
  //把java环境拿到
  env->GetJavaVM(&gs_jvm);
  const char* str;
  jboolean isCopy;
  str = env->GetStringUTFChars(dbname, &isCopy);
  if(str == nullptr) return;
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, str, gs_jvm, &db);
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
//  pool->enqueue(std::bind(&leveldb::DB::Put, db, writeOptions, key));
  db->Put(writeOptions, key);
}

JNIEXPORT jbyteArray JNICALL Java_leveldb_1sax_db_Get
    (JNIEnv *env, jobject, jbyteArray aquery_, jboolean is_use_am, jint am_version_id, jint st_version_id, jlongArray st_number) {
  aquery aquery1;
  vector<uint64_t> st_numbers;
  vector<ares> results;
  env->GetByteArrayRegion(aquery_, 0, sizeof(aquery), (jbyte*)&aquery1);
  size_t len = env->GetArrayLength(st_number);
  if (len) {
    int len_ = len / sizeof(uint64_t);
    st_numbers.reserve(len_);
    st_numbers.resize(len_);
    env->GetLongArrayRegion(st_number, 0, len, (jlong*)st_numbers.data());
  }
  db->Get(aquery1, is_use_am, am_version_id, st_version_id, st_numbers, results);
  jbyteArray newArr = env->NewByteArray(results.size()*sizeof(ares));
  env->SetByteArrayRegion(newArr, 0, results.size()*sizeof(ares), (jbyte*)results.data());
  return newArr;
}


JNIEXPORT void JNICALL Java_leveldb_1sax_db_unref_1am
    (JNIEnv *, jobject, jint id) {
  db->UnRef_am(id);
}


JNIEXPORT void JNICALL Java_leveldb_1sax_db_unref_1st
    (JNIEnv *, jobject, jint id) {
  db->UnRef_st(id);
}
