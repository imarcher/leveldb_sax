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

  STLeaf(unsigned short num, cod co_d, size_t size);
  STLeaf(size_t size);

  void Setrep(const char* newrep);
  void Setnewroom(size_t size);
  void Set(unsigned short num, cod co_d);
  void Setprefix(saxt prefix, saxt stleafkey, cod co_size, cod noco_size);
  void Setprefix(saxt prefix1);
  inline void SetLeafKey(LeafKey* dst, int id);
  ~STLeaf();

  char* Get_rep(int i);

  unsigned short num;
  cod co_d;
  cod co_size;
  cod noco_size;
  bool ismmap;
  saxt_type prefix[Bit_cardinality];
  char* rep;

};

#endif  // LEVELDB_STLEAF_H
