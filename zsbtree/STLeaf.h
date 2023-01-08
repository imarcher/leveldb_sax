//
// Created by hh on 2022/12/12.
//

#ifndef LEVELDB_STLEAF_H
#define LEVELDB_STLEAF_H

#include <globals.h>
#include "cstring"
#include "LeafKey.h"

class STLeaf {
 public:

  STLeaf(unsigned short num, size_t size);
  STLeaf(size_t size);

  void Setrep(const char* newrep);
  void Setnewroom(size_t size);
  void Set(unsigned short num);
  ~STLeaf();

  char* Get_rep(int i);

  unsigned short num;
  bool ismmap;
  char* rep;

};

#endif  // LEVELDB_STLEAF_H
