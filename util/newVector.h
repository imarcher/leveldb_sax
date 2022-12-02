//
// Created by hh on 2022/12/2.
//

#ifndef LEVELDB_NEWVECTOR_H
#define LEVELDB_NEWVECTOR_H

template <typename T>
class newVector {
 public:
  typedef T value_type;
  typedef T* iterator;
 private:
  value_type* _data;
  size_t _size;
  size_t _capacity;





};

#endif  // LEVELDB_NEWVECTOR_H
