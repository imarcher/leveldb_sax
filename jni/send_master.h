//
// Created by hh on 2022/12/30.
//

#ifndef UNTITLED_SEND_MASTER_H
#define UNTITLED_SEND_MASTER_H


#include <jni.h>
#include <chrono>

static JavaVM* gs_jvm = nullptr;

static inline void charcpy(char*& tmp_p, const void* obj, const size_t size_) {
  memcpy(tmp_p, obj, size_);
  tmp_p += size_;
}

static void send_master(char* a, size_t size_) {
    //获取环境
    JNIEnv *env;
    assert(gs_jvm!= nullptr);
    gs_jvm->AttachCurrentThread((void **)&env, NULL);
//
    jclass cls = env->FindClass("leveldb_sax/db_send");
    jmethodID mid = env->GetStaticMethodID(cls, "send_edit", "([B)V");

    jbyteArray newArr = env->NewByteArray(sizeof(size_));
    env->SetByteArrayRegion(newArr, 0, sizeof(size_), (jbyte*)a);

    env->CallStaticObjectMethod(cls, mid, newArr);
}








#endif //UNTITLED_SEND_MASTER_H
