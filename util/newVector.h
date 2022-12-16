//
// Created by hh on 2022/12/2.
//

#ifndef LEVELDB_NEWVECTOR_H
#define LEVELDB_NEWVECTOR_H


#include "vector"
#include <bits/valarray_before.h>
#include "cstring"

//输入用的，是不能变化的
template <typename T>
class newVector {
 public:
  typedef T value_type;

  newVector(newVector<value_type>& v, int l, int r) {
    this->v = v.v;
    this->l = v.l + l;
    this->r = v.l + r;
    size_ = r - l;
    topush_pos = l;
  }

  explicit newVector(std::vector<value_type>& v) {
    this->v = v;
    this->l = v.data();
    this->r = v.data() + v.size();
    size_ = v.size();
    topush_pos = l;
  }

  newVector(std::vector<value_type>& v, int l, int r) {
    this->v = v;
    this->l = v.data() + l;
    this->r = v.data() + r;
    size_ = r - l;
    topush_pos = l;
  }

  value_type* data() {
    return l;
  }

  value_type* begin() {
    return l;
  }

  value_type* end() {
    return r;
  }

  T& back() {
    return *(r - 1);
  }

  size_t size() {return size_;}

  T& operator[] (size_t n) {
    return *(l + n);
  }

  void resize(size_t size) {
    topush_pos = l + size;
  }

  void restart(value_type* ll) {
    l = ll;
    r = ll + size_;
  }

  bool push_back(value_type& vv) {
    if (v == r) return false;
    memcpy(topush_pos, &vv, sizeof(value_type));
    topush_pos++;
    return true;
  }

  int size_add() {
    return (topush_pos - l) / sizeof(value_type);
  }

  value_type* back_add() {
    return topush_pos - 1;
  }

  bool empty_add() {
    return topush_pos == l;
  }



  value_type* topush_pos;
  value_type* l;
  value_type* r;
  size_t size_;
  std::vector<value_type>& v;
};




#endif  // LEVELDB_NEWVECTOR_H
