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

  STNonLeaf(unsigned short num, cod co_d, size_t size);
  STNonLeaf(size_t size);
  void Set(unsigned short num, cod co_d, size_t size);
  void Setrep(const char* newrep);
  void Setisleaf();
  void Setnewroom(size_t size);

  void Setprefix(saxt prefix, saxt stleafkey, cod co_size, cod noco_size);
  void Setprefix(saxt prefix1);
  ~STNonLeaf();

  inline void SetSaxt(saxt dst, saxt saxt_)  {
    memcpy(dst, prefix, co_size);
    memcpy(((char*)dst) + co_size, saxt_, s_co_size);
  }


  cod Get_co_d(int i);
  int Getnum(int i);
  saxt Get_lsaxt(int i);

  saxt Get_rsaxt(int i);

  STpos Get_pos(int i);


  unsigned short num;
  cod co_d;
  cod co_size;
  cod s_co_size;
  cod lkey_size;
  cod pos_size;
  cod noco_size;
  bool isleaf;
  bool ismmap;
  size_t size;
  saxt_type prefix[Bit_cardinality];
  char* rep;
};

#endif  // LEVELDB_STNONLEAF_H
