//
// Created by hh on 2022/12/12.
//

#ifndef LEVELDB_STNONLEAF_H
#define LEVELDB_STNONLEAF_H


#include <globals.h>
#include "cstring"
#include "STpos.h"
#include "STkeyinfo.h"
class STNonLeaf {
 public:

  STNonLeaf(unsigned short num, cod co_d, saxt prefix, size_t size);

  void Setisleaf();

  ~STNonLeaf();

  cod Get_co_d(int i);
  int Getnum(int i);
  saxt Get_lsaxt(int i);

  saxt Get_rsaxt(int i);

  STpos Get_pos(int i);


  unsigned short num;
  cod co_d;
  cod co_size;
  cod lkey_size;
  cod pos_size;
  cod noco_size;
  bool isleaf;
  size_t size;
  saxt_type prefix[Bit_cardinality];
  char* rep;
};

#endif  // LEVELDB_STNONLEAF_H
