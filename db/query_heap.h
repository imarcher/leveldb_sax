//
// Created by hh on 2023/1/2.
//

#ifndef LEVELDB_QUERY_HEAP_H
#define LEVELDB_QUERY_HEAP_H

#include "globals.h"
#include "queue"
#include "mutex"
#include "boost/thread/shared_mutex.hpp"
#include "condition_variable"
#include "mem_version_set.h"
#include "version_set.h"

using namespace std;
namespace leveldb {
class query_heap {
 public:
  query_heap(int k, int tableNum) : k(k), use(tableNum), over(tableNum), vv1(nullptr), vv2(nullptr){}

  ~query_heap() {
    if (vv1 != nullptr) {
      v1_mutex->Lock();
      vv1->Unref();
      v1_mutex->Unlock();
    }
    if( vv2 != nullptr) {
      v2_mutex->Lock();
      vv2->Unref();
      v2_mutex->Unlock();
    }
  }

  //成功返回true
  bool push(ares& ares_);

  void get(vector<ares>& res);

  int need() const;

  bool isk() const;

  float top() const;

  inline void readLock() { mutex_.lock_shared(); }

  inline void readUnlock() { mutex_.unlock_shared(); }

  inline void writeLock() { mutex_.lock_upgrade(); }

  inline void writeUnlock() { mutex_.unlock_upgrade(); }


  inline void subUse() { use--; }

  // 这个表结束了
  inline bool subOver() {
    if (!(--over)) {
      cv.notify_one();
      return true;
    }
    return false;
  }

  void wait();

  bool isfinish();

 private:
  boost::shared_mutex mutex_;
  int k;
  priority_queue<ares> res_heap;
  int use;   // 有多少个表没做完第一个叶结点
  int over;  // 有多少个表没结束
  std::mutex mu_;
  std::condition_variable cv;

 public:
  // 记录版本和对应锁
  port::Mutex* v1_mutex;
  port::Mutex* v2_mutex;
  mem_version* vv1;
  Version* vv2;
};

}

#endif  // LEVELDB_QUERY_HEAP_H
