//
// Created by hh on 2022/12/12.
//

#include "STLeaf.h"


STLeaf::STLeaf(unsigned short num, cod co_d, saxt prefix, size_t size) {
  this->num = num;
  this->co_d = co_d;
  co_size = co_d * sizeof(saxt_type);
  noco_size = saxt_size - co_size + 8;
  memcpy(this->prefix, prefix, saxt_size);
  rep = new char[size];
  ismmap = false;
}

STLeaf::~STLeaf() {
  if (!ismmap) delete rep;
}

char* STLeaf::Get_rep(int i) { return rep + i * noco_size; }


STLeaf::STLeaf(size_t size) { ismmap = false; rep = new char[size]; }

void STLeaf::Set(unsigned short num, cod co_d, saxt prefix) {
  this->num = num;
  this->co_d = co_d;
  co_size = co_d * sizeof(saxt_type);
  noco_size = saxt_size - co_size + 8;
  memcpy(this->prefix, prefix, saxt_size);
}
void STLeaf::Setrep(const char* newrep) {
  if (newrep!=rep){
    ismmap = true;
    delete rep;
    rep = const_cast<char*>(newrep);
  }
}
