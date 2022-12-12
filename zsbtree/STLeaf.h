//
// Created by hh on 2022/12/12.
//

#ifndef LEVELDB_STLEAF_H
#define LEVELDB_STLEAF_H

#include <globals.h>
#include "cstring"

class STLeaf {
 public:

  STLeaf(unsigned short num, cod co_d, saxt prefix, size_t size);

  ~STLeaf();

  char* Get_rep(int i);

  unsigned short num;
  cod co_d;
  cod co_size;
  cod noco_size;
  saxt_type prefix[Bit_cardinality];
  char* rep;

};

#endif  // LEVELDB_STLEAF_H
