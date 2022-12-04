//
// Created by hh on 2022/12/2.
//

#ifndef LEVELDB_NEWVECTOR_H
#define LEVELDB_NEWVECTOR_H


#include "vector"
#include <bits/valarray_before.h>

//输入用的，是不能变化的
template <typename T>
class newVector {
 public:
  typedef T value_type;

  newVector(std::vector<value_type>& v) {
    this->v = v;
    this->l = v.data();
    this->r = v.data() + v.size();
    size_ = v.size();
  }

  newVector(std::vector<value_type>& v, int l, int r) {
      this->v = v;
      this->l = v.data() + l;
      this->r = v.data() + r;
      size_ = r - l;
  }


  newVector(newVector<value_type>& v, int l, int r) {
    this->v = v.v;
    this->l = v.data() + l;
    this->r = v.data() + r;
    size_ = r - l;
  }

  value_type* data(){
    return l;
  }

  value_type* begin(){
    return l;
  }

  value_type* end(){
    return r;
  }

  T& back(){
    return *(r - 1);
  }

  size_t size() {return size_;}

  T& operator[] (size_t n)
  {
    return v[l + n];
  }






  //左闭右开
  value_type* l;
  value_type* r;
  size_t size_;
  std::vector<value_type>& v;
};

#endif  // LEVELDB_NEWVECTOR_H
