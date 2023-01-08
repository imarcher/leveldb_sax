//
// Created by hh on 2022/12/12.
//

#include "STNonLeaf.h"


STNonLeaf::STNonLeaf(unsigned short num, size_t size) {
  this->num = num;
  rep = new char[size];
  this->size = size;
  isleaf = false;
  ismmap = false;
}

STNonLeaf::STNonLeaf(size_t size) {
  rep = new char[size];
  ismmap = false;
  isleaf = false;
}

void STNonLeaf::Set(unsigned short num, size_t size) {
  this->num = num;
  this->size = size;
}

void STNonLeaf::Setrep(const char* newrep) {
  if (newrep!=rep){
    if (!ismmap)
      delete rep;
    ismmap = true;
    rep = const_cast<char*>(newrep);
  }
}

void STNonLeaf::Setisleaf() { isleaf = *(bool*)(rep + size - 1); }

STNonLeaf::~STNonLeaf() {
  if (!ismmap) delete rep;
}

cod STNonLeaf::Get_co_d(int i) {
  return ((NonLeafKey*)rep)[i].co_d;
}
int STNonLeaf::Getnum(int i) {
  return ((NonLeafKey*)rep)[i].num;
}

saxt_only STNonLeaf::Get_lsaxt(int i) { return ((NonLeafKey*)rep)[i].lsaxt; }
saxt_only STNonLeaf::Get_rsaxt(int i) { return ((NonLeafKey*)rep)[i].rsaxt; }

STpos STNonLeaf::Get_pos(int i) {
  return *((STpos*)(&(((NonLeafKey*)rep)[i].p)));
}

void STNonLeaf::Setnewroom(size_t size) {
  rep = new char[size];
  ismmap = false;
}



