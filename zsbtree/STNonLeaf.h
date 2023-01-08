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

  STNonLeaf(unsigned short num, size_t size);
  STNonLeaf(size_t size);
  void Set(unsigned short num, size_t size);
  void Setrep(const char* newrep);
  void Setisleaf();
  void Setnewroom(size_t size);


  ~STNonLeaf();




  cod Get_co_d(int i);
  int Getnum(int i);
  saxt_only Get_lsaxt(int i);

  saxt_only Get_rsaxt(int i);

  STpos Get_pos(int i);


  unsigned short num;
  bool isleaf;
  bool ismmap;
  size_t size;
  char* rep;
};

#endif  // LEVELDB_STNONLEAF_H
