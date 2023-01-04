//
// Created by hh on 2023/1/2.
//

#include "query_heap.h"


namespace leveldb {
bool query_heap::push(ares& ares_) {
  if (res_heap.size() < k) {
    res_heap.push(ares_);
    return true;
  } else if (ares_ < res_heap.top()) {
    res_heap.pop();
    res_heap.push(ares_);
    return true;
  }
  return false;
}

void query_heap::get(vector<ares>& res) {
  res.reserve(res_heap.size());
  for (int i = 0; i < res_heap.size(); i++) {
    res.push_back(res_heap.top());
    res_heap.pop();
  }
}

int query_heap::need() const { return k - res_heap.size(); }

float query_heap::top() const {
  if (res_heap.empty()) return MAXFLOAT;
  return res_heap.top().dist;
}

bool query_heap::isk() const { return res_heap.size() == k; }
void query_heap::readLock() { mutex_.lock_shared(); }
void query_heap::readUnlock() { mutex_.unlock_shared(); }
void query_heap::writeLock() { mutex_.lock_upgrade(); }
void query_heap::writeUnlock() { mutex_.unlock_upgrade(); }

bool query_heap::isfinish() {
  if (!use && res_heap.size() == k) {
    cv.notify_one();
    return true;
  }
  return false;
}
void query_heap::subUse() { use--; }
bool query_heap::subOver() {
  if (!(--over)) {
    cv.notify_one();
    return true;
  }
  return false;
}

void query_heap::wait() {
  std::unique_lock<std::mutex> l(mu_);
  cv.wait(l);
}
}