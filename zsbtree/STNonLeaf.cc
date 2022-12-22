//
// Created by hh on 2022/12/12.
//

#include "STNonLeaf.h"


STNonLeaf::STNonLeaf(unsigned short num, cod co_d, size_t size) {
  this->num = num;
  this->co_d = co_d;
  co_size = co_d * sizeof(saxt_type);
  lkey_size = saxt_size - co_size;
  pos_size = (lkey_size << 1) + 2;
  lkey_size += 2;
  noco_size = pos_size + 8;
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

void STNonLeaf::Set(unsigned short num, cod co_d, size_t size) {
  this->num = num;
  this->co_d = co_d;
  co_size = co_d * sizeof(saxt_type);
  lkey_size = saxt_size - co_size;
  pos_size = (lkey_size << 1) + 2;
  lkey_size += 2;
  noco_size = pos_size + 8;
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
  return ((STkeyinfo*)(rep + i * noco_size))->GetCo_d();
}
int STNonLeaf::Getnum(int i) {
  return ((STkeyinfo*)(rep + i * noco_size))->GetNum();
}
saxt STNonLeaf::Get_lsaxt(int i) { return (saxt)(rep + i * noco_size + 2); }
saxt STNonLeaf::Get_rsaxt(int i) { return (saxt)(rep + i * noco_size + lkey_size); }

STpos STNonLeaf::Get_pos(int i) {
  return *((STpos*)(rep + i * noco_size + pos_size));
}

void STNonLeaf::Setnewroom(size_t size) {
  rep = new char[size];
  ismmap = false;
}

void STNonLeaf::Setprefix(saxt prefix, saxt stleafkey, cod co_size,
                          cod noco_size) {
  memcpy(this->prefix, prefix, co_size);
  memcpy(((char*)(this->prefix))+co_size, stleafkey, noco_size);
}
void STNonLeaf::Setprefix(saxt prefix1) {
  memcpy(prefix, prefix1, saxt_size);
}

