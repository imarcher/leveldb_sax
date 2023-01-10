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
  if (!res_heap.empty()) {
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


bool query_heap::isfinish() {
  if (!use && res_heap.size() == k) {
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