//
// Created by hh on 2022/12/2.
//

#ifndef LEVELDB_NEWVECTOR_H
#define LEVELDB_NEWVECTOR_H


#include "cstring"
#include "vector"

template <typename T>
class newVector {
 public:

  newVector(newVector<T>& v, int l, int r) {
      this->l = v.l + l;
      this->r = v.l + r;
      size_ = r - l;
      topush_pos = this->l;
  }

  newVector(std::vector<T>& v) {

      this->l = v.data();
      this->r = v.data() + v.size();
      size_ = v.size();
      topush_pos = l;
  }

  newVector(std::vector<T>& v, int l, int r) {

      this->l = v.data() + l;
      this->r = v.data() + r;
      size_ = r - l;
      topush_pos = this->l;
  }

  T* data() {
      return l;
  }

  T* begin() {
      return l;
  }

  T* end() {
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

  void restart(T* ll) {
      l = ll;
      r = ll + size_;
  }

  bool push_back(T& vv) {
      if (topush_pos == r) return false;
      memcpy(topush_pos, &vv, sizeof(T));
      topush_pos++;
      return true;
  }

  int size_add() {
      return (topush_pos - l) / sizeof(T);
  }

  T* back_add() {
      return topush_pos - 1;
  }

  bool empty_add() {
      return topush_pos == l;
  }



  T* topush_pos;
  T* l;
  T* r;
  size_t size_;

};



#endif  // LEVELDB_NEWVECTOR_H
