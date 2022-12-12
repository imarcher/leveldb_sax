//
// Created by hh on 2022/12/12.
//

#include "STNonLeaf.h"


STNonLeaf::STNonLeaf(unsigned short num, cod co_d, saxt prefix, size_t size) {
  this->num = num;
  this->co_d = co_d;
  co_size = co_d * sizeof(saxt_type);
  lkey_size = saxt_size - co_size;
  pos_size = lkey_size << 1;
  noco_size = pos_size + 8;
  memcpy(this->prefix, prefix, saxt_size);
  rep = new char[size];
  this->size = size;
}

void STNonLeaf::Setisleaf() { isleaf = rep + size - 1; }

STNonLeaf::~STNonLeaf() {
  free(rep);
}

cod STNonLeaf::Get_co_d(int i) {
  return ((STkeyinfo*)rep + i * noco_size)->GetCo_d();
}
int STNonLeaf::Getnum(int i) {
  return ((STkeyinfo*)rep + i * noco_size)->GetNum();
}
saxt STNonLeaf::Get_lsaxt(int i) { return (saxt)rep + i * noco_size + 2; }
saxt STNonLeaf::Get_rsaxt(int i) { return (saxt)rep + i * noco_size + lkey_size+ 2; }
STpos STNonLeaf::Get_pos(int i) { return *((STpos*)rep + i * noco_size + pos_size + 2); }

