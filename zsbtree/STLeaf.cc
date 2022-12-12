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
}

STLeaf::~STLeaf() {
  free(rep);
}

char* STLeaf::Get_rep(int i) { return rep + i * noco_size; }
